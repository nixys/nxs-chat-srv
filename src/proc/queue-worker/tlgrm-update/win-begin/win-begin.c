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

static u_char		_s_private_message[]	= {NXS_CHAT_SRV_UTF8_PRIVATE_MESSAGE};

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_win_begin(size_t        chat_id,
                                                                      size_t        message_id,
                                                                      size_t        rdmn_userid,
                                                                      nxs_string_t *user_api_key,
                                                                      nxs_bool_t    private_notes,
                                                                      size_t        files_count,
                                                                      nxs_buf_t *   response_buf)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *    tlgrm_sendmessage_ctx;
	nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx;
	nxs_chat_srv_u_last_issues_t *          last_issue_ctx;
	nxs_string_t                            callback_str, message, issue_subject, issue_project_name, private_message;
	nxs_buf_t *                             b;
	nxs_chat_srv_err_t                      rc;
	size_t                                  issue_id;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);
	nxs_string_init(&message);
	nxs_string_init_empty(&issue_subject);
	nxs_string_init_empty(&issue_project_name);
	nxs_string_init_empty(&private_message);

	tlgrm_sendmessage_ctx     = nxs_chat_srv_u_tlgrm_sendmessage_init();
	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();
	last_issue_ctx            = nxs_chat_srv_u_last_issues_init();

	if(nxs_chat_srv_u_last_issues_get(
	           last_issue_ctx, chat_id, rdmn_userid, user_api_key, &issue_id, &issue_subject, &issue_project_name) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_chat_srv_c_tlgrm_format_escape_html(NULL, &issue_subject);
	nxs_chat_srv_c_tlgrm_format_escape_html(NULL, &issue_project_name);

	if(private_notes == NXS_YES) {

		nxs_string_printf(&private_message, "\n%s", _s_private_message);
	}

	nxs_string_printf(&message,
	                  NXS_CHAT_SRV_TLGRM_MESSAGE_BEGIN,
	                  &private_message,
	                  files_count,
	                  &nxs_chat_srv_cfg.rdmn.host,
	                  issue_id,
	                  &issue_project_name,
	                  issue_id,
	                  &issue_subject);

	if(message_id == 0) {

		/* create new comment */

		nxs_chat_srv_u_tlgrm_sendmessage_add(tlgrm_sendmessage_ctx, chat_id, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_HTML);

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(tlgrm_sendmessage_ctx,
		                                                               0,
		                                                               0,
		                                                               NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE,
		                                                               issue_id,
		                                                               NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_ADD_LAST_ACTIVE);

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(tlgrm_sendmessage_ctx,
		                                                               1,
		                                                               0,
		                                                               NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECT_ISSUE,
		                                                               0,
		                                                               NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_SELECT_ISSUE);

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(tlgrm_sendmessage_ctx,
		                                                               2,
		                                                               0,
		                                                               NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_NEW_ISSUE,
		                                                               0,
		                                                               NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_NEW_ISSUE);

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(tlgrm_sendmessage_ctx,
		                                                               3,
		                                                               0,
		                                                               NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SESSION_DESTROY,
		                                                               0,
		                                                               NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_DESTROY_SESSION);

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

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(tlgrm_editmessagetext_ctx,
		                                                                   0,
		                                                                   0,
		                                                                   NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE,
		                                                                   issue_id,
		                                                                   NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_ADD_LAST_ACTIVE);

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(tlgrm_editmessagetext_ctx,
		                                                                   1,
		                                                                   0,
		                                                                   NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECT_ISSUE,
		                                                                   0,
		                                                                   NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_SELECT_ISSUE);

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(tlgrm_editmessagetext_ctx,
		                                                                   2,
		                                                                   0,
		                                                                   NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_NEW_ISSUE,
		                                                                   0,
		                                                                   NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_NEW_ISSUE);

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(tlgrm_editmessagetext_ctx,
		                                                                   3,
		                                                                   0,
		                                                                   NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SESSION_DESTROY,
		                                                                   0,
		                                                                   NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_DESTROY_SESSION);

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
	nxs_string_free(&issue_project_name);
	nxs_string_free(&private_message);

	tlgrm_sendmessage_ctx     = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);
	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_free(tlgrm_editmessagetext_ctx);
	last_issue_ctx            = nxs_chat_srv_u_last_issues_free(last_issue_ctx);

	return rc;
}

/* Module internal (static) functions */
