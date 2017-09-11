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

	nxs_string_init(&nxs_chat_srv_cfg.proc.pid_file);
	nxs_string_init(&nxs_chat_srv_cfg.log.path);
	nxs_string_init(&nxs_chat_srv_cfg.bind.iface);
	nxs_string_init(&nxs_chat_srv_cfg.bind.ssl.crt);
	nxs_string_init(&nxs_chat_srv_cfg.bind.ssl.key);
	nxs_string_init(&nxs_chat_srv_cfg.tlgrm.bot_api_addr);
	nxs_string_init(&nxs_chat_srv_cfg.tlgrm.bot_api_key);
	nxs_string_init(&nxs_chat_srv_cfg.tlgrm.auth_token);
	nxs_string_init(&nxs_chat_srv_cfg.mysql.name);
	nxs_string_init(&nxs_chat_srv_cfg.mysql.user);
	nxs_string_init(&nxs_chat_srv_cfg.mysql.pass);
	nxs_string_init(&nxs_chat_srv_cfg.mysql.host);
	nxs_string_init(&nxs_chat_srv_cfg.redis.host);
	nxs_string_init(&nxs_chat_srv_cfg.rdmn.host);
	nxs_string_init(&nxs_chat_srv_cfg.rdmn.api_key);
	nxs_string_init(&nxs_chat_srv_cfg.rdmn.auth_token);
	nxs_string_init(&nxs_chat_srv_cfg.queue_worker.sock_path);
	nxs_string_init(&nxs_chat_srv_cfg.queue_worker.sock_user);
	nxs_string_init(&nxs_chat_srv_cfg.queue_worker.sock_group);

	nxs_chat_srv_cfg.proc.queue_worker_term_timeout = 10;
	nxs_chat_srv_cfg.proc.rest_api_term_timeout     = 10;
	nxs_chat_srv_cfg.proc.daemonize                 = NXS_YES;
	nxs_chat_srv_cfg.log.level                      = NXS_LOG_LEVEL_MEM | NXS_LOG_LEVEL_INFO | NXS_LOG_LEVEL_WARN | NXS_LOG_LEVEL_ERROR;
	nxs_chat_srv_cfg.bind.port                      = 0;
	nxs_chat_srv_cfg.mysql.port                     = 0;
	nxs_chat_srv_cfg.redis.port                     = 0;
	nxs_chat_srv_cfg.queue_worker.sock_mode         = 0;
	nxs_chat_srv_cfg.queue_worker.sock_max_conn     = 0;

	/* Init cfg context values */

	nxs_string_init2(&cfg_ctx->cfg_path, 0, (u_char *)NXS_CHAT_SRV_CONF_PATH_DEFAULT);
}
