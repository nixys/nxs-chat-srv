#ifndef _INCLUDE_NXS_CHAT_SRV_D_TLGRM_C
#define _INCLUDE_NXS_CHAT_SRV_D_TLGRM_C
#endif

// clang-format off

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

// clang-format off

/* Module initializations */

static nxs_string_t		_s_content_type		= nxs_string("Content-Type: application/json");

nxs_chat_srv_d_tlgrm_method_t tlgrm_methods[] =
{
	{NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_MESSAGE,			nxs_string("sendMessage")},
	{NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_EDIT_MESSAGE_TEXT,		nxs_string("editMessageText")},
	{NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_ANSWER_CALLBACK_QUERY,		nxs_string("answerCallbackQuery")},

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
	int                ec;

	if(body == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((method = nxs_chat_srv_d_tlgrm_get_method(type)) == NULL) {

		nxs_log_write_warn(&process, "[%s]: unknown telegram request method type (type: %d)", type);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_curl_init(&curl);

	nxs_curl_add_header(&curl, &_s_content_type);

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
