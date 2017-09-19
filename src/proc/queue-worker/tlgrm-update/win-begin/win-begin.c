#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/queue-worker/tlgrm-update/win-begin/win-begin.h>
#include <proc/queue-worker/tlgrm-update/win-begin/ctx/win-begin-ctx.h>
#include <proc/queue-worker/tlgrm-update/win-begin/win-begin-subproc.h>

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

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_win_begin(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                                      size_t                         chat_id,
                                                                      size_t                         message_id,
                                                                      size_t                         rdmn_userid,
                                                                      nxs_chat_srv_m_tlgrm_update_t *update,
                                                                      nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *    tlgrm_sendmessage_ctx;
	nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx;
	nxs_chat_srv_u_rdmn_last_issue_t *      rdmn_last_issue_ctx;
	nxs_string_t                            callback_str, message, issue_subject;
	nxs_buf_t *                             b;
	nxs_chat_srv_err_t                      rc;
	size_t                                  issue_id;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);
	nxs_string_init(&message);
	nxs_string_init(&issue_subject);

	tlgrm_sendmessage_ctx     = nxs_chat_srv_u_tlgrm_sendmessage_init();
	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();
	rdmn_last_issue_ctx       = nxs_chat_srv_u_rdmn_last_issue_init();

	if(nxs_chat_srv_u_rdmn_last_issue_pull(rdmn_last_issue_ctx, rdmn_userid) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_chat_srv_u_rdmn_last_issue_get(rdmn_last_issue_ctx, &issue_id, &issue_subject);

	nxs_string_printf(&message, NXS_CHAT_SRV_TLGRM_MESSAGE_BEGIN, issue_id, &issue_subject, &nxs_chat_srv_cfg.rdmn.host, issue_id);

	if(message_id == 0) {

		/* create new comment */

		nxs_chat_srv_u_tlgrm_sendmessage_add(
		        tlgrm_sendmessage_ctx, chat_id, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(tlgrm_sendmessage_ctx,
		                                                               0,
		                                                               0,
		                                                               NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE,
		                                                               issue_id,
		                                                               NXS_CHAT_SRV_TLGRM_BUTTON_CAPTON_ADD_LAST_ACTIVE);

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(tlgrm_sendmessage_ctx,
		                                                               1,
		                                                               0,
		                                                               NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_NEW_ISSUE,
		                                                               0,
		                                                               NXS_CHAT_SRV_TLGRM_BUTTON_CAPTON_NEW_ISSUE);

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(tlgrm_sendmessage_ctx,
		                                                               2,
		                                                               0,
		                                                               NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_NEW_ISSUE,
		                                                               0,
		                                                               NXS_CHAT_SRV_TLGRM_BUTTON_CAPTON_CHOOSE_ISSUE);

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

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(tlgrm_editmessagetext_ctx,
		                                                                   0,
		                                                                   0,
		                                                                   NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE,
		                                                                   issue_id,
		                                                                   NXS_CHAT_SRV_TLGRM_BUTTON_CAPTON_ADD_LAST_ACTIVE);

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(tlgrm_editmessagetext_ctx,
		                                                                   1,
		                                                                   0,
		                                                                   NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_NEW_ISSUE,
		                                                                   0,
		                                                                   NXS_CHAT_SRV_TLGRM_BUTTON_CAPTON_NEW_ISSUE);

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(tlgrm_editmessagetext_ctx,
		                                                                   2,
		                                                                   0,
		                                                                   NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_NEW_ISSUE,
		                                                                   0,
		                                                                   NXS_CHAT_SRV_TLGRM_BUTTON_CAPTON_CHOOSE_ISSUE);

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

	nxs_string_free(&callback_str);
	nxs_string_free(&message);
	nxs_string_free(&issue_subject);

	tlgrm_sendmessage_ctx     = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);
	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_free(tlgrm_editmessagetext_ctx);
	rdmn_last_issue_ctx       = nxs_chat_srv_u_rdmn_last_issue_free(rdmn_last_issue_ctx);

	return rc;
}

/* Module internal (static) functions */
