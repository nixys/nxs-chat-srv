#ifndef _INCLUDE_NXS_CHAT_SRV_U_TLGRM_SENDMESSAGE_C
#define _INCLUDE_NXS_CHAT_SRV_U_TLGRM_SENDMESSAGE_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/tlgrm-sendmessage/tlgrm-sendmessage-typedefs.h>
#include <units/tlgrm-sendmessage/tlgrm-sendmessage.h>
#include <units/tlgrm-sendmessage/tlgrm-sendmessage-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	size_t			chat_id;
	nxs_string_t		text;
} nxs_chat_srv_u_tlgrm_sendmessage_el_t;

struct nxs_chat_srv_u_tlgrm_sendmessage_s
{
	nxs_array_t		messages;	/* type: nxs_chat_srv_u_tlgrm_sendmessage_el_t */
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_u_tlgrm_sendmessage_t *nxs_chat_srv_u_tlgrm_sendmessage_init(void)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_tlgrm_sendmessage_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_tlgrm_sendmessage_t));

	nxs_array_init2(&u_ctx->messages, nxs_chat_srv_u_tlgrm_sendmessage_el_t);

	return u_ctx;
}

nxs_chat_srv_u_tlgrm_sendmessage_t *nxs_chat_srv_u_tlgrm_sendmessage_free(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx)
{
	nxs_chat_srv_u_tlgrm_sendmessage_el_t *m;
	size_t                                 i;

	if(u_ctx == NULL) {

		return NULL;
	}

	for(i = 0; i < nxs_array_count(&u_ctx->messages); i++) {

		m = nxs_array_get(&u_ctx->messages, i);

		m->chat_id = 0;

		nxs_string_free(&m->text);
	}

	nxs_array_free(&u_ctx->messages);

	return (nxs_chat_srv_u_tlgrm_sendmessage_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_sendmessage_push(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx)
{
	nxs_chat_srv_u_tlgrm_sendmessage_el_t *m;
	size_t                                 i;
	nxs_chat_srv_err_t                     rc;
	nxs_string_t                           message;
	nxs_http_code_t                        http_code;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&message);

	for(i = 0; i < nxs_array_count(&u_ctx->messages); i++) {

		m = nxs_array_get(&u_ctx->messages, i);

		nxs_string_printf_dyn(&message, "{\"chat_id\":%zu,\"text\":\"%r\"}", m->chat_id, &m->text);

		if(nxs_chat_srv_d_tlgrm_request(NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_MESSAGE, &message, NULL, &http_code) !=
		   NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		if(http_code != NXS_HTTP_CODE_200_OK) {

			nxs_log_write_warn(&process,
			                   "[%s]: wrong telegram send message http code (http code: %d)",
			                   nxs_proc_get_name(&process),
			                   http_code);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}

error:

	nxs_string_free(&message);

	return rc;
}

void nxs_chat_srv_u_tlgrm_sendmessage_add(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx, size_t chat_id, nxs_string_t *text)
{
	nxs_chat_srv_u_tlgrm_sendmessage_el_t *m;

	if(u_ctx == NULL) {

		return;
	}

	m = nxs_array_add(&u_ctx->messages);

	m->chat_id = chat_id;

	nxs_string_init3(&m->text, text);
}

/* Module internal (static) functions */
