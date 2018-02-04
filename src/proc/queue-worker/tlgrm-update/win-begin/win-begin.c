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

static char		_s_private_message[]	= {NXS_CHAT_SRV_UTF8_PRIVATE_MESSAGE};

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_win_begin(nxs_chat_srv_m_user_ctx_t *user_ctx,
                                                                      size_t                     chat_id,
                                                                      size_t                     message_id,
                                                                      nxs_string_t *             user_api_key,
                                                                      nxs_bool_t                 private_notes,
                                                                      size_t                     files_count,
                                                                      nxs_buf_t *                response_buf)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *    tlgrm_sendmessage_ctx;
	nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx;
	nxs_chat_srv_u_labels_t *               labels_ctx;
	nxs_chat_srv_u_last_issues_t *          last_issue_ctx;
	nxs_string_t                            callback_str, message, last_issue_str, issue_subject, issue_project_name;
	nxs_buf_t *                             b;
	nxs_chat_srv_err_t                      rc;
	size_t                                  issue_id;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);
	nxs_string_init(&message);
	nxs_string_init(&last_issue_str);
	nxs_string_init_empty(&issue_subject);
	nxs_string_init_empty(&issue_project_name);

	tlgrm_sendmessage_ctx     = nxs_chat_srv_u_tlgrm_sendmessage_init();
	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();
	last_issue_ctx            = nxs_chat_srv_u_last_issues_init();
	labels_ctx                = nxs_chat_srv_u_labels_init();

	if(nxs_chat_srv_u_last_issues_get(
	           last_issue_ctx, chat_id, user_ctx->r_userid, user_api_key, &issue_id, &issue_subject, &issue_project_name) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_chat_srv_c_tlgrm_format_escape_html(NULL, &issue_subject);
	nxs_chat_srv_c_tlgrm_format_escape_html(NULL, &issue_project_name);

	if(issue_id == 0) {

		nxs_string_clone(&last_issue_str,
		                 nxs_chat_srv_u_labels_compile_key(labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_ABSENT));
	}
	else {

		nxs_string_printf(&last_issue_str,
		                  NXS_CHAT_SRV_TLGRM_MESSAGE_ISSUE_LINK_FMT,
		                  &nxs_chat_srv_cfg.rdmn.host,
		                  issue_id,
		                  &issue_project_name,
		                  issue_id,
		                  &issue_subject);
	}

	/*
	 * prepare label
	 */

	nxs_chat_srv_u_labels_variable_clear(labels_ctx);

	nxs_chat_srv_u_labels_variable_add(labels_ctx, "number_of_attached_files", "%zu", files_count);
	nxs_chat_srv_u_labels_variable_add(labels_ctx, "is_private_message", "%s", private_notes == NXS_YES ? _s_private_message : "");
	nxs_chat_srv_u_labels_variable_add(labels_ctx, "last_issue", "%r", &last_issue_str);

	nxs_string_clone(&message, nxs_chat_srv_u_labels_compile_key(labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_BEGIN));

	nxs_chat_srv_u_labels_variable_clear(labels_ctx);

	if(message_id == 0) {

		/* create new comment */

		nxs_chat_srv_u_tlgrm_sendmessage_add(tlgrm_sendmessage_ctx, chat_id, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_HTML);

		if(issue_id > 0) {

			nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(
			        tlgrm_sendmessage_ctx,
			        0,
			        0,
			        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE,
			        issue_id,
			        nxs_chat_srv_u_labels_compile_key_button(
			                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_ADD_LAST_ACTIVE));
		}

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(
		        tlgrm_sendmessage_ctx,
		        1,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECT_ISSUE,
		        0,
		        nxs_chat_srv_u_labels_compile_key_button(
		                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_SELECT_ISSUE_FROM_LIST));

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(
		        tlgrm_sendmessage_ctx,
		        2,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_NEW_ISSUE,
		        0,
		        nxs_chat_srv_u_labels_compile_key_button(
		                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_CREATE_NEW_ISSUE));

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(
		        tlgrm_sendmessage_ctx,
		        3,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SESSION_DESTROY,
		        0,
		        nxs_chat_srv_u_labels_compile_key_button(
		                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_CLOSE_DIALOG));

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

		if(issue_id > 0) {

			nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
			        tlgrm_editmessagetext_ctx,
			        0,
			        0,
			        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE,
			        issue_id,
			        nxs_chat_srv_u_labels_compile_key_button(
			                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_ADD_LAST_ACTIVE));
		}

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
		        tlgrm_editmessagetext_ctx,
		        1,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECT_ISSUE,
		        0,
		        nxs_chat_srv_u_labels_compile_key_button(
		                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_SELECT_ISSUE_FROM_LIST));

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
		        tlgrm_editmessagetext_ctx,
		        2,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_NEW_ISSUE,
		        0,
		        nxs_chat_srv_u_labels_compile_key_button(
		                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_CREATE_NEW_ISSUE));

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
		        tlgrm_editmessagetext_ctx,
		        3,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SESSION_DESTROY,
		        0,
		        nxs_chat_srv_u_labels_compile_key_button(
		                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_CLOSE_DIALOG));

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
	nxs_string_free(&last_issue_str);
	nxs_string_free(&issue_subject);
	nxs_string_free(&issue_project_name);

	tlgrm_sendmessage_ctx     = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);
	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_free(tlgrm_editmessagetext_ctx);
	last_issue_ctx            = nxs_chat_srv_u_last_issues_free(last_issue_ctx);
	labels_ctx                = nxs_chat_srv_u_labels_free(labels_ctx);

	return rc;
}

/* Module internal (static) functions */
