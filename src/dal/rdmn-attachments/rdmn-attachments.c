#ifndef _INCLUDE_NXS_CHAT_SRV_D_RDMN_ATTACHMENTS_C
#define _INCLUDE_NXS_CHAT_SRV_D_RDMN_ATTACHMENTS_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/rdmn-attachments/rdmn-attachments-typedefs.h>
#include <dal/rdmn-attachments/rdmn-attachments.h>
#include <dal/rdmn-attachments/rdmn-attachments-subdals.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */

static nxs_string_t		_s_content_type		= nxs_string("Content-Type: application/json");

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_d_rdmn_attachments_info(size_t attachment_id, nxs_http_code_t *http_code, nxs_buf_t *out_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_curl_t         curl;
	nxs_string_t       api_key;
	nxs_http_code_t    ret_code;
	nxs_buf_t *        b;
	int                ec;

	if(attachment_id == 0) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&api_key);

	nxs_curl_init(&curl);

	nxs_string_printf(&api_key, "X-Redmine-API-Key: %r", &nxs_chat_srv_cfg.rdmn.api_key);

	nxs_curl_add_header(&curl, &api_key);
	nxs_curl_add_header(&curl, &_s_content_type);

	nxs_curl_set_ssl_verivyhost(&curl, nxs_chat_srv_cfg.rdmn.ssl_verifyhost);

	if((ec = nxs_curl_query(&process,
	                        &curl,
	                        NXS_REST_API_COMMON_CMD_GET,
	                        (u_char *)"%r/attachments/%zu.json",
	                        &nxs_chat_srv_cfg.rdmn.host,
	                        attachment_id)) != NXS_CURL_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: rdmn get attachment info error: curl error (attachment id: %zu, rc: %d)",
		                   nxs_proc_get_name(&process),
		                   attachment_id,
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
			                    "[%s]: rdmn get attachment info: success (attachment id: %zu)",
			                    nxs_proc_get_name(&process),
			                    attachment_id);

			rc = NXS_CHAT_SRV_E_OK;

			break;

		default:

			nxs_log_write_error(&process,
			                    "[%s]: rdmn get attachment info error: wrong Redmine response code (attachment id: %zu, "
			                    "response code: %d, response body: \"%s\")",
			                    nxs_proc_get_name(&process),
			                    attachment_id,
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
        nxs_chat_srv_d_rdmn_attachments_download(nxs_string_t *rdmn_content_url, nxs_string_t *file_save_path, nxs_http_code_t *http_code)
{
	nxs_chat_srv_err_t rc;
	nxs_curl_t         curl;
	nxs_string_t       api_key;
	nxs_http_code_t    ret_code;
	int                ec;

	if(rdmn_content_url == NULL || file_save_path == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&api_key);

	nxs_curl_init(&curl);

	nxs_string_printf(&api_key, "X-Redmine-API-Key: %r", &nxs_chat_srv_cfg.rdmn.api_key);

	nxs_curl_add_header(&curl, &api_key);

	nxs_curl_set_ssl_verivyhost(&curl, nxs_chat_srv_cfg.rdmn.ssl_verifyhost);

	if((ec = nxs_curl_download(&process,
	                           &curl,
	                           NXS_REST_API_COMMON_CMD_GET,
	                           file_save_path,
	                           0600,
	                           (u_char *)"%r?key=%r",
	                           rdmn_content_url,
	                           &nxs_chat_srv_cfg.rdmn.api_key)) != NXS_CURL_E_OK) {

		nxs_log_write_warn(&process, "[%s]: rdmn file download error: curl error (rc: %d)", nxs_proc_get_name(&process), ec);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	ret_code = nxs_curl_get_ret_code(&curl);

	if(http_code != NULL) {

		*http_code = ret_code;
	}

	switch(ret_code) {

		case NXS_HTTP_CODE_200_OK:

			nxs_log_write_debug(&process, "[%s]: rdmn file download: success", nxs_proc_get_name(&process));

			rc = NXS_CHAT_SRV_E_OK;

			break;

		default:

			nxs_log_write_error(&process,
			                    "[%s]: rdmn file download error: wrong Redmine response code (response code: %d)",
			                    nxs_proc_get_name(&process),
			                    ret_code);

			rc = NXS_CHAT_SRV_E_WARN;

			break;
	}

error:

	nxs_string_free(&api_key);

	nxs_curl_free(&curl);

	return rc;
}

/* Module internal (static) functions */
