#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/queue-worker/tlgrm-update/win-wrong-action/win-wrong-action.h>
#include <proc/queue-worker/tlgrm-update/win-wrong-action/ctx/win-wrong-action-ctx.h>
#include <proc/queue-worker/tlgrm-update/win-wrong-action/win-wrong-action-subproc.h>

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

static nxs_string_t	_s_msg_wrong_action	= nxs_string(NXS_CHAT_SRV_TLGRM_MESSAGE_WRONG_ACTION);

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_win_wrong_action(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                                             size_t                         chat_id,
                                                                             nxs_chat_srv_m_tlgrm_update_t *update,
                                                                             nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_buf_t *                         b;

	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();

	nxs_chat_srv_u_tlgrm_sendmessage_add(
	        tlgrm_sendmessage_ctx, chat_id, &_s_msg_wrong_action, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_NONE);

	nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(tlgrm_sendmessage_ctx,
	                                                               0,
	                                                               0,
	                                                               NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SESSION_DESTROY,
	                                                               0,
	                                                               NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_DESTROY_SESSION);

	if(nxs_chat_srv_u_tlgrm_sendmessage_push(tlgrm_sendmessage_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(response_buf != NULL) {

		b = nxs_chat_srv_u_tlgrm_sendmessage_get_response_buf(tlgrm_sendmessage_ctx);

		nxs_buf_clone(response_buf, b);
	}
error:

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);

	return rc;
}

/* Module internal (static) functions */
