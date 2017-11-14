#ifndef _INCLUDE_NXS_CHAT_SRV_D_RDMN_USERS_C
#define _INCLUDE_NXS_CHAT_SRV_D_RDMN_USERS_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/rdmn-users/rdmn-users-typedefs.h>
#include <dal/rdmn-users/rdmn-users.h>
#include <dal/rdmn-users/rdmn-users-subdals.h>

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

nxs_chat_srv_err_t nxs_chat_srv_d_rdmn_users_get(size_t           user_id,
                                                 nxs_bool_t       include_memberships,
                                                 nxs_string_t *   api_key,
                                                 size_t           offset,
                                                 size_t           limit,
                                                 nxs_http_code_t *http_code,
                                                 nxs_buf_t *      out_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_curl_t         curl;
	nxs_string_t       api_key_header;
	nxs_http_code_t    ret_code;
	nxs_buf_t *        b;
	int                ec;

	if(api_key == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_curl_init(&curl);

	nxs_string_init(&api_key_header);

	nxs_string_printf(&api_key_header, "X-Redmine-API-Key: %r", api_key);

	nxs_curl_add_header(&curl, &api_key_header);
	nxs_curl_add_header(&curl, &_s_content_type);

	nxs_curl_set_ssl_verivyhost(&curl, nxs_chat_srv_cfg.rdmn.ssl_verifyhost);

	if(user_id == 0) {

		if((ec = nxs_curl_query(&process,
		                        &curl,
		                        NXS_REST_API_COMMON_CMD_GET,
		                        (u_char *)"%r/users.json?limit=%zu&offset=%zu",
		                        &nxs_chat_srv_cfg.rdmn.host,
		                        limit,
		                        offset)) != NXS_CURL_E_OK) {

			nxs_log_write_warn(&process,
			                   "[%s]: rdmn users get error: curl error (user id: %zu, offset: %zu, limit: %zu, rc: %d)",
			                   nxs_proc_get_name(&process),
			                   user_id,
			                   offset,
			                   limit,
			                   ec);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}
	else {

		if((ec = nxs_curl_query(&process,
		                        &curl,
		                        NXS_REST_API_COMMON_CMD_GET,
		                        (u_char *)"%r/users/%zu.json%s",
		                        &nxs_chat_srv_cfg.rdmn.host,
		                        user_id,
		                        include_memberships == NXS_YES ? "?include=memberships" : "")) != NXS_CURL_E_OK) {

			nxs_log_write_warn(&process,
			                   "[%s]: rdmn users get error: curl error (user id: %zu, offset: %zu, limit: %zu, rc: %d)",
			                   nxs_proc_get_name(&process),
			                   user_id,
			                   offset,
			                   limit,
			                   ec);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
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
			                    "[%s]: rdmn users get: success (user id: %zu, offset: %zu, limit: %zu)",
			                    nxs_proc_get_name(&process),
			                    user_id,
			                    offset,
			                    limit);

			rc = NXS_CHAT_SRV_E_OK;

			break;

		default:

			nxs_log_write_error(
			        &process,
			        "[%s]: rdmn users get error: wrong Redmine response code (user id: %zu, offset: %zu, limit: %zu, "
			        "response code: %d, response body: \"%s\")",
			        nxs_proc_get_name(&process),
			        user_id,
			        offset,
			        limit,
			        ret_code,
			        nxs_buf_get_subbuf(b, 0));

			rc = NXS_CHAT_SRV_E_WARN;

			break;
	}

error:

	nxs_curl_free(&curl);

	nxs_string_free(&api_key_header);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_rdmn_users_get_current(nxs_string_t *   api_key,
                                                         nxs_bool_t       include_memberships,
                                                         nxs_http_code_t *http_code,
                                                         nxs_buf_t *      out_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_curl_t         curl;
	nxs_string_t       api_key_header;
	nxs_http_code_t    ret_code;
	nxs_buf_t *        b;
	int                ec;

	if(api_key == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_curl_init(&curl);

	nxs_string_init(&api_key_header);

	nxs_string_printf(&api_key_header, "X-Redmine-API-Key: %r", api_key);

	nxs_curl_add_header(&curl, &api_key_header);
	nxs_curl_add_header(&curl, &_s_content_type);

	nxs_curl_set_ssl_verivyhost(&curl, nxs_chat_srv_cfg.rdmn.ssl_verifyhost);

	if((ec = nxs_curl_query(&process,
	                        &curl,
	                        NXS_REST_API_COMMON_CMD_GET,
	                        (u_char *)"%r/users/current.json%s",
	                        &nxs_chat_srv_cfg.rdmn.host,
	                        include_memberships == NXS_YES ? "?include=memberships" : "")) != NXS_CURL_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: rdmn current user get error: curl error (rest api key: %r, rc: %d)",
		                   nxs_proc_get_name(&process),
		                   api_key,
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

			nxs_log_write_debug(&process, "[%s]: rdmn current user get: success", nxs_proc_get_name(&process));

			rc = NXS_CHAT_SRV_E_OK;

			break;

		default:

			nxs_log_write_error(&process,
			                    "[%s]: rdmn current user get error: wrong Redmine response code (rest api key: %r, "
			                    "response code: %d, response body: \"%s\")",
			                    nxs_proc_get_name(&process),
			                    api_key,
			                    ret_code,
			                    nxs_buf_get_subbuf(b, 0));

			rc = NXS_CHAT_SRV_E_WARN;

			break;
	}

error:

	nxs_curl_free(&curl);

	nxs_string_free(&api_key_header);

	return rc;
}

/* Module internal (static) functions */
