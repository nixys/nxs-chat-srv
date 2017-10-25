#ifndef _INCLUDE_NXS_CHAT_SRV_U_LAST_ISSUES_C
#define _INCLUDE_NXS_CHAT_SRV_U_LAST_ISSUES_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/last-issues/last-issues-typedefs.h>
#include <units/last-issues/last-issues.h>
#include <units/last-issues/last-issues-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	size_t			*id;
	nxs_string_t		*subject;
	nxs_string_t		*project_name;
} last_issue_t;

struct nxs_chat_srv_u_last_issues_s
{
	nxs_chat_srv_d_db_last_issues_t		*db_last_issues_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_err_t
        nxs_chat_srv_u_last_issues_get_db(nxs_chat_srv_u_last_issues_t *u_ctx, size_t tlgrm_userid, size_t *rdmn_last_issue_id);
static nxs_chat_srv_err_t nxs_chat_srv_u_last_issues_id_extract(last_issue_t *last_issue_ctx, nxs_buf_t *json_buf);
static nxs_chat_srv_err_t nxs_chat_srv_u_last_issues_last_extract(last_issue_t *last_issue_ctx, nxs_buf_t *json_buf);
static nxs_cfg_json_state_t
        nxs_chat_srv_u_last_issues_last_extract_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_u_last_issues_extract_issue(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_u_last_issues_extract_poject(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

// clang-format off

/* Module initializations */

static nxs_string_t _s_par_user			= nxs_string("user");
static nxs_string_t _s_par_id			= nxs_string("id");
static nxs_string_t _s_par_issue		= nxs_string("issue");
static nxs_string_t _s_par_subject		= nxs_string("subject");
static nxs_string_t _s_par_project		= nxs_string("project");
static nxs_string_t _s_par_name			= nxs_string("name");

/* Module global functions */

// clang-format on

nxs_chat_srv_u_last_issues_t *nxs_chat_srv_u_last_issues_init(void)
{
	nxs_chat_srv_u_last_issues_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_last_issues_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_last_issues_t));

	u_ctx->db_last_issues_ctx = nxs_chat_srv_d_db_last_issues_init();

	return u_ctx;
}

nxs_chat_srv_u_last_issues_t *nxs_chat_srv_u_last_issues_free(nxs_chat_srv_u_last_issues_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	u_ctx->db_last_issues_ctx = nxs_chat_srv_d_db_last_issues_free(u_ctx->db_last_issues_ctx);

	return (nxs_chat_srv_u_last_issues_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_last_issues_get(nxs_chat_srv_u_last_issues_t *u_ctx,
                                                  size_t                        tlgrm_userid,
                                                  size_t                        rdmn_userid,
                                                  nxs_string_t *                user_api_key,
                                                  size_t *                      rdmn_last_issue_id,
                                                  nxs_string_t *                issue_subject,
                                                  nxs_string_t *                issue_project_name)
{
	nxs_buf_t          out_buf;
	last_issue_t       last_issue_ctx;
	nxs_chat_srv_err_t rc;

	if(u_ctx == NULL || rdmn_last_issue_id == NULL || issue_subject == NULL || user_api_key == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	*rdmn_last_issue_id = 0;

	last_issue_ctx.id           = rdmn_last_issue_id;
	last_issue_ctx.subject      = issue_subject;
	last_issue_ctx.project_name = issue_project_name;

	nxs_string_clear(issue_subject);

	nxs_buf_init2(&out_buf);

	switch(nxs_chat_srv_u_last_issues_get_db(u_ctx, tlgrm_userid, rdmn_last_issue_id)) {

		case NXS_CHAT_SRV_E_OK:

			/* Last Redmine issue id found in DB. Now try to get subject for that issue from Redmine */

			if(nxs_chat_srv_d_rdmn_issues_get_issue(*rdmn_last_issue_id, user_api_key, NULL, &out_buf) == NXS_CHAT_SRV_E_OK) {

				if(nxs_chat_srv_u_last_issues_id_extract(&last_issue_ctx, &out_buf) != NXS_CHAT_SRV_E_OK) {

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}
			}
			else {

				/* Can't get requested issue id from Remine. Trying to determine last issue from Redmine instead DB (Redis)
				 */

				if(nxs_chat_srv_d_rdmn_last_issue_get(rdmn_userid, user_api_key, NULL, &out_buf) != NXS_CHAT_SRV_E_OK) {

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(nxs_chat_srv_u_last_issues_last_extract(&last_issue_ctx, &out_buf)) {

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}
			}

			break;

		case NXS_CHAT_SRV_E_EXIST:

			/* Last Redmine issue id was not found in DB. Try to get data from Redmine */

			if(nxs_chat_srv_d_rdmn_last_issue_get(rdmn_userid, user_api_key, NULL, &out_buf) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_u_last_issues_last_extract(&last_issue_ctx, &out_buf)) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		default:

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_buf_free(&out_buf);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_last_issues_set(nxs_chat_srv_u_last_issues_t *u_ctx, size_t tlgrm_userid, size_t rdmn_last_issue_id)
{
	nxs_chat_srv_err_t rc;

	if((rc = nxs_chat_srv_d_db_last_issues_put(u_ctx->db_last_issues_ctx, tlgrm_userid, rdmn_last_issue_id)) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: can't save user last issue into DB (tlgrm userid: %zu, rdmn last issue id: %zu)",
		                    nxs_proc_get_name(&process),
		                    tlgrm_userid,
		                    rdmn_last_issue_id);
	}

	return rc;
}

/* Module internal (static) functions */

static nxs_chat_srv_err_t
        nxs_chat_srv_u_last_issues_get_db(nxs_chat_srv_u_last_issues_t *u_ctx, size_t tlgrm_userid, size_t *rdmn_last_issue_id)
{
	nxs_chat_srv_err_t rc;

	if(rdmn_last_issue_id == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	switch((rc = nxs_chat_srv_d_db_last_issues_get(u_ctx->db_last_issues_ctx, tlgrm_userid, rdmn_last_issue_id))) {

		case NXS_CHAT_SRV_E_OK:

			break;

		case NXS_CHAT_SRV_E_EXIST:

			nxs_log_write_debug(&process,
			                    "[%s]: can't get user last issue from DB: record does not exist (tlgrm userid: %zu)",
			                    nxs_proc_get_name(&process),
			                    tlgrm_userid);

			*rdmn_last_issue_id = 0;

			rc = NXS_CHAT_SRV_E_EXIST;

			break;

		default:

			nxs_log_write_error(&process,
			                    "[%s]: can't get user last issue from DB (tlgrm userid: %zu)",
			                    nxs_proc_get_name(&process),
			                    tlgrm_userid);

			*rdmn_last_issue_id = 0;

			break;
	}

	return rc;
}

static nxs_chat_srv_err_t nxs_chat_srv_u_last_issues_id_extract(last_issue_t *last_issue_ctx, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_issue,	last_issue_ctx,	&nxs_chat_srv_u_last_issues_extract_issue,	NULL,	NXS_CFG_JSON_TYPE_VOID,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(
		        &process, "[%s]: rdmn last issue (by id) extract error: parse rdmn last issue error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_chat_srv_err_t nxs_chat_srv_u_last_issues_last_extract(last_issue_t *last_issue_ctx, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_user,	last_issue_ctx,	&nxs_chat_srv_u_last_issues_last_extract_user,	NULL,	NXS_CFG_JSON_TYPE_VOID,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(
		        &process, "[%s]: rdmn last issue (last) extract error: parse rdmn last issue error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_u_last_issues_last_extract_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	last_issue_t *       var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t       cfg_json;
	nxs_array_t          cfg_arr;
	nxs_cfg_json_state_t rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_issue,		var,	&nxs_chat_srv_u_last_issues_extract_issue,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_NO,		NULL);

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
        nxs_chat_srv_u_last_issues_extract_issue(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	last_issue_t *       var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t       cfg_json;
	nxs_array_t          cfg_arr;
	nxs_cfg_json_state_t rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		var->id,		NULL,						NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_subject,	var->subject,		NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_project,	var->project_name,	&nxs_chat_srv_u_last_issues_extract_poject,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn last issue extract error: 'issue' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_u_last_issues_extract_poject(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_string_t *       var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t       cfg_json;
	nxs_array_t          cfg_arr;
	nxs_cfg_json_state_t rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_name,		var,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn last issue extract error: 'project' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}
