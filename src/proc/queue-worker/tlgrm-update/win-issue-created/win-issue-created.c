#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/queue-worker/tlgrm-update/win-issue-created/win-issue-created.h>
#include <proc/queue-worker/tlgrm-update/win-issue-created/ctx/win-issue-created-ctx.h>
#include <proc/queue-worker/tlgrm-update/win-issue-created/win-issue-created-subproc.h>

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

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_win_issue_created(nxs_chat_srv_u_db_sess_t * sess_ctx,
                                                                              nxs_chat_srv_m_user_ctx_t *user_ctx,
                                                                              size_t                     chat_id,
                                                                              size_t                     message_id,
                                                                              size_t                     new_issue_id,
                                                                              nxs_buf_t *                response_buf)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *    tlgrm_sendmessage_ctx;
	nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx;
	nxs_chat_srv_u_labels_t *               labels_ctx;
	nxs_buf_t *                             b;
	nxs_string_t                            subject, message, project;
	nxs_chat_srv_err_t                      rc;

	rc = NXS_CHAT_SRV_E_OK;

	tlgrm_sendmessage_ctx     = nxs_chat_srv_u_tlgrm_sendmessage_init();
	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();
	labels_ctx                = nxs_chat_srv_u_labels_init();

	nxs_string_init(&subject);
	nxs_string_init(&message);
	nxs_string_init(&project);

	if(nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx, NULL, &project, NULL, NULL, &subject, NULL, NULL, NULL, NULL) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_chat_srv_c_tlgrm_format_escape_html(NULL, &project);
	nxs_chat_srv_c_tlgrm_format_escape_html(NULL, &subject);

	nxs_chat_srv_u_labels_variable_add(labels_ctx,
	                                   "issue_link",
	                                   NXS_CHAT_SRV_TLGRM_MESSAGE_ISSUE_LINK_FMT,
	                                   &nxs_chat_srv_cfg.rdmn.url,
	                                   new_issue_id,
	                                   &project,
	                                   new_issue_id,
	                                   &subject);

	nxs_string_clone(&message,
	                 nxs_chat_srv_u_labels_compile_key(labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_ISSUE_CREATED));

	if(message_id == 0) {

		/* create new comment */

		nxs_chat_srv_u_tlgrm_sendmessage_add(tlgrm_sendmessage_ctx, chat_id, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_HTML);

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
		        tlgrm_editmessagetext_ctx, chat_id, message_id, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_HTML);

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

	tlgrm_sendmessage_ctx     = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);
	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_free(tlgrm_editmessagetext_ctx);
	labels_ctx                = nxs_chat_srv_u_labels_free(labels_ctx);

	nxs_string_free(&subject);
	nxs_string_free(&message);
	nxs_string_free(&project);

	return rc;
}

/* Module internal (static) functions */
