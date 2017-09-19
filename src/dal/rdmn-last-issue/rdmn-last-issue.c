#ifndef _INCLUDE_NXS_CHAT_SRV_D_RDMN_LAST_ISSUE_C
#define _INCLUDE_NXS_CHAT_SRV_D_RDMN_LAST_ISSUE_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/rdmn-last-issue/rdmn-last-issue-typedefs.h>
#include <dal/rdmn-last-issue/rdmn-last-issue.h>
#include <dal/rdmn-last-issue/rdmn-last-issue-subdals.h>

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

nxs_chat_srv_err_t
        nxs_chat_srv_d_rdmn_last_issue_get(size_t rdmn_userid, nxs_string_t *api_key, nxs_buf_t *out_buf, nxs_http_code_t *http_code)
{
	nxs_chat_srv_err_t rc;
	nxs_curl_t         curl;
	nxs_string_t       api_key_header;
	nxs_http_code_t    ret_code;
	int                ec;

	if(api_key == NULL || out_buf == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_buf_clear(out_buf);

	nxs_curl_init(&curl);

	nxs_string_init(&api_key_header);

	nxs_string_printf(&api_key_header, "X-Redmine-API-Key: %r", api_key);

	nxs_curl_add_header(&curl, &api_key_header);
	nxs_curl_add_header(&curl, &_s_content_type);

	nxs_curl_set_ssl_verivyhost(&curl, nxs_chat_srv_cfg.rdmn.ssl_verifyhost);

	if((ec = nxs_curl_query(&process,
	                        &curl,
	                        NXS_REST_API_COMMON_CMD_GET,
	                        (u_char *)"%r/users/%zu/last_issue.json",
	                        &nxs_chat_srv_cfg.rdmn.host,
	                        rdmn_userid)) != NXS_CURL_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: rdmn last issue get error: curl error (rc: %d, rdmn_userid: %zu)",
		                   nxs_proc_get_name(&process),
		                   ec,
		                   rdmn_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	ret_code = nxs_curl_get_ret_code(&curl);

	if(http_code != NULL) {

		*http_code = ret_code;
	}

	switch(ret_code) {

		case NXS_HTTP_CODE_200_OK:

			nxs_buf_clone(out_buf, nxs_curl_get_out_buf(&curl));

			break;

		default:

			nxs_log_write_warn(&process,
			                   "[%s]: rdmn last issue get error: wrong http error code (error http code: %d, rdmn_userid: %zu)",
			                   nxs_proc_get_name(&process),
			                   ret_code,
			                   rdmn_userid);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);

			break;
	}

error:

	nxs_curl_free(&curl);

	nxs_string_free(&api_key_header);

	return rc;
}

/* Module internal (static) functions */
