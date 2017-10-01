#ifndef _INCLUDE_NXS_CHAT_SRV_D_RDMN_ISSUES_C
#define _INCLUDE_NXS_CHAT_SRV_D_RDMN_ISSUES_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/rdmn-issues/rdmn-issues-typedefs.h>
#include <dal/rdmn-issues/rdmn-issues.h>
#include <dal/rdmn-issues/rdmn-issues-subdals.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */

static nxs_string_t		_s_content_type		= nxs_string("Content-Type: application/json");

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_d_rdmn_issues_add_comment(size_t           issue_id,
                                                          size_t           assigned_to_id,
                                                          nxs_string_t *   note,
                                                          nxs_bool_t       private_notes,
                                                          size_t           status_id,
                                                          nxs_array_t *    custom_fields,
                                                          nxs_string_t *   user_api_key,
                                                          nxs_http_code_t *http_code,
                                                          nxs_buf_t *      out_buf)
{
	nxs_chat_srv_err_t               rc;
	nxs_curl_t                       curl;
	nxs_string_t                     data, api_key, note_escaped, status, assigned_to, cf_str, cf_str_els;
	size_t                           i;
	nxs_chat_srv_m_rdmn_issues_cf_t *cf;
	nxs_http_code_t                  ret_code;
	nxs_buf_t *                      b;
	int                              ec;

	if(note == NULL || user_api_key == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&data);
	nxs_string_init(&api_key);
	nxs_string_init(&note_escaped);
	nxs_string_init_empty(&status);
	nxs_string_init_empty(&assigned_to);
	nxs_string_init_empty(&cf_str);
	nxs_string_init_empty(&cf_str_els);

	nxs_curl_init(&curl);

	nxs_string_printf(&api_key, "X-Redmine-API-Key: %r", user_api_key);

	nxs_curl_add_header(&curl, &api_key);
	nxs_curl_add_header(&curl, &_s_content_type);

	nxs_curl_set_ssl_verivyhost(&curl, nxs_chat_srv_cfg.rdmn.ssl_verifyhost);

	nxs_string_escape(&note_escaped, note, NXS_STRING_ESCAPE_TYPE_JSON);

	if(status_id > 0) {

		nxs_string_printf(&status, ",\"status_id\":%zu", status_id);
	}

	if(assigned_to_id > 0) {

		nxs_string_printf(&assigned_to, ",\"assigned_to_id\":%zu", assigned_to_id);
	}

	if(custom_fields != NULL && nxs_array_count(custom_fields) > 0) {

		for(i = 0; i < nxs_array_count(custom_fields); i++) {

			cf = nxs_array_get(custom_fields, i);

			if(i > 0) {

				nxs_string_char_add_char(&cf_str_els, (u_char)',');
			}

			nxs_string_printf2_cat(&cf_str_els, "{\"id\":%zu,\"value\":\"%r\"}", cf->id, &cf->value);
		}

		nxs_string_printf(&cf_str, ",\"custom_fields\":[%r]", &cf_str_els);
	}

	nxs_string_printf(&data,
	                  "{\"issue\":{\"notes\":\"%r\",\"private_notes\":%s%r%r%r}}",
	                  &note_escaped,
	                  private_notes == NXS_YES ? "true" : "false",
	                  &status,
	                  &assigned_to,
	                  &cf_str);

	nxs_curl_set_post(&curl, (nxs_buf_t *)&data);

	if((ec = nxs_curl_query(
	            &process, &curl, NXS_REST_API_COMMON_CMD_PUT, (u_char *)"%r/issues/%zu.json", &nxs_chat_srv_cfg.rdmn.host, issue_id)) !=
	   NXS_CURL_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: rdmn add issue comment error: curl error (issue id: %zu, rc: %d)",
		                   nxs_proc_get_name(&process),
		                   issue_id,
		                   ec);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	ret_code = nxs_curl_get_ret_code(&curl);
	b        = nxs_curl_get_out_buf(&curl);

	if(http_code != NULL) {

		*http_code = ret_code;
	}

	if(out_buf != NULL) {

		nxs_buf_clone(out_buf, b);
	}

	switch(ret_code) {

		case NXS_HTTP_CODE_200_OK:

			nxs_log_write_debug(&process,
			                    "[%s]: rdmn issue add note: note successfully added (issue id: %zu)",
			                    nxs_proc_get_name(&process),
			                    issue_id);

			rc = NXS_CHAT_SRV_E_OK;

			break;

		case NXS_HTTP_CODE_422_UNPROCESSABLE_ENTITY:

			nxs_log_write_warn(&process,
			                   "[%s]: rdmn issue add note warn: Redmine unprocessable entity (issue id: %zu, response code: "
			                   "%d, response body: \"%s\")",
			                   nxs_proc_get_name(&process),
			                   issue_id,
			                   ret_code,
			                   nxs_buf_get_subbuf(b, 0));

			rc = NXS_CHAT_SRV_E_ATTR;

			break;

		default:

			nxs_log_write_error(&process,
			                    "[%s]: rdmn issue add note error: wrong Redmine response code (issue id: %zu, response code: "
			                    "%d, response body: \"%s\")",
			                    nxs_proc_get_name(&process),
			                    issue_id,
			                    ret_code,
			                    nxs_buf_get_subbuf(b, 0));

			rc = NXS_CHAT_SRV_E_WARN;

			break;
	}

error:

	nxs_string_free(&data);
	nxs_string_free(&api_key);
	nxs_string_free(&note_escaped);
	nxs_string_free(&status);
	nxs_string_free(&assigned_to);
	nxs_string_free(&cf_str);
	nxs_string_free(&cf_str_els);

	nxs_curl_free(&curl);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_rdmn_issues_create(size_t           project_id,
                                                     size_t           priority_id,
                                                     nxs_string_t *   subject,
                                                     nxs_string_t *   description,
                                                     nxs_bool_t       is_private,
                                                     nxs_string_t *   user_api_key,
                                                     nxs_http_code_t *http_code,
                                                     nxs_buf_t *      out_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_curl_t         curl;
	nxs_string_t       data, api_key, subject_escaped, description_escaped;
	nxs_http_code_t    ret_code;
	nxs_buf_t *        b;
	int                ec;

	if(subject == NULL || description == NULL || user_api_key == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&data);
	nxs_string_init(&api_key);
	nxs_string_init(&subject_escaped);
	nxs_string_init(&description_escaped);

	nxs_curl_init(&curl);

	nxs_string_printf(&api_key, "X-Redmine-API-Key: %r", user_api_key);

	nxs_curl_add_header(&curl, &api_key);
	nxs_curl_add_header(&curl, &_s_content_type);

	nxs_curl_set_ssl_verivyhost(&curl, nxs_chat_srv_cfg.rdmn.ssl_verifyhost);

	nxs_string_escape(&subject_escaped, subject, NXS_STRING_ESCAPE_TYPE_JSON);
	nxs_string_escape(&description_escaped, description, NXS_STRING_ESCAPE_TYPE_JSON);

	nxs_string_printf(&data,
	                  "{"
	                  "\"issue\":{"
	                  "\"project_id\":%zu,"
	                  "\"priority_id\":%zu,"
	                  "\"subject\":\"%r\","
	                  "\"description\":\"%r\","
	                  "\"is_private\": %s"
	                  "}"
	                  "}",
	                  project_id,
	                  priority_id,
	                  &subject_escaped,
	                  &description_escaped,
	                  is_private == NXS_YES ? "true" : "false");

	nxs_curl_set_post(&curl, (nxs_buf_t *)&data);

	if((ec = nxs_curl_query(&process, &curl, NXS_REST_API_COMMON_CMD_POST, (u_char *)"%r/issues.json", &nxs_chat_srv_cfg.rdmn.host)) !=
	   NXS_CURL_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: rdmn issue create error: curl error (project id: %zu, rc: %d)",
		                   nxs_proc_get_name(&process),
		                   project_id,
		                   ec);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	ret_code = nxs_curl_get_ret_code(&curl);
	b        = nxs_curl_get_out_buf(&curl);

	if(http_code != NULL) {

		*http_code = ret_code;
	}

	if(out_buf != NULL) {

		nxs_buf_clone(out_buf, b);
	}

	switch(ret_code) {

		case NXS_HTTP_CODE_201_CREATED:

			nxs_log_write_debug(&process,
			                    "[%s]: rdmn issue create: successfully created (project id: %zu)",
			                    nxs_proc_get_name(&process),
			                    project_id);

			rc = NXS_CHAT_SRV_E_OK;

			break;

		case NXS_HTTP_CODE_422_UNPROCESSABLE_ENTITY:

			nxs_log_write_warn(&process,
			                   "[%s]: rdmn issue create warn: Redmine unprocessable entity (project id: %zu, response code: "
			                   "%d, response body: \"%s\")",
			                   nxs_proc_get_name(&process),
			                   project_id,
			                   ret_code,
			                   nxs_buf_get_subbuf(b, 0));

			rc = NXS_CHAT_SRV_E_ATTR;

			break;

		default:

			nxs_log_write_error(&process,
			                    "[%s]: rdmn issue create error: wrong Redmine response code (project id: %zu, response code: "
			                    "%d, response body: \"%s\")",
			                    nxs_proc_get_name(&process),
			                    project_id,
			                    ret_code,
			                    nxs_buf_get_subbuf(b, 0));

			rc = NXS_CHAT_SRV_E_WARN;

			break;
	}

error:

	nxs_string_free(&data);
	nxs_string_free(&api_key);
	nxs_string_free(&subject_escaped);
	nxs_string_free(&description_escaped);

	nxs_curl_free(&curl);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_rdmn_issues_get_query(size_t           issue_query_id,
                                                        nxs_string_t *   user_api_key,
                                                        size_t           offset,
                                                        size_t           limit,
                                                        nxs_http_code_t *http_code,
                                                        nxs_buf_t *      out_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_curl_t         curl;
	nxs_string_t       api_key;
	nxs_http_code_t    ret_code;
	nxs_buf_t *        b;
	int                ec;

	if(user_api_key == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&api_key);

	nxs_curl_init(&curl);

	nxs_string_printf(&api_key, "X-Redmine-API-Key: %r", user_api_key);

	nxs_curl_add_header(&curl, &api_key);
	nxs_curl_add_header(&curl, &_s_content_type);

	nxs_curl_set_ssl_verivyhost(&curl, nxs_chat_srv_cfg.rdmn.ssl_verifyhost);

	if((ec = nxs_curl_query(&process,
	                        &curl,
	                        NXS_REST_API_COMMON_CMD_GET,
	                        (u_char *)"%r/issues.json?query_id=%zu&offset=%zu&limit=%zu",
	                        &nxs_chat_srv_cfg.rdmn.host,
	                        issue_query_id,
	                        offset,
	                        limit)) != NXS_CURL_E_OK) {

		nxs_log_write_warn(
		        &process,
		        "[%s]: rdmn get issues by query error: curl error (issues query id: %zu, offset: %zu, limit: %zu, rc: %d)",
		        nxs_proc_get_name(&process),
		        issue_query_id,
		        offset,
		        limit,
		        ec);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	ret_code = nxs_curl_get_ret_code(&curl);
	b        = nxs_curl_get_out_buf(&curl);

	if(http_code != NULL) {

		*http_code = ret_code;
	}

	if(out_buf != NULL) {

		nxs_buf_clone(out_buf, b);
	}

	switch(ret_code) {

		case NXS_HTTP_CODE_200_OK:

			nxs_log_write_debug(&process,
			                    "[%s]: rdmn get issues by query: success (issues query id: %zu, offset: %zu, limit: %zu)",
			                    nxs_proc_get_name(&process),
			                    issue_query_id,
			                    offset,
			                    limit);

			rc = NXS_CHAT_SRV_E_OK;

			break;

		default:

			nxs_log_write_error(&process,
			                    "[%s]: rdmn get issues by query error: wrong Redmine response code (issues query id: %zu, "
			                    "offset: %zu, limit: %zu, response code: %d, response body: \"%s\")",
			                    nxs_proc_get_name(&process),
			                    issue_query_id,
			                    offset,
			                    limit,
			                    ret_code,
			                    nxs_buf_get_subbuf(b, 0));

			rc = NXS_CHAT_SRV_E_WARN;

			break;
	}

error:

	nxs_string_free(&api_key);

	nxs_curl_free(&curl);

	return rc;
}

nxs_chat_srv_err_t
        nxs_chat_srv_d_rdmn_issues_get_issue(size_t issue_id, nxs_string_t *user_api_key, nxs_http_code_t *http_code, nxs_buf_t *out_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_curl_t         curl;
	nxs_string_t       api_key;
	nxs_http_code_t    ret_code;
	nxs_buf_t *        b;
	int                ec;

	if(user_api_key == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&api_key);

	nxs_curl_init(&curl);

	nxs_string_printf(&api_key, "X-Redmine-API-Key: %r", user_api_key);

	nxs_curl_add_header(&curl, &api_key);
	nxs_curl_add_header(&curl, &_s_content_type);

	nxs_curl_set_ssl_verivyhost(&curl, nxs_chat_srv_cfg.rdmn.ssl_verifyhost);

	if((ec = nxs_curl_query(
	            &process, &curl, NXS_REST_API_COMMON_CMD_GET, (u_char *)"%r/issues/%zu.json", &nxs_chat_srv_cfg.rdmn.host, issue_id)) !=
	   NXS_CURL_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: rdmn get issue error: curl error (issue id: %zu, rc: %d)",
		                   nxs_proc_get_name(&process),
		                   issue_id,
		                   ec);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	ret_code = nxs_curl_get_ret_code(&curl);
	b        = nxs_curl_get_out_buf(&curl);

	if(http_code != NULL) {

		*http_code = ret_code;
	}

	if(out_buf != NULL) {

		nxs_buf_clone(out_buf, b);
	}

	switch(ret_code) {

		case NXS_HTTP_CODE_200_OK:

			nxs_log_write_debug(
			        &process, "[%s]: rdmn get issue: success (issue id: %zu)", nxs_proc_get_name(&process), issue_id);

			rc = NXS_CHAT_SRV_E_OK;

			break;

		default:

			nxs_log_write_error(&process,
			                    "[%s]: rdmn get issue error: wrong Redmine response code (issue id: %zu, "
			                    "response code: %d, response body: \"%s\")",
			                    nxs_proc_get_name(&process),
			                    issue_id,
			                    ret_code,
			                    nxs_buf_get_subbuf(b, 0));

			rc = NXS_CHAT_SRV_E_WARN;

			break;
	}

error:

	nxs_string_free(&api_key);

	nxs_curl_free(&curl);

	return rc;
}

/* Module internal (static) functions */
