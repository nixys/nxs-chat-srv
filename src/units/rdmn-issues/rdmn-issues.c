#ifndef _INCLUDE_NXS_CHAT_SRV_U_RDMN_ISSUES_C
#define _INCLUDE_NXS_CHAT_SRV_U_RDMN_ISSUES_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/rdmn-issues/rdmn-issues-typedefs.h>
#include <units/rdmn-issues/rdmn-issues.h>
#include <units/rdmn-issues/rdmn-issues-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_issues_create_extract(size_t *new_issue_id, nxs_buf_t *json_buf);
static nxs_cfg_json_state_t
        nxs_chat_srv_u_rdmn_issues_create_extract_issue(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

// clang-format off

/* Module initializations */

static nxs_string_t _s_par_issue		= nxs_string("issue");
static nxs_string_t _s_par_id			= nxs_string("id");

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_issues_create(size_t        project_id,
                                                     size_t        priority_id,
                                                     nxs_string_t *subject,
                                                     nxs_string_t *description,
                                                     size_t *      new_issue_id,
                                                     nxs_string_t *api_key)
{
	nxs_buf_t          out_buf;
	nxs_chat_srv_err_t rc;
	nxs_http_code_t    http_code;

	if(subject == NULL || description == NULL || new_issue_id == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_buf_init2(&out_buf);

	if(nxs_chat_srv_d_rdmn_issues_create(project_id, priority_id, subject, description, api_key, &out_buf, &http_code) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_rdmn_issues_create_extract(new_issue_id, &out_buf) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_buf_free(&out_buf);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_issues_add_note(size_t issue_id, nxs_string_t *note, nxs_string_t *api_key)
{
	nxs_chat_srv_err_t rc;
	nxs_http_code_t    http_code;

	rc = nxs_chat_srv_d_rdmn_issues_add_comment(issue_id, note, api_key, NULL, &http_code);

	return rc;
}

/* Module internal (static) functions */

static nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_issues_create_extract(size_t *new_issue_id, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_issue,		new_issue_id,	&nxs_chat_srv_u_rdmn_issues_create_extract_issue,	NULL,	NXS_CFG_JSON_TYPE_VOID,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(
		        &process, "[%s]: rdmn new issue extract error: parse rdmn new issue error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_u_rdmn_issues_create_extract_issue(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	size_t *             var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t       cfg_json;
	nxs_array_t          cfg_arr;
	nxs_cfg_json_state_t rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,	var,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn new issue extract error: 'issue' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}
