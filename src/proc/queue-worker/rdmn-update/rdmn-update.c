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
	nxs_chat_srv_err_t	(*handler)(nxs_chat_srv_m_rdmn_update_t *update, void *p_ctx);
} handler_update_t;

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_err_t handler_update_issue_create(nxs_chat_srv_m_rdmn_update_t *update, void *p_meta);
static nxs_chat_srv_err_t handler_update_issue_edit(nxs_chat_srv_m_rdmn_update_t *update, void *p_meta);

static void receivers_add(nxs_array_t *                  receivers,
                          size_t                         author_id,
                          size_t                         user_id,
                          nxs_array_t *                  members,
                          nxs_chat_srv_m_rdmn_journal_t *journal);

static void statistic_add(nxs_chat_srv_u_db_statistic_action_type_t action_type, size_t rdmn_userid);

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

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_rdmn_update_runtime(nxs_chat_srv_m_ra_queue_type_t type, nxs_string_t *data, void *p_meta)
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

		nxs_error(rc, handlers_update[i].handler(&update, p_meta), error);
	}

error:

	nxs_string_free(&base64_decoded);

	nxs_chat_srv_c_rdmn_update_free(&update);

	return rc;
}

/* Module internal (static) functions */

static nxs_chat_srv_err_t handler_update_issue_create(nxs_chat_srv_m_rdmn_update_t *update, void *p_meta)
{
	nxs_chat_srv_u_db_ids_t *           ids_ctx;
	nxs_chat_srv_m_rdmn_user_t *        u;
	nxs_chat_srv_m_user_ctx_t           user_ctx;
	nxs_chat_srv_u_last_issues_t *      last_issue_ctx;
	nxs_chat_srv_u_rdmn_attachments_t * rdmn_attachments_ctx;
	nxs_chat_srv_u_tlgrm_attachments_t *tlgrm_attachments_ctx;
	nxs_chat_srv_u_db_cache_t *         cache_ctx;
	nxs_chat_srv_err_t                  rc;
	nxs_array_t                         receivers;
	size_t                              i, tlgrm_userid, *id;

	rc = NXS_CHAT_SRV_E_OK;

	ids_ctx               = nxs_chat_srv_u_db_ids_init();
	last_issue_ctx        = nxs_chat_srv_u_last_issues_init();
	rdmn_attachments_ctx  = nxs_chat_srv_u_rdmn_attachments_init();
	tlgrm_attachments_ctx = nxs_chat_srv_u_tlgrm_attachments_init();
	cache_ctx             = nxs_chat_srv_u_db_cache_init();

	nxs_chat_srv_c_user_ctx_init(&user_ctx);

	nxs_array_init2(&receivers, size_t);

	if(nxs_chat_srv_u_db_cache_pull(cache_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	statistic_add(NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_RDMN_ISSUE_CREATE, update->data.issue.author.id);

	/* add to receivers array assigned to user */
	receivers_add(
	        &receivers, update->data.issue.author.id, update->data.issue.assigned_to.id, &update->data.issue.project.members, NULL);

	/* add to receivers array all watchers excluding author of the issue */
	for(i = 0; i < nxs_array_count(&update->data.issue.watchers); i++) {

		u = nxs_array_get(&update->data.issue.watchers, i);

		receivers_add(&receivers, update->data.issue.author.id, u->id, &update->data.issue.project.members, NULL);
	}

	/* add to receivers array all cf_watchers excluding author of the issue */
	for(i = 0; i < nxs_array_count(&update->data.issue.cf_watchers); i++) {

		id = nxs_array_get(&update->data.issue.cf_watchers, i);

		receivers_add(&receivers, update->data.issue.author.id, *id, &update->data.issue.project.members, NULL);
	}

	for(i = 0; i < nxs_array_count(&receivers); i++) {

		id = nxs_array_get(&receivers, i);

		if(nxs_chat_srv_u_db_cache_user_get_by_rdmn_id(cache_ctx, *id, &user_ctx) != NXS_CHAT_SRV_E_OK) {

			/* if user not found by rdmn user id */

			continue;
		}

		if(nxs_string_len(&user_ctx.t_username) == 0) {

			/* if user has empty 'telegram' field in Redmine account settings */

			continue;
		}

		if(nxs_chat_srv_u_db_ids_get(ids_ctx, *id, &tlgrm_userid) != NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		if(tlgrm_userid > 0) {

			nxs_log_write_debug(&process,
			                    "[%s]: sending rdmn update to watcher (type: issue_create, issue id: %d, rdmn user: \"%r %r\", "
			                    "rdmn user_id: %zu, "
			                    "tlgrm user_id: %zu)",
			                    nxs_proc_get_name(&process),
			                    update->data.issue.id,
			                    &user_ctx.r_userlname,
			                    &user_ctx.r_userfname,
			                    *id,
			                    tlgrm_userid);

			/* set issue 'update->data.issue.id' as last for telegram user 'tlgrm_userid' */
			nxs_chat_srv_u_last_issues_set(last_issue_ctx, tlgrm_userid, update->data.issue.id);

			if(nxs_chat_srv_p_queue_worker_rdmn_update_win_issue_created_runtime(
			           update, tlgrm_userid, rdmn_attachments_ctx, tlgrm_attachments_ctx) != NXS_CHAT_SRV_E_OK) {

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

	ids_ctx               = nxs_chat_srv_u_db_ids_free(ids_ctx);
	last_issue_ctx        = nxs_chat_srv_u_last_issues_free(last_issue_ctx);
	rdmn_attachments_ctx  = nxs_chat_srv_u_rdmn_attachments_free(rdmn_attachments_ctx);
	tlgrm_attachments_ctx = nxs_chat_srv_u_tlgrm_attachments_free(tlgrm_attachments_ctx);
	cache_ctx             = nxs_chat_srv_u_db_cache_free(cache_ctx);

	nxs_chat_srv_c_user_ctx_free(&user_ctx);

	nxs_array_free(&receivers);

	return rc;
}

static nxs_chat_srv_err_t handler_update_issue_edit(nxs_chat_srv_m_rdmn_update_t *update, void *p_meta)
{
	nxs_chat_srv_u_db_ids_t *           ids_ctx;
	nxs_chat_srv_u_rdmn_attachments_t * rdmn_attachments_ctx;
	nxs_chat_srv_u_tlgrm_attachments_t *tlgrm_attachments_ctx;
	nxs_chat_srv_u_db_cache_t *         cache_ctx;
	nxs_chat_srv_m_rdmn_user_t *        u;
	nxs_chat_srv_m_rdmn_journal_t *     journal;
	nxs_chat_srv_m_user_ctx_t           user_ctx;
	nxs_chat_srv_u_last_issues_t *      last_issue_ctx;
	nxs_chat_srv_u_presale_t *          presale_ctx;
	nxs_chat_srv_m_proc_queue_worker_t *p = p_meta;
	nxs_chat_srv_err_t                  rc;
	nxs_array_t                         receivers;
	size_t                              i, tlgrm_userid, *id, tlgrm_presale_userid;

	if((journal = nxs_array_get(&update->data.issue.journals, 0)) == NULL) {

		nxs_log_write_warn(&process,
		                   "[%s]: rdmn issue edit error: empty journal (issue id: %d)",
		                   nxs_proc_get_name(&process),
		                   update->data.issue.id);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_array_init2(&receivers, size_t);

	ids_ctx               = nxs_chat_srv_u_db_ids_init();
	last_issue_ctx        = nxs_chat_srv_u_last_issues_init();
	rdmn_attachments_ctx  = nxs_chat_srv_u_rdmn_attachments_init();
	tlgrm_attachments_ctx = nxs_chat_srv_u_tlgrm_attachments_init();
	presale_ctx           = nxs_chat_srv_u_presale_init();
	cache_ctx             = nxs_chat_srv_u_db_cache_init();

	nxs_chat_srv_c_user_ctx_init(&user_ctx);

	if(nxs_chat_srv_u_presale_pull(presale_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: error to get presale context while sending tlgrm message to user (issue id: %d)",
		                    nxs_proc_get_name(&process),
		                    update->data.issue.id);
	}

	if(nxs_chat_srv_u_db_cache_pull(cache_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	statistic_add(NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_RDMN_ISSUE_UPDATE, journal->user.id);

	/* add to receivers array author of issue */
	receivers_add(&receivers, journal->user.id, update->data.issue.author.id, &update->data.issue.project.members, journal);

	/* add to receivers array 'assigned to' user */
	receivers_add(&receivers, journal->user.id, update->data.issue.assigned_to.id, &update->data.issue.project.members, journal);

	/* add to receivers array all watchers excluding author of the comment */
	for(i = 0; i < nxs_array_count(&update->data.issue.watchers); i++) {

		u = nxs_array_get(&update->data.issue.watchers, i);

		receivers_add(&receivers, journal->user.id, u->id, &update->data.issue.project.members, journal);
	}

	/* add to receivers array all cf_watchers excluding author of the comment */
	for(i = 0; i < nxs_array_count(&update->data.issue.cf_watchers); i++) {

		id = nxs_array_get(&update->data.issue.cf_watchers, i);

		receivers_add(&receivers, journal->user.id, *id, &update->data.issue.project.members, journal);
	}

	/* send message to all receivers */
	for(i = 0; i < nxs_array_count(&receivers); i++) {

		id = nxs_array_get(&receivers, i);

		if(nxs_chat_srv_u_db_cache_user_get_by_rdmn_id(cache_ctx, *id, &user_ctx) != NXS_CHAT_SRV_E_OK) {

			/* if user not found by rdmn user id */

			continue;
		}

		if(nxs_string_len(&user_ctx.t_username) == 0) {

			/* if user has empty 'telegram' field in Redmine account settings */

			continue;
		}

		if(nxs_chat_srv_u_db_ids_get(ids_ctx, *id, &tlgrm_userid) != NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		if(tlgrm_userid > 0) {

			nxs_log_write_debug(&process,
			                    "[%s]: sending rdmn update to watcher (type: issue_edit, issue id: %d, rdmn user: \"%r %r\", "
			                    "rdmn user_id: %zu, "
			                    "tlgrm user_id: %zu)",
			                    nxs_proc_get_name(&process),
			                    update->data.issue.id,
			                    &user_ctx.r_userlname,
			                    &user_ctx.r_userfname,
			                    *id,
			                    tlgrm_userid);

			/* set issue 'update->data.issue.id' as last for telegram user 'tlgrm_userid' */
			nxs_chat_srv_u_last_issues_set(last_issue_ctx, tlgrm_userid, update->data.issue.id);

			if(nxs_chat_srv_p_queue_worker_rdmn_update_win_issue_updated_runtime(
			           update, tlgrm_userid, journal, rdmn_attachments_ctx, tlgrm_attachments_ctx, NXS_NO) !=
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

	if(nxs_chat_srv_u_presale_p_get_by_issue_id(presale_ctx, update->data.issue.id, &tlgrm_presale_userid) == NXS_CHAT_SRV_E_OK &&
	   tlgrm_presale_userid > 0 && journal->user.id != p->presale_rdmn_user_id) {

		if(journal->private_notes == NXS_NO) {

			/* sending only not private notes */

			if(nxs_chat_srv_p_queue_worker_rdmn_update_win_issue_updated_runtime(
			           update, tlgrm_presale_userid, journal, rdmn_attachments_ctx, tlgrm_attachments_ctx, NXS_YES) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_log_write_error(&process,
				                    "[%s]: error while sending presale tlgrm message to user (issue id: %d, "
				                    "tlgrm user id: %zu)",
				                    nxs_proc_get_name(&process),
				                    update->data.issue.id,
				                    tlgrm_presale_userid);
			}
		}
	}

error:

	ids_ctx               = nxs_chat_srv_u_db_ids_free(ids_ctx);
	last_issue_ctx        = nxs_chat_srv_u_last_issues_free(last_issue_ctx);
	rdmn_attachments_ctx  = nxs_chat_srv_u_rdmn_attachments_free(rdmn_attachments_ctx);
	tlgrm_attachments_ctx = nxs_chat_srv_u_tlgrm_attachments_free(tlgrm_attachments_ctx);
	presale_ctx           = nxs_chat_srv_u_presale_free(presale_ctx);
	cache_ctx             = nxs_chat_srv_u_db_cache_free(cache_ctx);

	nxs_chat_srv_c_user_ctx_free(&user_ctx);

	nxs_array_free(&receivers);

	return rc;
}

static void receivers_add(nxs_array_t *                  receivers,
                          size_t                         author_id,
                          size_t                         user_id,
                          nxs_array_t *                  members,
                          nxs_chat_srv_m_rdmn_journal_t *journal)
{
	nxs_chat_srv_m_rdmn_member_t *m;
	size_t *                      id, i;

	if(user_id == 0) {

		return;
	}

	/* skip adding into array author of changes */
	if(user_id == author_id) {

		return;
	}

	/* avoid elements duplicates */
	for(i = 0; i < nxs_array_count(receivers); i++) {

		id = nxs_array_get(receivers, i);

		if(*id == user_id) {

			return;
		}
	}

	/* check view changes permissions */
	for(i = 0; i < nxs_array_count(members); i++) {

		m = nxs_array_get(members, i);

		/* found member */
		if(m->id == user_id) {

			/* user allowed to see this issue */
			if(m->access.view_current_issue == NXS_YES) {

				if(journal == NULL) {

					/* issue without journal (new issue) */

					id = nxs_array_add(receivers);

					*id = user_id;

					return;
				}
				else {

					if(journal->private_notes == NXS_NO || m->access.view_private_notes == NXS_YES) {

						/* not a private note or user can see private notes */

						id = nxs_array_add(receivers);

						*id = user_id;

						return;
					}
				}
			}

			return;
		}
	}
}

static void statistic_add(nxs_chat_srv_u_db_statistic_action_type_t action_type, size_t rdmn_userid)
{
	nxs_chat_srv_u_db_statistic_t *statistic_ctx;

	statistic_ctx = nxs_chat_srv_u_db_statistic_init();

	if(nxs_chat_srv_u_db_statistic_add(statistic_ctx, action_type, rdmn_userid) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: can't save user action statistic (action id: %d, rdmn user id: %zu)",
		                   nxs_proc_get_name(&process),
		                   action_type,
		                   rdmn_userid);
	}

	statistic_ctx = nxs_chat_srv_u_db_statistic_free(statistic_ctx);
}
