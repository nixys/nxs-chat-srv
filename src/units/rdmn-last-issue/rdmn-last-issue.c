#ifndef _INCLUDE_NXS_CHAT_SRV_U_RDMN_LAST_ISSUE_C
#define _INCLUDE_NXS_CHAT_SRV_U_RDMN_LAST_ISSUE_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/rdmn-last-issue/rdmn-last-issue-typedefs.h>
#include <units/rdmn-last-issue/rdmn-last-issue.h>
#include <units/rdmn-last-issue/rdmn-last-issue-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t				process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_u_rdmn_last_issue_s
{
	size_t			user_id;
	size_t			issue_id;
	nxs_string_t		issue_subject;
};

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_last_issue_extract(nxs_chat_srv_u_rdmn_last_issue_t *u_ctx, nxs_buf_t *json_buf);
static nxs_cfg_json_state_t
        nxs_chat_srv_u_rdmn_last_issue_extract_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_u_rdmn_last_issue_extract_issue(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

// clang-format off

/* Module initializations */

static nxs_string_t _s_par_user			= nxs_string("user");
static nxs_string_t _s_par_id			= nxs_string("id");
static nxs_string_t _s_par_issue		= nxs_string("issue");
static nxs_string_t _s_par_subject		= nxs_string("subject");

/* Module global functions */

// clang-format on

nxs_chat_srv_u_rdmn_last_issue_t *nxs_chat_srv_u_rdmn_last_issue_init(void)
{
	nxs_chat_srv_u_rdmn_last_issue_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_rdmn_last_issue_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_rdmn_last_issue_t));

	u_ctx->user_id  = 0;
	u_ctx->issue_id = 0;

	nxs_string_init_empty(&u_ctx->issue_subject);

	return u_ctx;
}

nxs_chat_srv_u_rdmn_last_issue_t *nxs_chat_srv_u_rdmn_last_issue_free(nxs_chat_srv_u_rdmn_last_issue_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	u_ctx->user_id  = 0;
	u_ctx->issue_id = 0;

	nxs_string_free(&u_ctx->issue_subject);

	return (nxs_chat_srv_u_rdmn_last_issue_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_last_issue_pull(nxs_chat_srv_u_rdmn_last_issue_t *u_ctx, size_t rdmn_userid)
{
	nxs_chat_srv_err_t rc;
	nxs_buf_t          out_buf;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(rdmn_userid == 0) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_buf_init2(&out_buf);

	/*
	 * get data from Redmine
	 */

	if(nxs_chat_srv_d_rdmn_last_issue_get(rdmn_userid, &nxs_chat_srv_cfg.rdmn.api_key, &out_buf, NULL) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_rdmn_last_issue_extract(u_ctx, &out_buf)) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_buf_free(&out_buf);

	return rc;
}

nxs_chat_srv_err_t
        nxs_chat_srv_u_rdmn_last_issue_get(nxs_chat_srv_u_rdmn_last_issue_t *u_ctx, size_t *issue_id, nxs_string_t *issue_subject)
{

	if(u_ctx == NULL || issue_id == NULL || issue_subject == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	*issue_id = u_ctx->issue_id;

	nxs_string_clone(issue_subject, &u_ctx->issue_subject);

	return NXS_CHAT_SRV_E_OK;
}

/* Module internal (static) functions */

static nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_last_issue_extract(nxs_chat_srv_u_rdmn_last_issue_t *u_ctx, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_user,		u_ctx,	&nxs_chat_srv_u_rdmn_last_issue_extract_user,	NULL,	NXS_CFG_JSON_TYPE_VOID,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(
		        &process, "[%s]: rdmn last issue extract error: parse rdmn last issue error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_u_rdmn_last_issue_extract_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_u_rdmn_last_issue_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                    cfg_json;
	nxs_array_t                       cfg_arr;
	nxs_cfg_json_state_t              rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&var->user_id,		NULL,						NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_issue,		var,			&nxs_chat_srv_u_rdmn_last_issue_extract_issue,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_NO,		NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn last issue extract error: 'user' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_u_rdmn_last_issue_extract_issue(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_u_rdmn_last_issue_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                    cfg_json;
	nxs_array_t                       cfg_arr;
	nxs_cfg_json_state_t              rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&var->issue_id,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_subject,	&var->issue_subject,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn last issue extract error: 'user.issue' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}
