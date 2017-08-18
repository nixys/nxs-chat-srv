#ifndef _INCLUDE_NXS_CHAT_SRV_U_TLGRM_UPDATE_C
#define _INCLUDE_NXS_CHAT_SRV_U_TLGRM_UPDATE_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/tlgrm-update/tlgrm-update-typedefs.h>
#include <units/tlgrm-update/tlgrm-update.h>
#include <units/tlgrm-update/tlgrm-update-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_u_tlgrm_update_s
{
	nxs_chat_srv_m_tlgrm_update_t		update;
};

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_update_pull_json_extract(nxs_chat_srv_m_tlgrm_update_t *update, nxs_buf_t *json_buf);
static nxs_cfg_json_state_t
        nxs_chat_srv_u_tlgrm_update_pull_json_extract_message(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_u_tlgrm_update_pull_json_extract_chat(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

// clang-format off

/* Module initializations */

static nxs_string_t	_s_par_update_id	= nxs_string("update_id");
static nxs_string_t	_s_par_message		= nxs_string("message");
static nxs_string_t	_s_par_message_id	= nxs_string("message_id");
static nxs_string_t	_s_par_text		= nxs_string("text");
static nxs_string_t	_s_par_chat		= nxs_string("chat");
static nxs_string_t	_s_par_id		= nxs_string("id");
static nxs_string_t	_s_par_username		= nxs_string("username");

/* Module global functions */

// clang-format on

nxs_chat_srv_u_tlgrm_update_t *nxs_chat_srv_u_tlgrm_update_init(void)
{
	nxs_chat_srv_u_tlgrm_update_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_tlgrm_update_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_tlgrm_update_t));

	nxs_chat_srv_c_tlgrm_update_init(&u_ctx->update);

	return u_ctx;
}

nxs_chat_srv_u_tlgrm_update_t *nxs_chat_srv_u_tlgrm_update_free(nxs_chat_srv_u_tlgrm_update_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	nxs_chat_srv_c_tlgrm_update_free(&u_ctx->update);

	return (nxs_chat_srv_u_tlgrm_update_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_update_pull_json(nxs_chat_srv_u_tlgrm_update_t *u_ctx, nxs_buf_t *json_buf)
{

	if(u_ctx == NULL || json_buf == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	return nxs_chat_srv_u_tlgrm_update_pull_json_extract(&u_ctx->update, json_buf);
}

nxs_chat_srv_m_tlgrm_update_t *nxs_chat_srv_u_tlgrm_update_get(nxs_chat_srv_u_tlgrm_update_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	return &u_ctx->update;
}

/* Module internal (static) functions */

static nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_update_pull_json_extract(nxs_chat_srv_m_tlgrm_update_t *update, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	update->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_update_id,	&update->update_id,	NULL,							NULL,	NXS_CFG_JSON_TYPE_INT,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_message,	&update->message,	&nxs_chat_srv_u_tlgrm_update_pull_json_extract_message,	NULL,	NXS_CFG_JSON_TYPE_VOID,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(
		        &process, "[%s]: tlgrm update ctx pull rest api error: parse json_buf error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_u_tlgrm_update_pull_json_extract_message(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_tlgrm_message_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                  cfg_json;
	nxs_array_t                     cfg_arr;
	nxs_cfg_json_state_t            rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_message_id,	&var->message_id,	NULL,							NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_text,		&var->text,		NULL,							NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_chat,		&var->chat,		&nxs_chat_srv_u_tlgrm_update_pull_json_extract_chat,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "json read error: 'message' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_u_tlgrm_update_pull_json_extract_chat(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_tlgrm_chat_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t               cfg_json;
	nxs_array_t                  cfg_arr;
	nxs_cfg_json_state_t         rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&var->id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_username,	&var->username,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "json read error: 'message.chat' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}
