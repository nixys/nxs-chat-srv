#ifdef USE_NXS_JSON

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <conf/file-json/file-json.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static nxs_cfg_json_state_t nxs_chat_srv_conf_file_json_prep(nxs_cfg_json_t cfg);
static nxs_cfg_json_state_t nxs_chat_srv_conf_file_json_post(nxs_cfg_json_t cfg);

static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_proc(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_log(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_bind(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_bind_ssl(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_tlgrm(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_mysql(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_redis(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_chat_srv_conf_file_json_read_redis_nodes(nxs_process_t *     proc,
                                                                         nxs_json_t *        json,
                                                                         nxs_cfg_json_par_t *cfg_json_par_el,
                                                                         nxs_array_t *       cfg_arr);
static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_rdmn(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_ra_queue(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_cache(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_attachments(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_statistic(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

// clang-format off

/* Module initializations */

static nxs_string_t _s_par_proc				= nxs_string("proc");
static nxs_string_t _s_par_pid_file			= nxs_string("pid_file");
static nxs_string_t _s_par_queue_worker_term_timeout	= nxs_string("queue_worker_term_timeout");
static nxs_string_t _s_par_rest_api_term_timeout	= nxs_string("rest_api_term_timeout");
static nxs_string_t _s_par_logging			= nxs_string("logging");
static nxs_string_t _s_par_level			= nxs_string("level");
static nxs_string_t _s_par_path				= nxs_string("path");
static nxs_string_t _s_par_bind				= nxs_string("bind");
static nxs_string_t _s_par_iface			= nxs_string("iface");
static nxs_string_t _s_par_port				= nxs_string("port");
static nxs_string_t _s_par_ssl				= nxs_string("ssl");
static nxs_string_t _s_par_crt				= nxs_string("crt");
static nxs_string_t _s_par_key				= nxs_string("key");
static nxs_string_t _s_par_telegram			= nxs_string("telegram");
static nxs_string_t _s_par_bot_api_addr			= nxs_string("bot_api_addr");
static nxs_string_t _s_par_bot_api_key			= nxs_string("bot_api_key");
static nxs_string_t _s_par_mysql			= nxs_string("mysql");
static nxs_string_t _s_par_redis			= nxs_string("redis");
static nxs_string_t _s_par_name				= nxs_string("name");
static nxs_string_t _s_par_user				= nxs_string("user");
static nxs_string_t _s_par_pass				= nxs_string("pass");
static nxs_string_t _s_par_host				= nxs_string("host");
static nxs_string_t _s_par_redmine			= nxs_string("redmine");
static nxs_string_t _s_par_api_key			= nxs_string("api_key");
static nxs_string_t _s_par_ra_queue			= nxs_string("ra_queue");
static nxs_string_t _s_par_pop_timeout_ms		= nxs_string("pop_timeout_ms");
static nxs_string_t _s_par_queue_workers		= nxs_string("queue_workers");
static nxs_string_t _s_par_daemonize			= nxs_string("daemonize");
static nxs_string_t _s_par_auth_token			= nxs_string("auth_token");
static nxs_string_t _s_par_cache			= nxs_string("cache");
static nxs_string_t _s_par_lock_file			= nxs_string("lock_file");
static nxs_string_t _s_par_dev_accounts			= nxs_string("dev_accounts");
static nxs_string_t _s_par_projects_ttl			= nxs_string("projects_ttl");
static nxs_string_t _s_par_users_ttl			= nxs_string("users_ttl");
static nxs_string_t _s_par_issue_priorities_ttl		= nxs_string("issue_priorities_ttl");
static nxs_string_t _s_par_ssl_verifyhost		= nxs_string("ssl_verifyhost");
static nxs_string_t _s_par_issue_list_query_id		= nxs_string("issue_list_query_id");
static nxs_string_t _s_par_cf_telegram			= nxs_string("cf_telegram");
static nxs_string_t _s_par_cf_watchers			= nxs_string("cf_watchers");
static nxs_string_t _s_par_cf_ignore_status		= nxs_string("cf_ignore_status");
static nxs_string_t _s_par_status_in_progress		= nxs_string("status_in_progress");
static nxs_string_t _s_par_status_resolved		= nxs_string("status_resolved");
static nxs_string_t _s_par_status_need_feedback		= nxs_string("status_need_feedback");
static nxs_string_t _s_par_attachments			= nxs_string("attachments");
static nxs_string_t _s_par_tlgrm_download_tmp_dir	= nxs_string("tlgrm_download_tmp_dir");
static nxs_string_t _s_par_rdmn_download_tmp_dir	= nxs_string("rdmn_download_tmp_dir");
static nxs_string_t _s_par_cluster			= nxs_string("cluster");
static nxs_string_t _s_par_nodes			= nxs_string("nodes");
static nxs_string_t _s_par_keys_space			= nxs_string("keys_space");
static nxs_string_t _s_par_statistic			= nxs_string("statistic");
static nxs_string_t _s_par_presale_api_key		= nxs_string("presale_api_key");
static nxs_string_t _s_par_presale_project_name		= nxs_string("presale_project_name");
static nxs_string_t _s_par_webhook_host			= nxs_string("webhook_host");
static nxs_string_t _s_par_use_ssl			= nxs_string("use_ssl");

/* Module global functions */

// clang-format on

void nxs_chat_srv_conf_file_json_init(nxs_chat_srv_cfg_ctx_t *cfg_ctx)
{
}

nxs_chat_srv_err_t nxs_chat_srv_conf_file_json_runtime(nxs_chat_srv_cfg_ctx_t *cfg_ctx)
{
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;
	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_proc,		&nxs_chat_srv_cfg.proc,		&nxs_chat_srv_conf_file_json_read_proc,		NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_logging,	&nxs_chat_srv_cfg.log,		&nxs_chat_srv_conf_file_json_read_log,		NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_bind,		&nxs_chat_srv_cfg.bind,		&nxs_chat_srv_conf_file_json_read_bind,		NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_telegram,	&nxs_chat_srv_cfg.tlgrm,	&nxs_chat_srv_conf_file_json_read_tlgrm,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_mysql,		&nxs_chat_srv_cfg.mysql,	&nxs_chat_srv_conf_file_json_read_mysql,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_redis,		&nxs_chat_srv_cfg.redis,	&nxs_chat_srv_conf_file_json_read_redis,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_redmine,	&nxs_chat_srv_cfg.rdmn,		&nxs_chat_srv_conf_file_json_read_rdmn,		NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_ra_queue,	&nxs_chat_srv_cfg.ra_queue,	&nxs_chat_srv_conf_file_json_read_ra_queue,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_cache,		&nxs_chat_srv_cfg.cache,	&nxs_chat_srv_conf_file_json_read_cache,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_dev_accounts,	&nxs_chat_srv_cfg.dev_accounts,	NULL,						NULL,	NXS_CFG_JSON_TYPE_ARRAY_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_attachments,	&nxs_chat_srv_cfg.attachments,	&nxs_chat_srv_conf_file_json_read_attachments,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_statistic,	&nxs_chat_srv_cfg.statistic,	&nxs_chat_srv_conf_file_json_read_statistic,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(
	        &process, &cfg_json, &cfg_ctx->cfg_path, &nxs_chat_srv_conf_file_json_prep, &nxs_chat_srv_conf_file_json_post, &cfg_arr);

	if(nxs_cfg_json_read_file(&process, cfg_json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "json config read error");

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

/* Module internal (static) functions */

/* Project config file prepare function */
static nxs_cfg_json_state_t nxs_chat_srv_conf_file_json_prep(nxs_cfg_json_t cfg)
{

	/* init config values */

	return NXS_CFG_JSON_CONF_OK;
}

/* Project config file post function */

static nxs_cfg_json_state_t nxs_chat_srv_conf_file_json_post(nxs_cfg_json_t cfg)
{

	/* check all defined config values and prepare program runtime (on error - NXS_CFG_CONF_ERROR value must be returned)*/

	if(nxs_array_count(&nxs_chat_srv_cfg.redis.nodes) == 0) {

		nxs_log_write_raw(&process, "config read error: empty array of Redis nodes");

		return NXS_CFG_JSON_CONF_ERROR;
	}

	nxs_chat_srv_cfg.ra_queue.pop_timeout_ms *= 1000;

	return NXS_CFG_JSON_CONF_OK;
}

/* Project config file auxiliaries functions */

static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_proc(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_cfg_proc_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t           cfg_json;
	nxs_array_t              cfg_arr;
	nxs_cfg_json_state_t     rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_pid_file,			&var->pid_file,				NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_queue_worker_term_timeout,	&var->queue_worker_term_timeout,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		1,	3600,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_rest_api_term_timeout,		&var->rest_api_term_timeout,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		1,	3600,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_daemonize,			&var->daemonize,			NULL,	NULL,	NXS_CFG_JSON_TYPE_BOOL,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "config read error: 'proc' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

	var->queue_worker_term_timeout *= 1000;
	var->rest_api_term_timeout *= 1000;

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_conf_file_json_read_log(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_cfg_log_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t          cfg_json;
	nxs_array_t             cfg_arr;
	nxs_cfg_json_state_t    rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_level,	&var->level,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT_32,	0,	63,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_path,	&var->path,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "config read error: 'logging' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_bind(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_cfg_bind_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t           cfg_json;
	nxs_array_t              cfg_arr;
	nxs_cfg_json_state_t     rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_port,	&var->port,	NULL,						NULL,	NXS_CFG_JSON_TYPE_INT_16,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_iface,	&var->iface,	NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_ssl,	&var->ssl,	&nxs_chat_srv_conf_file_json_read_bind_ssl,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "config read error: 'bind' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_bind_ssl(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_cfg_bind_ssl_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t               cfg_json;
	nxs_array_t                  cfg_arr;
	nxs_cfg_json_state_t         rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_use_ssl,	&var->use_ssl,	NULL,	NULL,	NXS_CFG_JSON_TYPE_BOOL,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_crt,		&var->crt,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_key,		&var->key,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "config read error: 'bind.ssl' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_tlgrm(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_cfg_tlgrm_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t            cfg_json;
	nxs_array_t               cfg_arr;
	nxs_cfg_json_state_t      rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_bot_api_addr,	&var->bot_api_addr,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_bot_api_key,	&var->bot_api_key,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_auth_token,	&var->auth_token,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_webhook_host,	&var->webhook_host,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "config read error: 'telegram' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_mysql(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_cfg_mysql_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t            cfg_json;
	nxs_array_t               cfg_arr;
	nxs_cfg_json_state_t      rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_name,	&var->name,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_user,	&var->user,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_pass,	&var->pass,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_host,	&var->host,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_port,	&var->port,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT_16,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "config read error: 'mysql' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_redis(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_cfg_redis_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t            cfg_json;
	nxs_array_t               cfg_arr;
	nxs_cfg_json_state_t      rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_keys_space,	&var->keys_space,	NULL,	NULL,						NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_cluster,	&var->is_cluster,	NULL,	NULL,						NXS_CFG_JSON_TYPE_BOOL,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_nodes,		&var->nodes,		NULL,	&nxs_chat_srv_conf_file_json_read_redis_nodes,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "config read error: 'redis' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_conf_file_json_read_redis_nodes(nxs_process_t *     proc,
                                                                         nxs_json_t *        json,
                                                                         nxs_cfg_json_par_t *cfg_json_par_el,
                                                                         nxs_array_t *       cfg_arr)
{
	nxs_array_t *                  nodes = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_cfg_redis_node_t *n;

	n = nxs_array_add(nodes);

	n->port = 0;

	nxs_string_init(&n->host);

	nxs_cfg_json_conf_array_skip_undef(cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_port,	&n->port,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT_16,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_host,	&n->host,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_rdmn(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_cfg_rdmn_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t           cfg_json;
	nxs_array_t              cfg_arr;
	nxs_cfg_json_state_t     rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_host,			&var->host,			NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_api_key,		&var->api_key,			NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_auth_token,		&var->auth_token,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_issue_list_query_id,	&var->issue_list_query_id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_ssl_verifyhost,		&var->ssl_verifyhost,		NULL,	NULL,	NXS_CFG_JSON_TYPE_BOOL,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_cf_telegram,		&var->cf_telegram,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_cf_watchers,		&var->cf_watchers,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_cf_ignore_status,	&var->cf_ignore_status,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_status_in_progress,	&var->status_in_progress,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_status_resolved,	&var->status_resolved,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_status_need_feedback,	&var->status_need_feedback,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_presale_api_key,	&var->presale_api_key,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_presale_project_name,	&var->presale_project_name,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "config read error: 'redmine' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_ra_queue(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_cfg_ra_queue_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t               cfg_json;
	nxs_array_t                  cfg_arr;
	nxs_cfg_json_state_t         rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_pop_timeout_ms,	&var->pop_timeout_ms,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_queue_workers,	&var->queue_workers,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "config read error: 'ra_queue' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_cache(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_cfg_cache_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t            cfg_json;
	nxs_array_t               cfg_arr;
	nxs_cfg_json_state_t      rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_projects_ttl,		&var->projects_ttl,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_users_ttl,		&var->users_ttl,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_issue_priorities_ttl,	&var->issue_priorities_ttl,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_lock_file,		&var->lock_file,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "config read error: 'cache' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_attachments(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_cfg_attachments_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                  cfg_json;
	nxs_array_t                     cfg_arr;
	nxs_cfg_json_state_t            rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_tlgrm_download_tmp_dir,	&var->tlgrm_download_tmp_dir,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_rdmn_download_tmp_dir,	&var->rdmn_download_tmp_dir,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "config read error: 'attachments' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_conf_file_json_read_statistic(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_cfg_statistic_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                cfg_json;
	nxs_array_t                   cfg_arr;
	nxs_cfg_json_state_t          rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_auth_token,	&var->auth_token,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "config read error: 'statistic' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

#endif /* USE_NXS_JSON */
