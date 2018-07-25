#!/bin/bash

set -e

INPUT_DATA_FILE="/tmp/nxs-chat-srv_input-data.yml"
CONF_FILE="/etc/nxs-chat-srv/nxs-chat-srv.conf"
TPL_CONF_FILE="/usr/share/nxs-chat-srv/nxs-chat-srv.conf.j2"

# usage: file_env VAR [DEFAULT]
#    ie: file_env 'XYZ_DB_PASSWORD' 'example'
# (will allow for "$XYZ_DB_PASSWORD_FILE" to fill in the value of
#  "$XYZ_DB_PASSWORD" from a file, especially for Docker's secrets feature)
file_env() {
	local var="$1"
	local fileVar="${var}_FILE"
	local def="${2:-}"
	if [ "${!var:-}" ] && [ "${!fileVar:-}" ]; then
		echo >&2 "error: both $var and $fileVar are set (but are exclusive)"
		exit 1
	fi
	local val="$def"
	if [ "${!var:-}" ]; then
		val="${!var}"
	elif [ "${!fileVar:-}" ]; then
		val="$(< "${!fileVar}")"
	fi
	export "$var"="$val"
	unset "$fileVar"
}

function file_env_input_data()
{
	local var="$1"
	file_env "${var}"

	local value="${!var}"
	local key=`echo "${var}" | tr '[:upper:]' '[:lower:]'`

	if [ ! -z "${value}" ];
	then
		echo "${key}: ${value}" >> ${INPUT_DATA_FILE}
	fi
}

if [ -f "${INPUT_DATA_FILE}" ];
then
	rm -f ${INPUT_DATA_FILE}
fi

file_env 'NXS_CHAT_SRV_INITIALIZE'
file_env 'REDIS_NODES'
file_env 'DEV_ACCOUNTS'

file_env_input_data 'PROC_QUEUE_WORKER_TERM_TIMEOUT'
file_env_input_data 'PROC_REST_API_TERM_TIMEOUT'
file_env_input_data 'LOGGING_LEVEL'
file_env_input_data 'BIND_PORT'
file_env_input_data 'BIND_SSL_USE_SSL'
file_env_input_data 'BIND_SSL_CRT'
file_env_input_data 'BIND_SSL_KEY'
file_env_input_data 'TELEGRAM_BOT_API_KEY'
file_env_input_data 'TELEGRAM_AUTH_TOKEN'
file_env_input_data 'TELEGRAM_WEBHOOK_HOST'
file_env_input_data 'TELEGRAM_PROXY_TYPE'
file_env_input_data 'TELEGRAM_PROXY_HOST'
file_env_input_data 'TELEGRAM_PROXY_PORT'
file_env_input_data 'TELEGRAM_PROXY_AUTH_TYPE'
file_env_input_data 'TELEGRAM_PROXY_AUTH_USER'
file_env_input_data 'TELEGRAM_PROXY_AUTH_PASS'
file_env_input_data 'MYSQL_NAME'
file_env_input_data 'MYSQL_USER'
file_env_input_data 'MYSQL_PASS'
file_env_input_data 'MYSQL_HOST'
file_env_input_data 'MYSQL_PORT'
file_env_input_data 'REDIS_KEYS_SPACE'
file_env_input_data 'REDIS_CLUSTER'
file_env_input_data 'REDMINE_URL'
file_env_input_data 'REDMINE_HOST'
file_env_input_data 'REDMINE_API_KEY'
file_env_input_data 'REDMINE_AUTH_TOKEN'
file_env_input_data 'REDMINE_PRESALE_API_KEY'
file_env_input_data 'REDMINE_PRESALE_PROJECT_NAME'
file_env_input_data 'REDMINE_ISSUE_LIST_QUERY_ID'
file_env_input_data 'REDMINE_SSL_VERIFYHOST'
file_env_input_data 'REDMINE_CF_TELEGRAM'
file_env_input_data 'REDMINE_CF_WATCHERS'
file_env_input_data 'REDMINE_CF_IGNORE_STATUS'
file_env_input_data 'REDMINE_STATUS_IN_PROGRESS'
file_env_input_data 'REDMINE_STATUS_RESOLVED'
file_env_input_data 'REDMINE_STATUS_NEED_FEEDBACK'
file_env_input_data 'RA_QUEUE_POP_TIMEOUT_MS'
file_env_input_data 'RA_QUEUE_QUEUE_WORKERS'
file_env_input_data 'CACHE_PROJECTS_TTL'
file_env_input_data 'CACHE_USERS_TTL'
file_env_input_data 'CACHE_ISSUE_PRIORITIES_TTL'
file_env_input_data 'ATTACHMENTS_TLGRM_DOWNLOAD_TMP_DIR'
file_env_input_data 'ATTACHMENTS_RDMN_DOWNLOAD_TMP_DIR'
file_env_input_data 'STATISTIC_AUTH_TOKEN'
file_env_input_data 'LABELS_DEFAULT_LANG'
file_env_input_data 'LABELS_LABELS_PATH'
file_env_input_data 'GREETINGS_LANG_1'
file_env_input_data 'GREETINGS_TEXT_1'
file_env_input_data 'GREETINGS_LANG_2'
file_env_input_data 'GREETINGS_TEXT_2'

# Redis nodes
if [ -z ${REDIS_NODES} ];
then
	REDIS_NODES="127.0.0.1:6379"
fi

echo "redis_nodes:" >> ${INPUT_DATA_FILE}
IFS=',' read -ra node <<< "${REDIS_NODES}"; unset IFS
for n in "${node[@]}"; do
	IFS=':' read -ra entry <<< "${n}"; unset IFS
	echo -e "- host: ${entry[0]}\n  port: ${entry[1]}" >> ${INPUT_DATA_FILE}
done

# Dev accounts
if [ ! -z "${DEV_ACCOUNTS}" ];
then
	echo "dev_accounts:" >> ${INPUT_DATA_FILE}
	IFS=',' read -ra account <<< "${DEV_ACCOUNTS}"; unset IFS
	for a in "${account[@]}"; do
		echo "- ${a}" >> ${INPUT_DATA_FILE}
	done
fi

jinja2 ${TPL_CONF_FILE} ${INPUT_DATA_FILE} > ${CONF_FILE}

if [ "$NXS_CHAT_SRV_INITIALIZE" ];
then
	IFS=',' read -ra inits <<< "${NXS_CHAT_SRV_INITIALIZE}"; unset IFS
	for i in "${inits[@]}"; do
		"$@" -i "${i}"
	done
fi

exec "$@"
