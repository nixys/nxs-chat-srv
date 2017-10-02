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

static nxs_string_t _s_msg_select_issue	= nxs_string(NXS_CHAT_SRV_TLGRM_MESSAGE_SELECT_ISSUE);

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_win_select_issue(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                                             size_t                         chat_id,
                                                                             size_t                         message_id,
                                                                             nxs_chat_srv_m_tlgrm_update_t *update,
                                                                             nxs_array_t *                  issues,
                                                                             size_t                         offset,
                                                                             size_t                         issues_count,
                                                                             nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx;
	nxs_buf_t *                             b;
	nxs_chat_srv_m_rdmn_issues_query_t *    iss;
	size_t                                  i;
	nxs_chat_srv_err_t                      rc;

	rc = NXS_CHAT_SRV_E_OK;

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

	if(message_id == 0) {

		/* create new comment */
	}
	else {

		/* update existing comment */

		tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

		nxs_chat_srv_u_tlgrm_editmessagetext_add(
		        tlgrm_editmessagetext_ctx, chat_id, message_id, &_s_msg_select_issue, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_NONE);

		for(i = 0; i < nxs_array_count(issues); i++) {

			iss = nxs_array_get(issues, i);

			nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(tlgrm_editmessagetext_ctx,
			                                                                   i,
			                                                                   0,
			                                                                   NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE,
			                                                                   iss->id,
			                                                                   (u_char *)"#%zu - %r",
			                                                                   iss->id,
			                                                                   &iss->subject);
		}

		if(offset > 0) {

			nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
			        tlgrm_editmessagetext_ctx,
			        i,
			        0,
			        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECT_ISSUE,
			        offset - NXS_CHAT_SRV_TLGRM_ISSUES_LIMIT,
			        NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_BACK);
		}

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(tlgrm_editmessagetext_ctx,
		                                                                   i,
		                                                                   1,
		                                                                   NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_BACK,
		                                                                   0,
		                                                                   NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_CANCEL);

		if(issues_count > offset + NXS_CHAT_SRV_TLGRM_ISSUES_LIMIT) {

			nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
			        tlgrm_editmessagetext_ctx,
			        i,
			        2,
			        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECT_ISSUE,
			        offset + NXS_CHAT_SRV_TLGRM_PROJECTS_LIMIT,
			        NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_FORWARD);
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

	return rc;
}

/* Module internal (static) functions */
