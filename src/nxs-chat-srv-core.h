#ifndef _INCLUDE_NXS_CHAT_SRV_CORE_H
#define _INCLUDE_NXS_CHAT_SRV_CORE_H

// clang-format off

#include <hiredis/hiredis.h>
#include <hiredis/hircluster.h>

/* Project version */
#define NXS_CHAT_SRV_VERSION						"0.3.1"

/* Project basic global definitions */

#define	NXS_CHAT_SRV_CONF_PATH_DEFAULT					"/etc/nxs-chat-srv/nxs-chat-srv.conf"

#define	NXS_CHAT_SRV_CONF_LABELS_DEFAULT_PATH				"/usr/share/nxs-chat-srv/labels/"
#define	NXS_CHAT_SRV_CONF_LABELS_DEFAULT_LANG				"en"

#define NXS_CHAT_SRV_GREETING_DEFAULT					"%{userfname}, welcome to our customer care system chat!\nWe are looking forward to your issues."

#define NXS_CHAT_SRV_REST_API_MAX_HEADER_SIZE				1048576		/* 1 MB */

#define NXS_CHAT_SRV_TLGRM_MAX_MESSAGE_SIZE				3584		/* 3.5 K */
#define NXS_CHAT_SRV_TLGRM_MAX_MESSAGE_PREVIEW				256

#define NXS_CHAT_SRV_RDMN_QUERY_LIMIT					100

#define NXS_CHAT_SRV_TLGRM_PROJECTS_LIMIT				5
#define NXS_CHAT_SRV_TLGRM_ISSUES_LIMIT					5

#define NXS_CHAT_SRV_UTF8_PRIVATE_MESSAGE				0xF0, 0x9F, 0x94, 0x95, 0x0

#define NXS_CHAT_SRV_TLGRM_MESSAGE_ISSUE_LINK_FMT			"<a href=\"%r/issues/%zu\">[%r - #%zu] %r</a>"

/* Project global typedefs */



/* Project global structures */

typedef enum
{
	NXS_CHAT_SRV_INIT_MODE_NONE			= 0,
	NXS_CHAT_SRV_INIT_MODE_WEBHOOK_SET		= 1,
	NXS_CHAT_SRV_INIT_MODE_WEBHOOK_SET_SSC		= 2,
	NXS_CHAT_SRV_INIT_MODE_MYSQL_CREATE_TABLES	= 4,
} nxs_chat_srv_init_mode_t;

typedef enum
{
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_NONE,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_MESSAGE,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_EDIT_MESSAGE_TEXT,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_ANSWER_CALLBACK_QUERY,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_GET_FILE,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_PHOTO,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_STICKER,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_DOCUMENT,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VOICE,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VIDEO,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_GET_CHAT,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_GET_WEBHOOKINFO,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SET_WEBHOOK,
} nxs_chat_srv_tlgrm_request_type_t;

/* Project errors */
typedef enum
{
	NXS_CHAT_SRV_E_OK,
	NXS_CHAT_SRV_E_WARN,
	NXS_CHAT_SRV_E_ERR,
	NXS_CHAT_SRV_E_PTR,
	NXS_CHAT_SRV_E_INIT,
	NXS_CHAT_SRV_E_EXIT,
	NXS_CHAT_SRV_E_EXIST,
	NXS_CHAT_SRV_E_TIMEOUT,
	NXS_CHAT_SRV_E_TYPE,
	NXS_CHAT_SRV_E_ATTR,
} nxs_chat_srv_err_t;

/* Project config structures */

typedef struct
{
	nxs_string_t				cfg_path;
} nxs_chat_srv_cfg_ctx_t;

typedef struct
{
	nxs_string_t				pid_file;
	size_t					queue_worker_term_timeout;
	size_t					rest_api_term_timeout;
	nxs_bool_t				daemonize;
} nxs_chat_srv_cfg_proc_t;

typedef struct
{
	nxs_string_t				path;
	int					level;
} nxs_chat_srv_cfg_log_t;

typedef struct
{
	nxs_bool_t				use_ssl;
	nxs_string_t				crt;
	nxs_string_t				key;
} nxs_chat_srv_cfg_bind_ssl_t;

typedef struct
{
	nxs_string_t				iface;
	uint16_t				port;
	nxs_chat_srv_cfg_bind_ssl_t		ssl;
} nxs_chat_srv_cfg_bind_t;

typedef struct
{
	nxs_curl_proxy_auth_type_t		type;
	nxs_string_t				username;
	nxs_string_t				password;
} nxs_chat_srv_cfg_proxy_auth_t;

typedef struct
{
	nxs_curl_proxy_type_t			type;
	nxs_string_t				host;
	uint16_t				port;
	nxs_chat_srv_cfg_proxy_auth_t		auth;
} nxs_chat_srv_cfg_proxy_t;

typedef struct
{
	nxs_string_t				bot_api_addr;
	nxs_string_t				bot_api_key;
	nxs_string_t				auth_token;
	nxs_string_t				webhook_host;
	nxs_chat_srv_cfg_proxy_t		proxy;
} nxs_chat_srv_cfg_tlgrm_t;

typedef struct
{
	nxs_string_t				name;
	nxs_string_t				user;
	nxs_string_t				pass;
	nxs_string_t				host;
	uint16_t				port;
} nxs_chat_srv_cfg_mysql_t;

typedef struct
{
	nxs_string_t				host;
	uint16_t				port;
} nxs_chat_srv_cfg_redis_node_t;

typedef struct
{
	nxs_string_t				keys_space;
	nxs_bool_t				is_cluster;
	nxs_array_t				nodes;				/* type: nxs_chat_srv_cfg_redis_node_t */
} nxs_chat_srv_cfg_redis_t;

typedef struct
{
	nxs_string_t				url;
	nxs_string_t				host;
	nxs_string_t				api_key;
	nxs_string_t				auth_token;
	size_t					issue_list_query_id;
	nxs_bool_t				ssl_verifyhost;
	size_t					cf_telegram;
	size_t					cf_watchers;
	size_t					cf_ignore_status;
	size_t					status_in_progress;
	size_t					status_resolved;
	size_t					status_need_feedback;
	nxs_string_t				presale_api_key;
	nxs_string_t				presale_project_name;
} nxs_chat_srv_cfg_rdmn_t;

typedef struct
{
	size_t					pop_timeout_ms;
	size_t					queue_workers;
} nxs_chat_srv_cfg_ra_queue_t;

typedef struct
{
	time_t					projects_ttl;
	time_t					users_ttl;
	time_t					issue_priorities_ttl;
	nxs_string_t				lock_file;
} nxs_chat_srv_cfg_cache_t;

typedef struct
{
	nxs_string_t				tlgrm_download_tmp_dir;
	nxs_string_t				rdmn_download_tmp_dir;
} nxs_chat_srv_cfg_attachments_t;

typedef struct
{
	nxs_string_t				auth_token;
} nxs_chat_srv_cfg_statistic_t;

typedef struct
{
	nxs_string_t				key;
	nxs_string_t				text;
} nxs_chat_srv_cfg_label_t;

typedef struct
{
	nxs_string_t				lang;
	nxs_array_t				labels;				/* type: nxs_chat_srv_cfg_label_t */
} nxs_chat_srv_cfg_labels_set_t;

typedef struct
{
	nxs_string_t				default_lang;
	nxs_chat_srv_cfg_labels_set_t		*default_labels_set;
	nxs_string_t				labels_path;
	nxs_array_t				labels_sets;			/* type: nxs_chat_srv_cfg_labels_set_t */
} nxs_chat_srv_cfg_labels_t;

typedef struct
{
	nxs_string_t				lang;
	nxs_string_t				text;
} nxs_chat_srv_cfg_greeting_t;

typedef struct
{
	nxs_chat_srv_init_mode_t		init_mode;
	nxs_chat_srv_cfg_proc_t			proc;
	nxs_chat_srv_cfg_log_t			log;
	nxs_chat_srv_cfg_bind_t			bind;
	nxs_chat_srv_cfg_tlgrm_t		tlgrm;
	nxs_chat_srv_cfg_mysql_t		mysql;
	nxs_chat_srv_cfg_redis_t		redis;
	nxs_chat_srv_cfg_rdmn_t			rdmn;
	nxs_chat_srv_cfg_ra_queue_t		ra_queue;
	nxs_chat_srv_cfg_cache_t		cache;
	nxs_array_t				dev_accounts;			/* type: nxs_string_t */
	nxs_chat_srv_cfg_attachments_t		attachments;
	nxs_chat_srv_cfg_statistic_t		statistic;
	nxs_chat_srv_cfg_labels_t		labels;
	nxs_array_t				greetings;			/* type: nxs_chat_srv_cfg_greeting_t */
} nxs_chat_srv_cfg_t;

/* Project includes */



/* Global functions for project's core module */



#endif /* _INCLUDE_NXS_CHAT_SRV_CORE_H */
