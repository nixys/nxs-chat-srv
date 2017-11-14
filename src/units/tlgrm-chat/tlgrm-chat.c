#ifndef _INCLUDE_NXS_CHAT_SRV_U_TLGRM_CHAT_C
#define _INCLUDE_NXS_CHAT_SRV_U_TLGRM_CHAT_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/tlgrm-chat/tlgrm-chat-typedefs.h>
#include <units/tlgrm-chat/tlgrm-chat.h>
#include <units/tlgrm-chat/tlgrm-chat-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_u_tlgrm_chat_s
{
	nxs_buf_t			response_buf;
	nxs_chat_srv_m_tlgrm_chat_t	chat;
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_u_tlgrm_chat_t *nxs_chat_srv_u_tlgrm_chat_init(void)
{
	nxs_chat_srv_u_tlgrm_chat_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_tlgrm_chat_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_tlgrm_chat_t));

	nxs_buf_init2(&u_ctx->response_buf);

	nxs_chat_srv_c_tlgrm_chat_init(&u_ctx->chat);

	return u_ctx;
}

nxs_chat_srv_u_tlgrm_chat_t *nxs_chat_srv_u_tlgrm_chat_free(nxs_chat_srv_u_tlgrm_chat_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	nxs_chat_srv_c_tlgrm_chat_free(&u_ctx->chat);

	nxs_buf_free(&u_ctx->response_buf);

	return (nxs_chat_srv_u_tlgrm_chat_t *)nxs_free(u_ctx);
}

/* This function can be reusable without context clean up */
nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_chat_pull(nxs_chat_srv_u_tlgrm_chat_t *u_ctx, size_t chat_id)
{
	nxs_chat_srv_err_t rc;
	nxs_string_t       message;
	nxs_bool_t         response_status;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&message);

	nxs_string_printf(&message, "{\"chat_id\":%zu}", chat_id);

	if(nxs_chat_srv_d_tlgrm_request(NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_GET_CHAT, &message, NULL, &u_ctx->response_buf) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process, "[%s]: can't get chat from tlgrm (chat id: %zu)", nxs_proc_get_name(&process), chat_id);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_c_tlgrm_chat_result_pull_json(&u_ctx->chat, &response_status, &u_ctx->response_buf) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_string_free(&message);

	return rc;
}

nxs_chat_srv_m_tlgrm_chat_t *nxs_chat_srv_u_tlgrm_chat_get(nxs_chat_srv_u_tlgrm_chat_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	return &u_ctx->chat;
}

/* Module internal (static) functions */
