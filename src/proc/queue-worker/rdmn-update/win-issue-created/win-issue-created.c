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



/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_rdmn_update_win_issue_created_runtime(nxs_chat_srv_m_rdmn_update_t *update,
                                                                                     size_t                        tlgrm_userid)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_string_t                        message;

	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&message);

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();

	nxs_string_printf(&message,
	                  NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_CREATED,
	                  update->data.issue.id,
	                  &update->data.issue.subject,
	                  &nxs_chat_srv_cfg.rdmn.host,
	                  update->data.issue.id,
	                  &update->data.issue.author.name,
	                  &update->data.issue.status.name,
	                  &update->data.issue.priority.name,
	                  &update->data.issue.assigned_to.name,
	                  &update->data.issue.description);

	/* create new comment */

	nxs_chat_srv_u_tlgrm_sendmessage_add(tlgrm_sendmessage_ctx, tlgrm_userid, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

	// nxs_chat_srv_u_tlgrm_sendmessage_force_reply_set(tlgrm_sendmessage_ctx);
	nxs_chat_srv_u_tlgrm_sendmessage_disable_web_page_preview(tlgrm_sendmessage_ctx);

	if(nxs_chat_srv_u_tlgrm_sendmessage_push(tlgrm_sendmessage_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);

	nxs_string_free(&message);

	return rc;
}

/* Module internal (static) functions */
