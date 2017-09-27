#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/queue-worker/tlgrm-update/win-add-note-ext/win-add-note-ext.h>
#include <proc/queue-worker/tlgrm-update/win-add-note-ext/ctx/win-add-note-ext-ctx.h>
#include <proc/queue-worker/tlgrm-update/win-add-note-ext/win-add-note-ext-subproc.h>

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

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_win_add_note_ext(size_t     chat_id,
                                                                             size_t     message_id,
                                                                             size_t     issue_id,
                                                                             nxs_buf_t *response_buf)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *    tlgrm_sendmessage_ctx;
	nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx;
	nxs_string_t                            message;
	nxs_buf_t *                             b;
	nxs_chat_srv_err_t                      rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&message);

	tlgrm_sendmessage_ctx     = nxs_chat_srv_u_tlgrm_sendmessage_init();
	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

	nxs_string_printf(&message, NXS_CHAT_SRV_TLGRM_MESSAGE_ADD_NOTE_EXT);

	if(message_id == 0) {

		/* create new comment */

		nxs_chat_srv_u_tlgrm_sendmessage_add(
		        tlgrm_sendmessage_ctx, chat_id, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(
		        tlgrm_sendmessage_ctx,
		        0,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE_EXT_S_IN_PROGRESS,
		        issue_id,
		        NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_TO_STATUS_IN_PROGRESS);

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(
		        tlgrm_sendmessage_ctx,
		        1,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE_EXT_S_NEED_FEEDBACK,
		        issue_id,
		        NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_TO_STATUS_NEED_FEEDBACK);

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(
		        tlgrm_sendmessage_ctx,
		        2,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE_EXT_S_RESOLVED,
		        issue_id,
		        NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_TO_STATUS_RESOLVED);

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(tlgrm_sendmessage_ctx,
		                                                               3,
		                                                               0,
		                                                               NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE_EXT_PRIVATE,
		                                                               issue_id,
		                                                               NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_TO_PRIVATE_NOTE);

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(
		        tlgrm_sendmessage_ctx,
		        4,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE_EXT_WF_IGNORE,
		        issue_id,
		        NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_IGNORE_STATUS);

		nxs_chat_srv_u_tlgrm_sendmessage_disable_web_page_preview(tlgrm_sendmessage_ctx);

		if(nxs_chat_srv_u_tlgrm_sendmessage_push(tlgrm_sendmessage_ctx) != NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		if(response_buf != NULL) {

			b = nxs_chat_srv_u_tlgrm_sendmessage_get_response_buf(tlgrm_sendmessage_ctx);

			nxs_buf_clone(response_buf, b);
		}
	}
	else {

		/* update existing comment */

		nxs_chat_srv_u_tlgrm_editmessagetext_add(
		        tlgrm_editmessagetext_ctx, chat_id, message_id, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
		        tlgrm_editmessagetext_ctx,
		        0,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE_EXT_S_IN_PROGRESS,
		        issue_id,
		        NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_TO_STATUS_IN_PROGRESS);

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
		        tlgrm_editmessagetext_ctx,
		        1,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE_EXT_S_NEED_FEEDBACK,
		        issue_id,
		        NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_TO_STATUS_NEED_FEEDBACK);

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
		        tlgrm_editmessagetext_ctx,
		        2,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE_EXT_S_RESOLVED,
		        issue_id,
		        NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_TO_STATUS_RESOLVED);

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
		        tlgrm_editmessagetext_ctx,
		        3,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE_EXT_PRIVATE,
		        issue_id,
		        NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_TO_PRIVATE_NOTE);

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
		        tlgrm_editmessagetext_ctx,
		        4,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE_EXT_WF_IGNORE,
		        issue_id,
		        NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_IGNORE_STATUS);

		nxs_chat_srv_u_tlgrm_editmessagetext_disable_web_page_preview(tlgrm_editmessagetext_ctx);

		if(nxs_chat_srv_u_tlgrm_editmessagetext_push(tlgrm_editmessagetext_ctx) != NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		if(response_buf != NULL) {

			b = nxs_chat_srv_u_tlgrm_editmessagetext_get_response_buf(tlgrm_editmessagetext_ctx);

			nxs_buf_clone(response_buf, b);
		}
	}

error:

	nxs_string_free(&message);

	tlgrm_sendmessage_ctx     = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);
	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_free(tlgrm_editmessagetext_ctx);

	return rc;
}

/* Module internal (static) functions */
