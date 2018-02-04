#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/queue-worker/tlgrm-update/win-select-issue/win-select-issue.h>
#include <proc/queue-worker/tlgrm-update/win-select-issue/ctx/win-select-issue-ctx.h>
#include <proc/queue-worker/tlgrm-update/win-select-issue/win-select-issue-subproc.h>

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

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_win_select_issue(nxs_chat_srv_u_db_sess_t *    sess_ctx,
                                                                             nxs_chat_srv_u_rdmn_issues_t *rdmn_issues_ctx,
                                                                             nxs_chat_srv_m_user_ctx_t *   user_ctx,
                                                                             size_t                        chat_id,
                                                                             size_t                        message_id,
                                                                             size_t                        offset,
                                                                             size_t                        issues_count,
                                                                             nxs_buf_t *                   response_buf)
{
	nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx;
	nxs_chat_srv_u_labels_t *               labels_ctx;
	nxs_buf_t *                             b;
	nxs_string_t *                          issue_subject;
	size_t                                  i, issue_id;
	nxs_chat_srv_err_t                      rc;

	rc = NXS_CHAT_SRV_E_OK;

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();
	labels_ctx                = nxs_chat_srv_u_labels_init();

	if(message_id == 0) {

		/* create new comment */
	}
	else {

		/* update existing comment */

		if(issues_count == 0) {

			nxs_chat_srv_u_tlgrm_editmessagetext_add(
			        tlgrm_editmessagetext_ctx,
			        chat_id,
			        message_id,
			        nxs_chat_srv_u_labels_compile_key(
			                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_SELECT_ISSUE_FOR_COMMENT_EMPTY),
			        NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_NONE);
		}
		else {

			nxs_chat_srv_u_tlgrm_editmessagetext_add(
			        tlgrm_editmessagetext_ctx,
			        chat_id,
			        message_id,
			        nxs_chat_srv_u_labels_compile_key(
			                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_SELECT_ISSUE_FOR_COMMENT),
			        NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_NONE);
		}

		for(i = 0; (issue_subject = nxs_chat_srv_u_rdmn_issues_get_shorts(rdmn_issues_ctx, i, &issue_id)) != NULL; i++) {

			nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(tlgrm_editmessagetext_ctx,
			                                                                   i,
			                                                                   0,
			                                                                   NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE,
			                                                                   issue_id,
			                                                                   (u_char *)"#%zu - %r",
			                                                                   issue_id,
			                                                                   issue_subject);
		}

		if(offset > 0) {

			nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
			        tlgrm_editmessagetext_ctx,
			        i,
			        0,
			        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECT_ISSUE,
			        offset - NXS_CHAT_SRV_TLGRM_ISSUES_LIMIT,
			        nxs_chat_srv_u_labels_compile_key_button(
			                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_BACK));
		}

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
		        tlgrm_editmessagetext_ctx,
		        i,
		        1,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_BACK,
		        0,
		        nxs_chat_srv_u_labels_compile_key_button(labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_CANCEL));

		if(issues_count > offset + NXS_CHAT_SRV_TLGRM_ISSUES_LIMIT) {

			nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
			        tlgrm_editmessagetext_ctx,
			        i,
			        2,
			        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECT_ISSUE,
			        offset + NXS_CHAT_SRV_TLGRM_PROJECTS_LIMIT,
			        nxs_chat_srv_u_labels_compile_key_button(
			                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_FORWARD));
		}

		if(nxs_chat_srv_u_tlgrm_editmessagetext_push(tlgrm_editmessagetext_ctx) != NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		if(response_buf != NULL) {

			b = nxs_chat_srv_u_tlgrm_editmessagetext_get_response_buf(tlgrm_editmessagetext_ctx);

			nxs_buf_clone(response_buf, b);
		}
	}

error:

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_free(tlgrm_editmessagetext_ctx);
	labels_ctx                = nxs_chat_srv_u_labels_free(labels_ctx);

	return rc;
}

/* Module internal (static) functions */
