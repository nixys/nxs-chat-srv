#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/queue-worker/tlgrm-update/win-new-issue/win-new-issue.h>
#include <proc/queue-worker/tlgrm-update/win-new-issue/ctx/win-new-issue-ctx.h>
#include <proc/queue-worker/tlgrm-update/win-new-issue/win-new-issue-subproc.h>

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

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_win_new_issue(nxs_chat_srv_u_db_sess_t * sess_ctx,
                                                                          nxs_chat_srv_m_user_ctx_t *user_ctx,
                                                                          size_t                     chat_id,
                                                                          size_t                     message_id,
                                                                          nxs_bool_t                 full_message,
                                                                          nxs_buf_t *                response_buf)
{
	nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx;
	nxs_chat_srv_u_labels_t *               labels_ctx;
	nxs_string_t callback_str, description, description_preview, subject, project_name, priority_name, priority_label, message,
	        private_msg;
	nxs_buf_t *        b;
	nxs_chat_srv_err_t rc;
	nxs_bool_t         is_private;
	size_t             files_count, project_id;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);
	nxs_string_init(&description);
	nxs_string_init(&description_preview);
	nxs_string_init(&subject);
	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&priority_label);
	nxs_string_init(&message);
	nxs_string_init_empty(&private_msg);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();
	labels_ctx                = nxs_chat_srv_u_labels_init();

	files_count = nxs_chat_srv_u_db_sess_get_files_count(sess_ctx);

	if(message_id == 0) {

		/* create new comment */
	}
	else {

		/* update existing comment */

		nxs_chat_srv_u_db_sess_t_get_new_issue(
		        sess_ctx, &project_id, &project_name, NULL, &priority_name, &subject, &description, &is_private, NULL, NULL);

		if(project_id == 0) {

			/* no projects available for user */

			nxs_chat_srv_u_tlgrm_editmessagetext_add(
			        tlgrm_editmessagetext_ctx,
			        chat_id,
			        message_id,
			        nxs_chat_srv_u_labels_compile_key(
			                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_NEW_ISSUE_EMPTY),
			        NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_HTML);

			nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
			        tlgrm_editmessagetext_ctx,
			        0,
			        0,
			        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_BACK,
			        0,
			        nxs_chat_srv_u_labels_compile_key_button(
			                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_CANCEL));
		}
		else {

			if(is_private == NXS_YES) {

				nxs_string_clone(&private_msg,
				                 nxs_chat_srv_u_labels_compile_key(
				                         labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_PRIVATE_ISSUE));
			}

			if(nxs_string_len(&subject) > 0) {

				nxs_chat_srv_c_tlgrm_format_escape_html(NULL, &subject);
			}
			else {

				nxs_string_clone(&subject,
				                 nxs_chat_srv_u_labels_compile_key(
				                         labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_EMPTY_SUBJECT));
			}

			switch(nxs_chat_srv_c_tlgrm_make_message_preview(&description_preview, &description)) {

				case NXS_CHAT_SRV_E_OK:

					nxs_chat_srv_c_tlgrm_format_escape_html(NULL, &description_preview);

					break;

				case NXS_CHAT_SRV_E_WARN:

					nxs_chat_srv_u_labels_variable_clear(labels_ctx);

					nxs_string_clone(
					        &description_preview,
					        nxs_chat_srv_u_labels_compile_key(labels_ctx,
					                                          &user_ctx->r_userlang,
					                                          NXS_CHAT_SRV_U_LABELS_KEY_MESSAGE_TOO_LARGE_FOR_PREVIEW));

					break;

				default:

					break;
			}

			nxs_chat_srv_c_tlgrm_format_escape_html(NULL, &project_name);
			nxs_chat_srv_c_tlgrm_format_escape_html(NULL, &priority_name);

			nxs_chat_srv_u_labels_variable_clear(labels_ctx);

			/* trying to translate the priority name */
			nxs_string_clone(&priority_label,
			                 nxs_chat_srv_u_labels_compile_key(
			                         labels_ctx, &user_ctx->r_userlang, (char *)nxs_string_str(&priority_name)));

			nxs_chat_srv_u_labels_variable_add(labels_ctx, "project_name", "%r", &project_name);
			nxs_chat_srv_u_labels_variable_add(labels_ctx, "priority_name", "%r", &priority_label);
			nxs_chat_srv_u_labels_variable_add(labels_ctx, "issue_subject", "%r", &subject);
			nxs_chat_srv_u_labels_variable_add(labels_ctx, "private_issue", "%r", &private_msg);
			nxs_chat_srv_u_labels_variable_add(labels_ctx, "issue_description", "%r", &description_preview);
			nxs_chat_srv_u_labels_variable_add(labels_ctx, "number_of_attached_files", "%zu", files_count);

			if(full_message == NXS_YES) {

				nxs_string_clone(&message,
				                 nxs_chat_srv_u_labels_compile_key(
				                         labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_NEW_ISSUE_FULL));
			}
			else {

				nxs_string_clone(&message,
				                 nxs_chat_srv_u_labels_compile_key(
				                         labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_NEW_ISSUE_SHORT));
			}

			nxs_chat_srv_u_labels_variable_clear(labels_ctx);

			nxs_chat_srv_u_tlgrm_editmessagetext_add(
			        tlgrm_editmessagetext_ctx, chat_id, message_id, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_HTML);

			if(full_message == NXS_YES) {

				nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
				        tlgrm_editmessagetext_ctx,
				        0,
				        0,
				        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECT_PROJECT,
				        0,
				        nxs_chat_srv_u_labels_compile_key_button(
				                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_PROJECT));

				nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
				        tlgrm_editmessagetext_ctx,
				        0,
				        1,
				        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECT_PRIORITY,
				        0,
				        nxs_chat_srv_u_labels_compile_key_button(
				                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_PRIORITY));

				/*
				 * TODO: at the moment change description functionality for new issue is disabled
				 */
				/*
				nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
				        tlgrm_editmessagetext_ctx,
				        0,
				        2,
				        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_CHANGE_DESCRIPTION,
				        0,
				        NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_DESCRIPTION);
				*/

				nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
				        tlgrm_editmessagetext_ctx,
				        1,
				        0,
				        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_CREATE_ISSUE,
				        0,
				        nxs_chat_srv_u_labels_compile_key_button(
				                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_CREATE_ISSUE));

				nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
				        tlgrm_editmessagetext_ctx,
				        2,
				        0,
				        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_BACK,
				        0,
				        nxs_chat_srv_u_labels_compile_key_button(
				                labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_CANCEL));
			}
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

	nxs_string_free(&callback_str);
	nxs_string_free(&description);
	nxs_string_free(&description_preview);
	nxs_string_free(&subject);
	nxs_string_free(&project_name);
	nxs_string_free(&priority_name);
	nxs_string_free(&priority_label);
	nxs_string_free(&message);
	nxs_string_free(&private_msg);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_free(tlgrm_editmessagetext_ctx);
	labels_ctx                = nxs_chat_srv_u_labels_free(labels_ctx);

	return rc;
}

/* Module internal (static) functions */
