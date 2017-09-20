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

static nxs_string_t	_s_msg_empty_subject		= nxs_string(NXS_CHAT_SRV_TLGRM_MESSAGE_EMPTY_SUBJECT);

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_win_select_priority(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                                                nxs_chat_srv_u_db_cache_t *    cache_ctx,
                                                                                size_t                         chat_id,
                                                                                size_t                         message_id,
                                                                                nxs_chat_srv_m_tlgrm_update_t *update,
                                                                                nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_editmessagetext_t *  tlgrm_editmessagetext_ctx;
	nxs_string_t                              callback_str, description, subject, project_name, priority_name, message;
	nxs_buf_t *                               b;
	nxs_chat_srv_err_t                        rc;
	nxs_chat_srv_m_db_cache_issue_priority_t *issue_priority;
	nxs_array_t                               priorities;
	size_t                                    i;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);
	nxs_string_init(&description);
	nxs_string_init(&subject);
	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&message);

	nxs_array_init2(&priorities, nxs_chat_srv_m_db_cache_issue_priority_t);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

	if(message_id == 0) {

		/* create new comment */
	}
	else {

		/* update existing comment */

		nxs_chat_srv_u_db_cache_prio_get(cache_ctx, &priorities);

		nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx, NULL, &project_name, NULL, &priority_name, &subject, &description, NULL);

		nxs_string_printf(&message,
		                  NXS_CHAT_SRV_TLGRM_MESSAGE_ISSUE_FULL,
		                  &project_name,
		                  &priority_name,
		                  nxs_string_len(&subject) > 0 ? &subject : &_s_msg_empty_subject,
		                  &description);

		tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

		nxs_chat_srv_u_tlgrm_editmessagetext_add(
		        tlgrm_editmessagetext_ctx, chat_id, message_id, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

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
	nxs_string_free(&subject);
	nxs_string_free(&project_name);
	nxs_string_free(&priority_name);
	nxs_string_free(&message);

	nxs_array_free(&priorities);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_free(tlgrm_editmessagetext_ctx);

	return rc;
}

/* Module internal (static) functions */
