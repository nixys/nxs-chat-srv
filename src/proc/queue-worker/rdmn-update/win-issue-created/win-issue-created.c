#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/queue-worker/rdmn-update/win-issue-created/win-issue-created.h>
#include <proc/queue-worker/rdmn-update/win-issue-created/ctx/win-issue-created-ctx.h>
#include <proc/queue-worker/rdmn-update/win-issue-created/win-issue-created-subproc.h>

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

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_rdmn_update_win_issue_created_runtime(nxs_chat_srv_m_rdmn_update_t *update,
                                                                                     size_t                        tlgrm_userid)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_string_t message, private_issue, description_fmt, project_fmt, subject_fmt, author_fmt, status_fmt, priority_fmt,
	        assigned_to_fmt;
	nxs_buf_t *                    out_buf;
	nxs_bool_t                     response_status;
	nxs_chat_srv_m_tlgrm_message_t tlgrm_message;
	nxs_chat_srv_u_db_issues_t *   db_issue_ctx;

	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&message);
	nxs_string_init_empty(&private_issue);
	nxs_string_init_empty(&description_fmt);
	nxs_string_init_empty(&project_fmt);
	nxs_string_init_empty(&subject_fmt);
	nxs_string_init_empty(&author_fmt);
	nxs_string_init_empty(&status_fmt);
	nxs_string_init_empty(&priority_fmt);
	nxs_string_init_empty(&assigned_to_fmt);

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();
	db_issue_ctx          = nxs_chat_srv_u_db_issues_init();

	nxs_chat_srv_c_tlgrm_message_init(&tlgrm_message);

	if(update->data.issue.is_private == NXS_YES) {

		nxs_string_printf(&private_issue, NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_CREATED_IS_PRIVATE, _s_private_message);
	}

	nxs_chat_srv_c_tlgrm_format_escape_html(&description_fmt, &update->data.issue.description);
	nxs_chat_srv_c_tlgrm_format_escape_html(&project_fmt, &update->data.issue.project.name);
	nxs_chat_srv_c_tlgrm_format_escape_html(&subject_fmt, &update->data.issue.subject);
	nxs_chat_srv_c_tlgrm_format_escape_html(&author_fmt, &update->data.issue.author.name);
	nxs_chat_srv_c_tlgrm_format_escape_html(&status_fmt, &update->data.issue.status.name);
	nxs_chat_srv_c_tlgrm_format_escape_html(&priority_fmt, &update->data.issue.priority.name);
	nxs_chat_srv_c_tlgrm_format_escape_html(&assigned_to_fmt, &update->data.issue.assigned_to.name);

	nxs_string_printf(&message,
	                  NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_CREATED,
	                  &nxs_chat_srv_cfg.rdmn.host,
	                  update->data.issue.id,
	                  &project_fmt,
	                  update->data.issue.id,
	                  &subject_fmt,
	                  &private_issue,
	                  &author_fmt,
	                  &status_fmt,
	                  &priority_fmt,
	                  &assigned_to_fmt,
	                  &description_fmt);

	/* create new comment */

	nxs_chat_srv_u_tlgrm_sendmessage_add(tlgrm_sendmessage_ctx, tlgrm_userid, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_HTML);

	nxs_chat_srv_u_tlgrm_sendmessage_disable_web_page_preview(tlgrm_sendmessage_ctx);

	if(nxs_chat_srv_u_tlgrm_sendmessage_push(tlgrm_sendmessage_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	out_buf = nxs_chat_srv_u_tlgrm_sendmessage_get_response_buf(tlgrm_sendmessage_ctx);

	if(nxs_chat_srv_c_tlgrm_message_result_pull_json(&tlgrm_message, &response_status, out_buf) == NXS_CHAT_SRV_E_OK) {

		nxs_chat_srv_u_db_issues_set(db_issue_ctx, tlgrm_userid, tlgrm_message.message_id, update->data.issue.id);
	}

error:

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);
	db_issue_ctx          = nxs_chat_srv_u_db_issues_free(db_issue_ctx);

	nxs_chat_srv_c_tlgrm_message_free(&tlgrm_message);

	nxs_string_free(&message);
	nxs_string_free(&private_issue);
	nxs_string_free(&description_fmt);
	nxs_string_free(&project_fmt);
	nxs_string_free(&subject_fmt);
	nxs_string_free(&author_fmt);
	nxs_string_free(&status_fmt);
	nxs_string_free(&priority_fmt);
	nxs_string_free(&assigned_to_fmt);

	return rc;
}

/* Module internal (static) functions */
