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
extern		nxs_process_t				process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static nxs_cfg_json_state_t				nxs_chat_srv_conf_file_json_prep									(nxs_cfg_json_t cfg);
static nxs_cfg_json_state_t				nxs_chat_srv_conf_file_json_post									(nxs_cfg_json_t cfg);

// clang-format off

/* Module initializations */

static nxs_string_t _s_par_loglevel			= nxs_string("loglevel");
static nxs_string_t _s_par_logpath			= nxs_string("logpath");

/* Module global functions */

// clang-format on

void nxs_chat_srv_conf_file_json_init(nxs_chat_srv_cfg_ctx_t *cfg_ctx)
{

}

nxs_chat_srv_err_t nxs_chat_srv_conf_file_json_runtime(nxs_chat_srv_cfg_ctx_t *cfg_ctx)
{
	nxs_cfg_json_t		cfg_json;
	nxs_array_t			cfg_arr;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_loglevel,		&nxs_chat_srv_cfg.log_level,		NULL, NULL,		NXS_CFG_JSON_TYPE_INT_32,	0,	63,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_logpath,		&nxs_chat_srv_cfg.log_path,		NULL, NULL,		NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	nxs_cfg_json_init(&process, &cfg_json, &cfg_ctx->cfg_path, &nxs_chat_srv_conf_file_json_prep, &nxs_chat_srv_conf_file_json_post, &cfg_arr);

	if(nxs_cfg_json_read_file(&process, cfg_json) != NXS_CFG_JSON_CONF_OK){

		nxs_log_write_raw(&process, "json config read error");

		return NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return NXS_CHAT_SRV_E_OK;
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

	return NXS_CFG_JSON_CONF_OK;
}

/* Project config file auxiliaries functions */

#endif /* USE_NXS_JSON */
