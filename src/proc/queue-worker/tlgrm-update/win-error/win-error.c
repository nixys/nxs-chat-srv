#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/queue-worker/tlgrm-update/win-error/win-error.h>
#include <proc/queue-worker/tlgrm-update/win-error/ctx/win-error-ctx.h>
#include <proc/queue-worker/tlgrm-update/win-error/win-error-subproc.h>

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



/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(nxs_chat_srv_u_db_sess_t * sess_ctx,
                                                                      nxs_chat_srv_m_user_ctx_t *user_ctx,
                                                                      size_t                     chat_id,
                                                                      nxs_buf_t *                response_buf)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_chat_srv_u_labels_t *           labels_ctx;
	nxs_buf_t *                         b;

	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();
	labels_ctx            = nxs_chat_srv_u_labels_init();

	nxs_chat_srv_u_labels_variable_add(labels_ctx, "redmine_url", "%r", &nxs_chat_srv_cfg.rdmn.host);

	nxs_chat_srv_u_tlgrm_sendmessage_add(
	        tlgrm_sendmessage_ctx,
	        chat_id,
	        nxs_chat_srv_u_labels_compile_key(labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_INTERNAL_SERVER_ERROR),
	        NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_HTML);

	nxs_chat_srv_u_tlgrm_sendmessage_disable_web_page_preview(tlgrm_sendmessage_ctx);

	if(nxs_chat_srv_u_tlgrm_sendmessage_push(tlgrm_sendmessage_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(response_buf != NULL) {

		b = nxs_chat_srv_u_tlgrm_sendmessage_get_response_buf(tlgrm_sendmessage_ctx);

		nxs_buf_clone(response_buf, b);
	}
error:

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);
	labels_ctx            = nxs_chat_srv_u_labels_free(labels_ctx);

	return rc;
}

/* Module internal (static) functions */
