#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/queue-worker/rdmn-update/rdmn-update.h>
#include <proc/queue-worker/rdmn-update/ctx/rdmn-update-ctx.h>
#include <proc/queue-worker/rdmn-update/rdmn-update-subproc.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	nxs_string_t		action;
	nxs_chat_srv_err_t	(*handler)(nxs_chat_srv_m_rdmn_update_t *update);
} handler_update_t;

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_err_t handler_update_issue_create(nxs_chat_srv_m_rdmn_update_t *update);
static nxs_chat_srv_err_t handler_update_issue_edit(nxs_chat_srv_m_rdmn_update_t *update);

// clang-format off

/* Module initializations */

handler_update_t handlers_update[] =
{
	{nxs_string("issue_create"),		&handler_update_issue_create},
	{nxs_string("issue_edit"),		&handler_update_issue_edit},

	{NXS_STRING_NULL_STR,	NULL}
};

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_rdmn_update_runtime(nxs_chat_srv_m_queue_com_types_t type, nxs_string_t *data)
{
	nxs_string_t                 base64_decoded;
	nxs_chat_srv_m_rdmn_update_t update;
	nxs_chat_srv_err_t           rc;
	size_t                       i;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&base64_decoded);

	nxs_chat_srv_c_rdmn_update_init(&update);

	nxs_base64_decode_string(&base64_decoded, data);

	if(nxs_chat_srv_c_rdmn_update_pull_json(&update, (nxs_buf_t *)&base64_decoded) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	for(i = 0; nxs_string_len(&handlers_update[i].action) > 0; i++) {

		if(nxs_string_cmp(&handlers_update[i].action, 0, &update.action, 0) == NXS_YES) {

			break;
		}
	}

	if(handlers_update[i].handler != NULL) {

		nxs_error(rc, handlers_update[i].handler(&update), error);
	}

error:

	nxs_string_free(&base64_decoded);

	nxs_chat_srv_c_rdmn_update_free(&update);

	return rc;
}

/* Module internal (static) functions */

static nxs_chat_srv_err_t handler_update_issue_create(nxs_chat_srv_m_rdmn_update_t *update)
{
	nxs_chat_srv_u_db_ids_t *     ids_ctx;
	nxs_chat_srv_m_rdmn_user_t *  u;
	nxs_chat_srv_u_last_issues_t *last_issue_ctx;
	nxs_chat_srv_err_t            rc;
	nxs_array_t                   receivers;
	size_t                        i, tlgrm_userid, *id;

	rc = NXS_CHAT_SRV_E_OK;

	ids_ctx        = nxs_chat_srv_u_db_ids_init();
	last_issue_ctx = nxs_chat_srv_u_last_issues_init();

	nxs_array_init2(&receivers, size_t);

	/* add to receivers array assigned to user */
	if(update->data.issue.author.id != update->data.issue.assigned_to.id) {

		id = nxs_array_add(&receivers);

		*id = update->data.issue.assigned_to.id;
	}

	/* add to receivers array all watchers excluding author of the comment */
	for(i = 0; i < nxs_array_count(&update->data.issue.watchers); i++) {

		u = nxs_array_get(&update->data.issue.watchers, i);

		/* not send message to author of the issue */
		if(u->id == update->data.issue.author.id) {

			continue;
		}

		/* excluding assigned to user, it will add to array above */
		if(u->id == update->data.issue.assigned_to.id) {

			continue;
		}

		id = nxs_array_add(&receivers);

		*id = u->id;
	}

	for(i = 0; i < nxs_array_count(&receivers); i++) {

		id = nxs_array_get(&receivers, i);

		if(nxs_chat_srv_u_db_ids_get(ids_ctx, *id, &tlgrm_userid) != NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		if(tlgrm_userid > 0) {

			/* set issue 'update->data.issue.id' as last for telegram user 'tlgrm_userid' */
			nxs_chat_srv_u_last_issues_set(last_issue_ctx, tlgrm_userid, update->data.issue.id);

			if(nxs_chat_srv_p_queue_worker_rdmn_update_win_issue_created_runtime(update, tlgrm_userid) != NXS_CHAT_SRV_E_OK) {

				nxs_log_write_error(&process,
				                    "[%s]: error while sending tlgrm message to user (issue id: %d, "
				                    "tlgrm user id: %zu)",
				                    nxs_proc_get_name(&process),
				                    update->data.issue.id,
				                    tlgrm_userid);
			}
		}
	}

error:

	ids_ctx        = nxs_chat_srv_u_db_ids_free(ids_ctx);
	last_issue_ctx = nxs_chat_srv_u_last_issues_free(last_issue_ctx);

	nxs_array_free(&receivers);

	return rc;
}

static nxs_chat_srv_err_t handler_update_issue_edit(nxs_chat_srv_m_rdmn_update_t *update)
{
	nxs_chat_srv_u_db_ids_t *      ids_ctx;
	nxs_chat_srv_m_rdmn_user_t *   u;
	nxs_chat_srv_m_rdmn_journal_t *journal;
	nxs_chat_srv_u_last_issues_t * last_issue_ctx;
	nxs_chat_srv_err_t             rc;
	nxs_array_t                    receivers;
	size_t                         i, tlgrm_userid, *id;

	if((journal = nxs_array_get(&update->data.issue.journals, 0)) == NULL) {

		nxs_log_write_warn(&process,
		                   "[%s]: rdmn issue edit error: empty journal (issue id: %d)",
		                   nxs_proc_get_name(&process),
		                   update->data.issue.id);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_array_init2(&receivers, size_t);

	ids_ctx        = nxs_chat_srv_u_db_ids_init();
	last_issue_ctx = nxs_chat_srv_u_last_issues_init();

	/* add to receivers array assigned to user */
	if(journal->user.id != update->data.issue.assigned_to.id) {

		id = nxs_array_add(&receivers);

		*id = update->data.issue.assigned_to.id;
	}

	/* add to receivers array all watchers excluding author of the comment */
	for(i = 0; i < nxs_array_count(&update->data.issue.watchers); i++) {

		u = nxs_array_get(&update->data.issue.watchers, i);

		/* not send message to author of the comment */
		if(u->id == journal->user.id) {

			continue;
		}

		/* excluding assigned to user, it will add to array above */
		if(u->id == update->data.issue.assigned_to.id) {

			continue;
		}

		id = nxs_array_add(&receivers);

		*id = u->id;
	}

	/* send message to all receivers */
	for(i = 0; i < nxs_array_count(&receivers); i++) {

		id = nxs_array_get(&receivers, i);

		if(nxs_chat_srv_u_db_ids_get(ids_ctx, *id, &tlgrm_userid) != NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		if(tlgrm_userid > 0) {

			/* set issue 'update->data.issue.id' as last for telegram user 'tlgrm_userid' */
			nxs_chat_srv_u_last_issues_set(last_issue_ctx, tlgrm_userid, update->data.issue.id);

			if(nxs_chat_srv_p_queue_worker_rdmn_update_win_issue_updated_runtime(update, tlgrm_userid, journal) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_log_write_error(&process,
				                    "[%s]: error while sending tlgrm message to user (issue id: %d, "
				                    "tlgrm user id: %zu)",
				                    nxs_proc_get_name(&process),
				                    update->data.issue.id,
				                    tlgrm_userid);
			}
		}
	}

error:

	ids_ctx        = nxs_chat_srv_u_db_ids_free(ids_ctx);
	last_issue_ctx = nxs_chat_srv_u_last_issues_free(last_issue_ctx);

	nxs_array_free(&receivers);

	return rc;
}
