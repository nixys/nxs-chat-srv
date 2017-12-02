#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-conf.h>
#include <nxs-chat-srv-dal.h>

/* Definitions */

/*
 * Possible values:
 * 0 - no config
 * 1 - ini-config
 * 2 - json-config
 */
#define NXS_NXS_CHAT_SRV_CFG_TYPE		2

/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static void nxs_nxs_chat_srv_conf_cfg_ctx_init(nxs_chat_srv_cfg_ctx_t *cfg_ctx);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_conf_runtime(int argc, char *argv[])
{
	nxs_chat_srv_cfg_ctx_t cfg_ctx;
	nxs_chat_srv_err_t     rc;
	nxs_args_t             args;

#if NXS_NXS_CHAT_SRV_CFG_TYPE & 1
	nxs_cfg_t cfg;
#endif

	nxs_nxs_chat_srv_conf_cfg_ctx_init(&cfg_ctx);

	/* Command line args */

	nxs_chat_srv_conf_args_init(&args, &cfg_ctx);

	if((rc = nxs_chat_srv_conf_args_runtime(args, argc, (u_char **)argv)) != NXS_CHAT_SRV_E_OK) {

		return rc;
	}

/* Config in ini-format */

#if NXS_NXS_CHAT_SRV_CFG_TYPE & 1
	nxs_chat_srv_conf_file_ini_init(&cfg, &cfg_ctx);

	if(nxs_chat_srv_conf_file_ini_runtime(cfg, &cfg_ctx) != NXS_CHAT_SRV_E_OK) {

		return NXS_CHAT_SRV_E_ERR;
	}
#endif

/* Config in json-format */

#if NXS_NXS_CHAT_SRV_CFG_TYPE & 2
	nxs_chat_srv_conf_file_json_init(&cfg_ctx);

	if(nxs_chat_srv_conf_file_json_runtime(&cfg_ctx) != NXS_CHAT_SRV_E_OK) {

		return NXS_CHAT_SRV_E_ERR;
	}
#endif

	return NXS_CHAT_SRV_E_OK;
}

/* Module internal (static) functions */

static void nxs_nxs_chat_srv_conf_cfg_ctx_init(nxs_chat_srv_cfg_ctx_t *cfg_ctx)
{

	/* Init config values */

	nxs_string_init_empty(&nxs_chat_srv_cfg.proc.pid_file);
	nxs_string_init_empty(&nxs_chat_srv_cfg.log.path);
	nxs_string_init_empty(&nxs_chat_srv_cfg.bind.iface);
	nxs_string_init_empty(&nxs_chat_srv_cfg.bind.ssl.crt);
	nxs_string_init_empty(&nxs_chat_srv_cfg.bind.ssl.key);
	nxs_string_init_empty(&nxs_chat_srv_cfg.tlgrm.bot_api_addr);
	nxs_string_init_empty(&nxs_chat_srv_cfg.tlgrm.bot_api_key);
	nxs_string_init_empty(&nxs_chat_srv_cfg.tlgrm.auth_token);
	nxs_string_init_empty(&nxs_chat_srv_cfg.tlgrm.webhook_host);
	nxs_string_init_empty(&nxs_chat_srv_cfg.mysql.name);
	nxs_string_init_empty(&nxs_chat_srv_cfg.mysql.user);
	nxs_string_init_empty(&nxs_chat_srv_cfg.mysql.pass);
	nxs_string_init_empty(&nxs_chat_srv_cfg.mysql.host);
	nxs_string_init_empty(&nxs_chat_srv_cfg.redis.keys_space);
	nxs_string_init_empty(&nxs_chat_srv_cfg.rdmn.host);
	nxs_string_init_empty(&nxs_chat_srv_cfg.rdmn.api_key);
	nxs_string_init_empty(&nxs_chat_srv_cfg.rdmn.auth_token);
	nxs_string_init_empty(&nxs_chat_srv_cfg.rdmn.presale_api_key);
	nxs_string_init_empty(&nxs_chat_srv_cfg.rdmn.presale_project_name);
	nxs_string_init_empty(&nxs_chat_srv_cfg.cache.lock_file);
	nxs_string_init_empty(&nxs_chat_srv_cfg.attachments.tlgrm_download_tmp_dir);
	nxs_string_init_empty(&nxs_chat_srv_cfg.attachments.rdmn_download_tmp_dir);
	nxs_string_init_empty(&nxs_chat_srv_cfg.statistic.auth_token);

	nxs_array_init2(&nxs_chat_srv_cfg.dev_accounts, nxs_string_t);
	nxs_array_init2(&nxs_chat_srv_cfg.redis.nodes, nxs_chat_srv_cfg_redis_node_t);

	nxs_chat_srv_cfg.init_mode                      = NXS_CHAT_SRV_INIT_MODE_NONE;
	nxs_chat_srv_cfg.proc.queue_worker_term_timeout = 10;
	nxs_chat_srv_cfg.proc.rest_api_term_timeout     = 10;
	nxs_chat_srv_cfg.proc.daemonize                 = NXS_YES;
	nxs_chat_srv_cfg.log.level                      = NXS_LOG_LEVEL_MEM | NXS_LOG_LEVEL_INFO | NXS_LOG_LEVEL_WARN | NXS_LOG_LEVEL_ERROR;
	nxs_chat_srv_cfg.bind.port                      = 0;
	nxs_chat_srv_cfg.mysql.port                     = 0;
	nxs_chat_srv_cfg.redis.is_cluster               = NXS_NO;
	nxs_chat_srv_cfg.ra_queue.pop_timeout_ms        = 0;
	nxs_chat_srv_cfg.ra_queue.queue_workers         = 0;
	nxs_chat_srv_cfg.cache.rdmn_cf_telegram         = 0;
	nxs_chat_srv_cfg.cache.projects_ttl             = 0;
	nxs_chat_srv_cfg.cache.users_ttl                = 0;
	nxs_chat_srv_cfg.cache.issue_priorities_ttl     = 0;
	nxs_chat_srv_cfg.rdmn.ssl_verifyhost            = NXS_YES;
	nxs_chat_srv_cfg.rdmn.issue_list_query_id       = 0;
	nxs_chat_srv_cfg.rdmn.cf_watchers               = 0;
	nxs_chat_srv_cfg.rdmn.cf_ignore_status          = 0;
	nxs_chat_srv_cfg.rdmn.status_in_progress        = 0;
	nxs_chat_srv_cfg.rdmn.status_resolved           = 0;
	nxs_chat_srv_cfg.rdmn.status_need_feedback      = 0;

	/* Init cfg context values */

	nxs_string_init2(&cfg_ctx->cfg_path, 0, (u_char *)NXS_CHAT_SRV_CONF_PATH_DEFAULT);
}
