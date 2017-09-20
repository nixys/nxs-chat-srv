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
                                                          nxs_string_t *   note,
                                                          nxs_string_t *   user_api_key,
                                                          nxs_buf_t *      out_buf,
                                                          nxs_http_code_t *http_code)
{
	nxs_chat_srv_err_t rc;
	nxs_curl_t         curl;
	nxs_string_t       data, api_key, note_escaped;
	int                ec;

	if(note == NULL || user_api_key == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&data);
	nxs_string_init(&api_key);
	nxs_string_init(&note_escaped);

	nxs_curl_init(&curl);

	nxs_string_printf(&api_key, "X-Redmine-API-Key: %r", user_api_key);

	nxs_curl_add_header(&curl, &api_key);
	nxs_curl_add_header(&curl, &_s_content_type);

	nxs_curl_set_ssl_verivyhost(&curl, nxs_chat_srv_cfg.rdmn.ssl_verifyhost);

	nxs_string_escape(&note_escaped, note, NXS_STRING_ESCAPE_TYPE_JSON);

	nxs_string_printf(&data, "{\"issue\":{\"notes\":\"%r\"}}", &note_escaped);

	nxs_curl_set_post(&curl, (nxs_buf_t *)&data);

	if((ec = nxs_curl_query(
	            &process, &curl, NXS_REST_API_COMMON_CMD_PUT, (u_char *)"%r/issues/%zu.json", &nxs_chat_srv_cfg.rdmn.host, issue_id)) !=
	   NXS_CURL_E_OK) {

		nxs_log_write_warn(&process, "[%s]: redmine add issue comment error: curl error (rc: %d)", nxs_proc_get_name(&process), ec);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(http_code != NULL) {

		*http_code = nxs_curl_get_ret_code(&curl);
	}

	if(out_buf != NULL) {

		nxs_buf_clone(out_buf, nxs_curl_get_out_buf(&curl));
	}

error:

	nxs_string_free(&data);
	nxs_string_free(&api_key);
	nxs_string_free(&note_escaped);

	nxs_curl_free(&curl);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_rdmn_issues_create(size_t           project_id,
                                                     size_t           priority_id,
                                                     nxs_string_t *   subject,
                                                     nxs_string_t *   description,
                                                     nxs_string_t *   user_api_key,
                                                     nxs_buf_t *      out_buf,
                                                     nxs_http_code_t *http_code)
{
	nxs_chat_srv_err_t rc;
	nxs_curl_t         curl;
	nxs_string_t       data, api_key, subject_escaped, description_escaped;
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
	                  "\"description\":\"%r\""
	                  "}"
	                  "}",
	                  project_id,
	                  priority_id,
	                  &subject_escaped,
	                  &description_escaped);

	nxs_curl_set_post(&curl, (nxs_buf_t *)&data);

	if((ec = nxs_curl_query(&process, &curl, NXS_REST_API_COMMON_CMD_POST, (u_char *)"%r/issues.json", &nxs_chat_srv_cfg.rdmn.host)) !=
	   NXS_CURL_E_OK) {

		nxs_log_write_warn(&process, "[%s]: redmine create issue error: curl error (rc: %d)", nxs_proc_get_name(&process), ec);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(http_code != NULL) {

		*http_code = nxs_curl_get_ret_code(&curl);
	}

	if(out_buf != NULL) {

		nxs_buf_clone(out_buf, nxs_curl_get_out_buf(&curl));
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
                                                        nxs_buf_t *      out_buf,
                                                        nxs_http_code_t *http_code)
{
	nxs_chat_srv_err_t rc;
	nxs_curl_t         curl;
	nxs_string_t       api_key;
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
		        &process, "[%s]: redmine get issues by query error: curl error (rc: %d)", nxs_proc_get_name(&process), ec);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(http_code != NULL) {

		*http_code = nxs_curl_get_ret_code(&curl);
	}

	if(out_buf != NULL) {

		nxs_buf_clone(out_buf, nxs_curl_get_out_buf(&curl));
	}

error:

	nxs_string_free(&api_key);

	nxs_curl_free(&curl);

	return rc;
}

/* Module internal (static) functions */
