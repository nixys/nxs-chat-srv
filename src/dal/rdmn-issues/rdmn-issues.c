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

typedef struct
{
	size_t			id;
	nxs_string_t		value;
} nxs_chat_srv_d_rdmn_issues_cf_t;

typedef struct
{
	nxs_string_t		token;
	nxs_string_t		filename;
	nxs_string_t		content_type;
} nxs_chat_srv_d_rdmn_issues_upload_t;

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */

static nxs_string_t		_s_content_type_json		= nxs_string("Content-Type: application/json");
static nxs_string_t		_s_content_type_octet_stream	= nxs_string("Content-Type: application/octet-stream");

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_d_rdmn_issues_add_comment(size_t           issue_id,
                                                          size_t           assigned_to_id,
                                                          nxs_string_t *   note,
                                                          nxs_bool_t       private_notes,
                                                          size_t           status_id,
                                                          nxs_array_t *    uploads,
                                                          nxs_array_t *    custom_fields,
                                                          nxs_string_t *   user_api_key,
                                                          nxs_http_code_t *http_code,
                                                          nxs_buf_t *      out_buf)
{
	nxs_chat_srv_err_t               rc;
	nxs_curl_t                       curl;
	nxs_string_t                     data, api_key, note_escaped, status, assigned_to, uploads_str, uploads_str_els, cf_str, cf_str_els;
	size_t                           i;
	nxs_chat_srv_d_rdmn_issues_cf_t *cf;
	nxs_http_code_t                  ret_code;
	nxs_chat_srv_d_rdmn_issues_upload_t *u;
	nxs_buf_t *                          b;
	int                                  ec;

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
	nxs_string_init_empty(&uploads_str);
	nxs_string_init_empty(&uploads_str_els);

	nxs_curl_init(&curl);

	nxs_string_printf(&api_key, "X-Redmine-API-Key: %r", user_api_key);

	nxs_curl_add_header(&curl, &api_key);
	nxs_curl_add_header(&curl, &_s_content_type_json);

	nxs_curl_set_ssl_verivyhost(&curl, nxs_chat_srv_cfg.rdmn.ssl_verifyhost);

	nxs_string_escape(&note_escaped, note, NXS_STRING_ESCAPE_TYPE_JSON);

	if(status_id > 0) {

		nxs_string_printf(&status, ",\"status_id\":%zu", status_id);
	}

	if(assigned_to_id > 0) {

		nxs_string_printf(&assigned_to, ",\"assigned_to_id\":%zu", assigned_to_id);
	}

	if(uploads != NULL && nxs_array_count(uploads) > 0) {

		for(i = 0; i < nxs_array_count(uploads); i++) {

			u = nxs_array_get(uploads, i);

			if(i > 0) {

				nxs_string_char_add_char(&uploads_str_els, (u_char)',');
			}

			nxs_string_printf2_cat(&uploads_str_els,
			                       "{\"token\":\"%r\",\"filename\":\"%r\",\"content_type\":\"%r\"}",
			                       &u->token,
			                       &u->filename,
			                       &u->content_type);
		}

		nxs_string_printf(&uploads_str, ",\"uploads\":[%r]", &uploads_str_els);
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
	                  "{\"issue\":{"
	                  "\"notes\":\"%r\","
	                  "\"private_notes\":%s"
	                  "%r"
	                  "%r"
	                  "%r"
	                  "%r"
	                  "}"
	                  "}",
	                  &note_escaped,
	                  private_notes == NXS_YES ? "true" : "false",
	                  &status,
	                  &assigned_to,
	                  &uploads_str,
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
	nxs_string_free(&uploads_str);
	nxs_string_free(&uploads_str_els);

	nxs_curl_free(&curl);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_rdmn_issues_create(size_t           project_id,
                                                     size_t           priority_id,
                                                     nxs_string_t *   subject,
                                                     nxs_string_t *   description,
                                                     nxs_bool_t       is_private,
                                                     nxs_array_t *    uploads,
                                                     nxs_string_t *   user_api_key,
                                                     nxs_http_code_t *http_code,
                                                     nxs_buf_t *      out_buf)
{
	nxs_chat_srv_err_t                   rc;
	nxs_curl_t                           curl;
	nxs_string_t                         data, api_key, subject_escaped, description_escaped, uploads_str, uploads_str_els;
	nxs_http_code_t                      ret_code;
	nxs_chat_srv_d_rdmn_issues_upload_t *u;
	nxs_buf_t *                          b;
	size_t                               i;
	int                                  ec;

	if(subject == NULL || description == NULL || user_api_key == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&data);
	nxs_string_init(&api_key);
	nxs_string_init(&subject_escaped);
	nxs_string_init(&description_escaped);
	nxs_string_init_empty(&uploads_str);
	nxs_string_init_empty(&uploads_str_els);

	nxs_curl_init(&curl);

	nxs_string_printf(&api_key, "X-Redmine-API-Key: %r", user_api_key);

	nxs_curl_add_header(&curl, &api_key);
	nxs_curl_add_header(&curl, &_s_content_type_json);

	nxs_curl_set_ssl_verivyhost(&curl, nxs_chat_srv_cfg.rdmn.ssl_verifyhost);

	nxs_string_escape(&subject_escaped, subject, NXS_STRING_ESCAPE_TYPE_JSON);
	nxs_string_escape(&description_escaped, description, NXS_STRING_ESCAPE_TYPE_JSON);

	if(uploads != NULL && nxs_array_count(uploads) > 0) {

		for(i = 0; i < nxs_array_count(uploads); i++) {

			u = nxs_array_get(uploads, i);

			if(i > 0) {

				nxs_string_char_add_char(&uploads_str_els, (u_char)',');
			}

			nxs_string_printf2_cat(&uploads_str_els,
			                       "{\"token\":\"%r\",\"filename\":\"%r\",\"content_type\":\"%r\"}",
			                       &u->token,
			                       &u->filename,
			                       &u->content_type);
		}

		nxs_string_printf(&uploads_str, ",\"uploads\":[%r]", &uploads_str_els);
	}

	nxs_string_printf(&data,
	                  "{"
	                  "\"issue\":{"
	                  "\"project_id\":%zu,"
	                  "\"priority_id\":%zu,"
	                  "\"subject\":\"%r\","
	                  "\"description\":\"%r\","
	                  "\"is_private\": %s"
	                  "%r"
	                  "}"
	                  "}",
	                  project_id,
	                  priority_id,
	                  &subject_escaped,
	                  &description_escaped,
	                  is_private == NXS_YES ? "true" : "false",
	                  &uploads_str);

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
	nxs_string_free(&uploads_str);
	nxs_string_free(&uploads_str_els);

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
	nxs_curl_add_header(&curl, &_s_content_type_json);

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
	nxs_curl_add_header(&curl, &_s_content_type_json);

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

nxs_chat_srv_err_t nxs_chat_srv_d_rdmn_issues_file_upload(nxs_string_t *   user_api_key,
                                                          nxs_string_t *   file_path,
                                                          nxs_http_code_t *http_code,
                                                          nxs_buf_t *      out_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_curl_t         curl;
	nxs_string_t       api_key;
	nxs_http_code_t    ret_code;
	nxs_buf_t *        b;
	int                ec;

	if(user_api_key == NULL || file_path == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&api_key);

	nxs_curl_init(&curl);

	nxs_string_printf(&api_key, "X-Redmine-API-Key: %r", user_api_key);

	nxs_curl_add_header(&curl, &api_key);
	nxs_curl_add_header(&curl, &_s_content_type_octet_stream);

	nxs_curl_set_ssl_verivyhost(&curl, nxs_chat_srv_cfg.rdmn.ssl_verifyhost);

	if((ec = nxs_curl_upload(
	            &process, &curl, NXS_REST_API_COMMON_CMD_POST, file_path, (u_char *)"%r/uploads.json", &nxs_chat_srv_cfg.rdmn.host)) !=
	   NXS_CURL_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: rdmn file upload error: curl error (file path: %r, rc: %d)",
		                   nxs_proc_get_name(&process),
		                   file_path,
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
			                    "[%s]: rdmn file upload: note successfully uploaded (file path: %r)",
			                    nxs_proc_get_name(&process),
			                    file_path);

			rc = NXS_CHAT_SRV_E_OK;

			break;

		case NXS_HTTP_CODE_422_UNPROCESSABLE_ENTITY:

			nxs_log_write_warn(&process,
			                   "[%s]: rdmn file upload warn: Redmine unprocessable entity, possible exceeds the maximum file "
			                   "size allowed (file path: %r, response code: "
			                   "%d, response body: \"%s\")",
			                   nxs_proc_get_name(&process),
			                   file_path,
			                   ret_code,
			                   nxs_buf_get_subbuf(b, 0));

			rc = NXS_CHAT_SRV_E_ATTR;

			break;

		default:

			nxs_log_write_error(&process,
			                    "[%s]: rdmn file upload error: wrong Redmine response code (file path: %r, response code: "
			                    "%d, response body: \"%s\")",
			                    nxs_proc_get_name(&process),
			                    file_path,
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

void nxs_chat_srv_d_rdmn_issues_cf_init(nxs_array_t *custom_fields)
{

	if(custom_fields == NULL) {

		return;
	}

	nxs_array_init2(custom_fields, nxs_chat_srv_d_rdmn_issues_cf_t);
}

void nxs_chat_srv_d_rdmn_issues_cf_free(nxs_array_t *custom_fields)
{
	nxs_chat_srv_d_rdmn_issues_cf_t *cf;
	size_t                           i;

	if(custom_fields == NULL) {

		return;
	}

	for(i = 0; i < nxs_array_count(custom_fields); i++) {

		cf = nxs_array_get(custom_fields, i);

		cf->id = 0;

		nxs_string_free(&cf->value);
	}

	nxs_array_free(custom_fields);
}

void nxs_chat_srv_d_rdmn_issues_cf_add(nxs_array_t *custom_fields, size_t id, nxs_string_t *value)
{
	nxs_chat_srv_d_rdmn_issues_cf_t *cf;

	if(custom_fields == NULL || value == NULL) {

		return;
	}

	if(id == 0) {

		return;
	}

	cf = nxs_array_add(custom_fields);

	cf->id = id;

	nxs_string_init3(&cf->value, value);
}

void nxs_chat_srv_d_rdmn_issues_uploads_init(nxs_array_t *uploads)
{

	if(uploads == NULL) {

		return;
	}

	nxs_array_init2(uploads, nxs_chat_srv_d_rdmn_issues_upload_t);
}

void nxs_chat_srv_d_rdmn_issues_uploads_free(nxs_array_t *uploads)
{
	nxs_chat_srv_d_rdmn_issues_upload_t *u;
	size_t                               i;

	if(uploads == NULL) {

		return;
	}

	for(i = 0; i < nxs_array_count(uploads); i++) {

		u = nxs_array_get(uploads, i);

		nxs_string_free(&u->token);
		nxs_string_free(&u->filename);
		nxs_string_free(&u->content_type);
	}

	nxs_array_free(uploads);
}

void nxs_chat_srv_d_rdmn_issues_uploads_add(nxs_array_t *uploads, nxs_string_t *token, nxs_string_t *filename, nxs_string_t *content_type)
{
	nxs_chat_srv_d_rdmn_issues_upload_t *u;

	if(uploads == NULL || token == NULL || filename == NULL || content_type == NULL) {

		return;
	}

	u = nxs_array_add(uploads);

	nxs_string_init3(&u->token, token);
	nxs_string_init3(&u->filename, filename);
	nxs_string_init3(&u->content_type, content_type);
}

/* Module internal (static) functions */
