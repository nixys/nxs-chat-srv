#ifndef _INCLUDE_NXS_CHAT_SRV_C_USER_CTX_C
#define _INCLUDE_NXS_CHAT_SRV_C_USER_CTX_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t				process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

void nxs_chat_srv_c_user_ctx_init(nxs_chat_srv_m_user_ctx_t *user_ctx)
{

	if(user_ctx == NULL) {

		return;
	}

	user_ctx->r_userid = 0;

	nxs_string_init(&user_ctx->t_username);
	nxs_string_init(&user_ctx->t_userlang);
	nxs_string_init(&user_ctx->r_username);
	nxs_string_init(&user_ctx->r_userfname);
	nxs_string_init(&user_ctx->r_userlname);
}

void nxs_chat_srv_c_user_ctx_free(nxs_chat_srv_m_user_ctx_t *user_ctx)
{

	if(user_ctx == NULL) {

		return;
	}

	user_ctx->r_userid = 0;

	nxs_string_free(&user_ctx->t_username);
	nxs_string_free(&user_ctx->t_userlang);
	nxs_string_free(&user_ctx->r_username);
	nxs_string_free(&user_ctx->r_userfname);
	nxs_string_free(&user_ctx->r_userlname);
}

/* Module internal (static) functions */
