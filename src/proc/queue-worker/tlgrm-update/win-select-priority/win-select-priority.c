#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/queue-worker/tlgrm-update/win-select-priority/win-select-priority.h>
#include <proc/queue-worker/tlgrm-update/win-select-priority/ctx/win-select-priority-ctx.h>
#include <proc/queue-worker/tlgrm-update/win-select-priority/win-select-priority-subproc.h>

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

static u_char		_s_private_message[]	= {NXS_CHAT_SRV_UTF8_PRIVATE_MESSAGE};

static nxs_string_t	_s_msg_empty_subject	= nxs_string(NXS_CHAT_SRV_TLGRM_MESSAGE_EMPTY_SUBJECT);
static nxs_string_t	_s_msg_empty_project	= nxs_string(NXS_CHAT_SRV_TLGRM_MESSAGE_NEW_ISSUE_EMPTY);

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_win_select_priority(nxs_chat_srv_u_db_sess_t * sess_ctx,
                                                                                nxs_chat_srv_u_db_cache_t *cache_ctx,
                                                                                size_t                     chat_id,
                                                                                size_t                     message_id,
                                                                                nxs_buf_t *                response_buf)
{
	nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx;
	nxs_string_t       callback_str, description, description_preview, subject, project_name, priority_name, message, private_msg;
	nxs_buf_t *        b;
	nxs_chat_srv_err_t rc;
	nxs_chat_srv_m_db_cache_issue_priority_t *issue_priority;
	nxs_array_t                               priorities;
	nxs_bool_t                                is_private;
	size_t                                    i, files_count, project_id;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);
	nxs_string_init(&description);
	nxs_string_init(&description_preview);
	nxs_string_init(&subject);
	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&message);
	nxs_string_init_empty(&private_msg);

	nxs_array_init2(&priorities, nxs_chat_srv_m_db_cache_issue_priority_t);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

	files_count = nxs_chat_srv_u_db_sess_get_files_count(sess_ctx);

	if(message_id == 0) {

		/* create new comment */
	}
	else {

		/* update existing comment */

		nxs_chat_srv_u_db_cache_prio_get(cache_ctx, &priorities);

		nxs_chat_srv_u_db_sess_t_get_new_issue(
		        sess_ctx, &project_id, &project_name, NULL, &priority_name, &subject, &description, &is_private, NULL, NULL);

		if(project_id == 0) {

			/* no projects available for user */

			nxs_chat_srv_u_tlgrm_editmessagetext_add(tlgrm_editmessagetext_ctx,
			                                         chat_id,
			                                         message_id,
			                                         &_s_msg_empty_project,
			                                         NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_HTML);

			nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(tlgrm_editmessagetext_ctx,
			                                                                   0,
			                                                                   0,
			                                                                   NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_BACK,
			                                                                   0,
			                                                                   NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_CANCEL);
		}
		else {

			if(is_private == NXS_YES) {

				nxs_string_printf(&private_msg, NXS_CHAT_SRV_TLGRM_MESSAGE_ISSUE_PRIVACY, _s_private_message);
			}

			nxs_chat_srv_c_tlgrm_format_escape_html(NULL, &project_name);
			nxs_chat_srv_c_tlgrm_format_escape_html(NULL, &priority_name);
			nxs_chat_srv_c_tlgrm_format_escape_html(NULL, &subject);

			if(nxs_chat_srv_c_tlgrm_make_message_preview(&description_preview, &description) == NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_c_tlgrm_format_escape_html(NULL, &description_preview);
			}

			nxs_string_printf(&message,
			                  NXS_CHAT_SRV_TLGRM_MESSAGE_NEW_ISSUE_FULL,
			                  &project_name,
			                  &priority_name,
			                  nxs_string_len(&subject) > 0 ? &subject : &_s_msg_empty_subject,
			                  &private_msg,
			                  &description_preview,
			                  files_count);

			nxs_chat_srv_u_tlgrm_editmessagetext_add(
			        tlgrm_editmessagetext_ctx, chat_id, message_id, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_HTML);

			for(i = 0; i < nxs_array_count(&priorities); i++) {

				issue_priority = nxs_array_get(&priorities, i);

				nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
				        tlgrm_editmessagetext_ctx,
				        i,
				        0,
				        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECTED_PRIORITY,
				        issue_priority->id,
				        nxs_string_str(issue_priority->name));
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
	nxs_string_free(&message);
	nxs_string_free(&private_msg);

	nxs_array_free(&priorities);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_free(tlgrm_editmessagetext_ctx);

	return rc;
}

/* Module internal (static) functions */
