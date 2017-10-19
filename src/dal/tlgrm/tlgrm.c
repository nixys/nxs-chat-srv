#ifndef _INCLUDE_NXS_CHAT_SRV_D_TLGRM_C
#define _INCLUDE_NXS_CHAT_SRV_D_TLGRM_C
#endif

// clang-format off

#include <curl/curl.h>

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/tlgrm/tlgrm-typedefs.h>
#include <dal/tlgrm/tlgrm.h>
#include <dal/tlgrm/tlgrm-subdals.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	nxs_chat_srv_tlgrm_request_type_t	type;
	nxs_string_t				name;
} nxs_chat_srv_d_tlgrm_method_t;

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_string_t *nxs_chat_srv_d_tlgrm_get_method(nxs_chat_srv_tlgrm_request_type_t type);
static size_t nxs_chat_srv_d_tlgrm_upload_get_data(void *buffer, size_t size, size_t nmemb, void *r);

// clang-format off

/* Module initializations */

static nxs_string_t		_s_content_type_json		= nxs_string("Content-Type: application/json");

static char			_s_form_photo[]			= "photo";
static char			_s_form_document[]		= "document";
static char			_s_form_chat_id[]		= "chat_id";
static char			_s_form_caption[]		= "caption";

nxs_chat_srv_d_tlgrm_method_t tlgrm_methods[] =
{
	{NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_MESSAGE,			nxs_string("sendMessage")},
	{NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_EDIT_MESSAGE_TEXT,		nxs_string("editMessageText")},
	{NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_ANSWER_CALLBACK_QUERY,		nxs_string("answerCallbackQuery")},
	{NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_GET_FILE,			nxs_string("getFile")},
	{NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_PHOTO,			nxs_string("sendPhoto")},
	{NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_DOCUMENT,			nxs_string("sendDocument")},

	{NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_NONE, {NULL, 0, 0}}
};

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_d_tlgrm_request(nxs_chat_srv_tlgrm_request_type_t type,
                                                nxs_string_t *                    body,
                                                nxs_http_code_t *                 http_code,
                                                nxs_buf_t *                       out_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_curl_t         curl;
	nxs_string_t *     method;
	nxs_http_code_t    ret_code;
	nxs_buf_t *        b;
	nxs_curl_err_t     ec;

	if(body == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((method = nxs_chat_srv_d_tlgrm_get_method(type)) == NULL) {

		nxs_log_write_warn(&process, "[%s]: unknown telegram request method type (type: %d)", type);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_curl_init(&curl);

	nxs_curl_add_header(&curl, &_s_content_type_json);

	nxs_curl_set_post(&curl, (nxs_buf_t *)body);

	if((ec = nxs_curl_query(&process,
	                        &curl,
	                        NXS_REST_API_COMMON_CMD_POST,
	                        (u_char *)"%r/bot%r/%r",
	                        &nxs_chat_srv_cfg.tlgrm.bot_api_addr,
	                        &nxs_chat_srv_cfg.tlgrm.bot_api_key,
	                        method)) != NXS_CURL_E_OK) {

		nxs_log_write_warn(&process, "[%s]: tlgrm request error: curl error (rc: %d)", nxs_proc_get_name(&process), ec);

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

			nxs_log_write_debug(&process, "[%s]: tlgrm request: success", nxs_proc_get_name(&process));

			rc = NXS_CHAT_SRV_E_OK;

			break;

		default:

			nxs_log_write_error(
			        &process,
			        "[%s]: tlgrm request error: wrong Telegram response code (response code: %d, response body: \"%s\")",
			        nxs_proc_get_name(&process),
			        ret_code,
			        nxs_buf_get_subbuf(b, 0));

			rc = NXS_CHAT_SRV_E_WARN;

			break;
	}

error:

	nxs_curl_free(&curl);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_tlgrm_download(nxs_string_t *tlgrm_file_name, nxs_string_t *file_save_path, nxs_http_code_t *http_code)
{
	nxs_chat_srv_err_t rc;
	nxs_curl_t         curl;
	nxs_http_code_t    ret_code;
	int                ec;

	if(tlgrm_file_name == NULL || file_save_path == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_curl_init(&curl);

	if((ec = nxs_curl_download(&process,
	                           &curl,
	                           NXS_REST_API_COMMON_CMD_GET,
	                           file_save_path,
	                           0600,
	                           (u_char *)"%r/file/bot%r/%r",
	                           &nxs_chat_srv_cfg.tlgrm.bot_api_addr,
	                           &nxs_chat_srv_cfg.tlgrm.bot_api_key,
	                           tlgrm_file_name)) != NXS_CURL_E_OK) {

		nxs_log_write_warn(&process, "[%s]: tlgrm file download error: curl error (rc: %d)", nxs_proc_get_name(&process), ec);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	ret_code = nxs_curl_get_ret_code(&curl);

	if(http_code != NULL) {

		*http_code = ret_code;
	}

	switch(ret_code) {

		case NXS_HTTP_CODE_200_OK:

			nxs_log_write_debug(&process, "[%s]: tlgrm file download: success", nxs_proc_get_name(&process));

			rc = NXS_CHAT_SRV_E_OK;

			break;

		default:

			nxs_log_write_error(&process,
			                    "[%s]: tlgrm file download error: wrong Telegram response code (response code: %d)",
			                    nxs_proc_get_name(&process),
			                    ret_code);

			rc = NXS_CHAT_SRV_E_WARN;

			break;
	}

error:

	nxs_curl_free(&curl);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_tlgrm_upload(nxs_chat_srv_tlgrm_request_type_t type,
                                               nxs_string_t *                    file_path,
                                               size_t                            chat_id,
                                               nxs_string_t *                    caption,
                                               nxs_http_code_t *                 http_code,
                                               nxs_buf_t *                       out_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_http_code_t    ret_code;
	nxs_string_t       tmp_str, *method;
	CURL *             c;
	CURLcode           res;
	long int           h_c = 0;

	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr  = NULL;

	if(file_path == NULL || out_buf == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((method = nxs_chat_srv_d_tlgrm_get_method(type)) == NULL) {

		nxs_log_write_warn(&process,
		                   "[%s]: tlgrm file upload error: unknown telegram request method type (file path: %r, method type: %d)",
		                   file_path,
		                   type);

		return NXS_CHAT_SRV_E_ERR;
	}

	nxs_buf_clear(out_buf);

	curl_global_init(CURL_GLOBAL_ALL);

	if((c = curl_easy_init()) == NULL) {

		nxs_log_write_warn(&process,
		                   "[%s]: tlgrm file upload error: curl error (file path: %r, rc: %d)",
		                   nxs_proc_get_name(&process),
		                   file_path,
		                   NXS_CURL_E_INIT);

		return NXS_CHAT_SRV_E_ERR;
	}

	nxs_string_init(&tmp_str);

	nxs_string_printf(&tmp_str, "%zu", chat_id);

	curl_formadd(&formpost,
	             &lastptr,
	             CURLFORM_COPYNAME,
	             _s_form_chat_id,
	             CURLFORM_COPYCONTENTS,
	             (char *)nxs_string_str(&tmp_str),
	             CURLFORM_END);

	curl_formadd(&formpost,
	             &lastptr,
	             CURLFORM_COPYNAME,
	             _s_form_caption,
	             CURLFORM_COPYCONTENTS,
	             (char *)nxs_string_str(caption),
	             CURLFORM_END);

	switch(type) {

		case NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_PHOTO:

			curl_formadd(&formpost,
			             &lastptr,
			             CURLFORM_COPYNAME,
			             _s_form_photo,
			             CURLFORM_FILE,
			             (char *)nxs_string_str(file_path),
			             CURLFORM_END);

			break;

		case NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_DOCUMENT:
		default:

			curl_formadd(&formpost,
			             &lastptr,
			             CURLFORM_COPYNAME,
			             _s_form_document,
			             CURLFORM_FILE,
			             (char *)nxs_string_str(file_path),
			             CURLFORM_END);

			break;
	}

	nxs_string_printf(&tmp_str, "%r/bot%r/%r", &nxs_chat_srv_cfg.tlgrm.bot_api_addr, &nxs_chat_srv_cfg.tlgrm.bot_api_key, method);

	curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, nxs_chat_srv_d_tlgrm_upload_get_data);
	curl_easy_setopt(c, CURLOPT_WRITEDATA, out_buf);
	curl_easy_setopt(c, CURLOPT_URL, nxs_string_str(&tmp_str));
	curl_easy_setopt(c, CURLOPT_HTTPPOST, formpost);

	res = curl_easy_perform(c);

	nxs_buf_add_char(out_buf, (u_char)'\0');

	if(res != CURLE_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: tlgrm file upload error, failed to process curl query get: %s (file path: %r, uri: \"%r\")",
		                   nxs_proc_get_name(&process),
		                   curl_easy_strerror(res),
		                   file_path,
		                   &tmp_str);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &h_c);

	ret_code = (nxs_http_code_t)h_c;

	if(http_code != NULL) {

		*http_code = ret_code;
	}

	switch(ret_code) {

		case NXS_HTTP_CODE_200_OK:

			nxs_log_write_debug(
			        &process, "[%s]: tlgrm file upload: success (file path: %r)", nxs_proc_get_name(&process), file_path);

			rc = NXS_CHAT_SRV_E_OK;

			break;

		default:

			nxs_log_write_error(&process,
			                    "[%s]: tlgrm file upload error: wrong Telegram response code (file path: %r, response code: "
			                    "%d, response body: \"%R\")",
			                    nxs_proc_get_name(&process),
			                    file_path,
			                    ret_code,
			                    out_buf);

			rc = NXS_CHAT_SRV_E_WARN;

			break;
	}

error:

	if(formpost != NULL) {

		curl_formfree(formpost);
	}

	nxs_string_free(&tmp_str);

	curl_easy_cleanup(c);

	return rc;
}

/* Module internal (static) functions */

static nxs_string_t *nxs_chat_srv_d_tlgrm_get_method(nxs_chat_srv_tlgrm_request_type_t type)
{
	size_t i;

	for(i = 0; tlgrm_methods[i].type != NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_NONE; i++) {

		if(tlgrm_methods[i].type == type) {

			return &tlgrm_methods[i].name;
		}
	}

	return NULL;
}

static size_t nxs_chat_srv_d_tlgrm_upload_get_data(void *buffer, size_t size, size_t nmemb, void *r)
{
	size_t     chunk_size;
	nxs_buf_t *response;

	chunk_size = size * nmemb;
	response   = r;

	nxs_buf_cpy(response, nxs_buf_get_len(response), buffer, chunk_size);

	return chunk_size;
}
