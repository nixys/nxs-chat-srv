#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/queue-worker/tlgrm-update/tlgrm-update.h>
#include <proc/queue-worker/tlgrm-update/ctx/tlgrm-update-ctx.h>
#include <proc/queue-worker/tlgrm-update/tlgrm-update-subproc.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	nxs_chat_srv_m_db_sess_type_t		type;
	nxs_chat_srv_err_t			(*handler)(nxs_chat_srv_m_tlgrm_bttn_callback_t *callback, nxs_chat_srv_m_tlgrm_update_t *update, nxs_chat_srv_u_db_sess_t *sess_ctx, nxs_chat_srv_u_db_cache_t *cache_ctx, nxs_chat_srv_m_user_ctx_t *user_ctx);
} handlers_callback_t;

typedef struct
{
	nxs_chat_srv_m_db_sess_type_t		type;
	nxs_chat_srv_err_t			(*handler)(nxs_chat_srv_m_tlgrm_update_t *update, nxs_chat_srv_u_db_sess_t *sess_ctx, nxs_chat_srv_u_db_cache_t *cache_ctx, nxs_chat_srv_m_user_ctx_t *user_ctx);
} handlers_message_t;

/* Module internal (static) functions prototypes */

// clang-format on

static size_t check_user(nxs_chat_srv_u_db_cache_t *cache_ctx, nxs_chat_srv_m_user_ctx_t *user_ctx, nxs_chat_srv_m_tlgrm_update_t *update);

static nxs_chat_srv_err_t handler_callback_exec(nxs_chat_srv_m_tlgrm_update_t *update,
                                                nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                nxs_chat_srv_u_db_cache_t *    cache_ctx,
                                                nxs_chat_srv_m_user_ctx_t *    user_ctx);

static nxs_chat_srv_err_t handler_message_exec(nxs_chat_srv_m_tlgrm_update_t *update,
                                               nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                               nxs_chat_srv_u_db_cache_t *    cache_ctx,
                                               nxs_chat_srv_m_user_ctx_t *    user_ctx);

static nxs_chat_srv_err_t handler_callback_sess_type_message(nxs_chat_srv_m_tlgrm_bttn_callback_t *callback,
                                                             nxs_chat_srv_m_tlgrm_update_t *       update,
                                                             nxs_chat_srv_u_db_sess_t *            sess_ctx,
                                                             nxs_chat_srv_u_db_cache_t *           cache_ctx,
                                                             nxs_chat_srv_m_user_ctx_t *           user_ctx);
static nxs_chat_srv_err_t handler_callback_sess_type_new_issue(nxs_chat_srv_m_tlgrm_bttn_callback_t *callback,
                                                               nxs_chat_srv_m_tlgrm_update_t *       update,
                                                               nxs_chat_srv_u_db_sess_t *            sess_ctx,
                                                               nxs_chat_srv_u_db_cache_t *           cache_ctx,
                                                               nxs_chat_srv_m_user_ctx_t *           user_ctx);

static nxs_chat_srv_err_t handler_message_sess_type_empty(nxs_chat_srv_m_tlgrm_update_t *update,
                                                          nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                          nxs_chat_srv_u_db_cache_t *    cache_ctx,
                                                          nxs_chat_srv_m_user_ctx_t *    user_ctx);

static nxs_chat_srv_err_t handler_message_sess_type_message(nxs_chat_srv_m_tlgrm_update_t *update,
                                                            nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                            nxs_chat_srv_u_db_cache_t *    cache_ctx,
                                                            nxs_chat_srv_m_user_ctx_t *    user_ctx);
static nxs_chat_srv_err_t handler_message_sess_type_new_issue(nxs_chat_srv_m_tlgrm_update_t *update,
                                                              nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                              nxs_chat_srv_u_db_cache_t *    cache_ctx,
                                                              nxs_chat_srv_m_user_ctx_t *    user_ctx);

static nxs_chat_srv_err_t
        handler_message_exec_reply(nxs_chat_srv_m_tlgrm_update_t *update, nxs_chat_srv_m_user_ctx_t *user_ctx, size_t tlgrm_userid);

// clang-format off

/* Module initializations */

static handlers_callback_t handlers_callback[] =
{
	{NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE,		&handler_callback_sess_type_message},
	{NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE,		&handler_callback_sess_type_new_issue},

	{NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE,		NULL}
};

static handlers_message_t handlers_message[] =
{
	{NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE,		&handler_message_sess_type_message},
	{NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE,		&handler_message_sess_type_new_issue},

	{NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE,		NULL}
};

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_runtime(nxs_chat_srv_m_queue_com_types_t type, nxs_string_t *data)
{
	nxs_string_t                  base64_decoded;
	nxs_chat_srv_m_tlgrm_update_t update;
	nxs_chat_srv_u_db_sess_t *    sess_ctx;
	nxs_chat_srv_u_db_cache_t *   cache_ctx;
	nxs_chat_srv_u_db_ids_t *     ids_ctx;
	nxs_chat_srv_m_user_ctx_t     user_ctx;
	nxs_chat_srv_err_t            rc;
	size_t                        tlgrm_user_id;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&base64_decoded);

	nxs_chat_srv_c_tlgrm_update_init(&update);

	sess_ctx  = nxs_chat_srv_u_db_sess_init();
	cache_ctx = nxs_chat_srv_u_db_cache_init();
	ids_ctx   = nxs_chat_srv_u_db_ids_init();

	nxs_chat_srv_c_user_ctx_init(&user_ctx);

	nxs_base64_decode_string(&base64_decoded, data);

	if(nxs_chat_srv_c_tlgrm_update_pull_json(&update, (nxs_buf_t *)&base64_decoded) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_db_cache_pull(cache_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if((tlgrm_user_id = check_user(cache_ctx, &user_ctx, &update)) == 0) {

		/* User not found */

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_db_ids_set(ids_ctx, user_ctx.r_userid, tlgrm_user_id) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_db_sess_pull(sess_ctx, tlgrm_user_id) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(update.callback_query._is_used == NXS_YES) {

		/* callback query */

		nxs_chat_srv_u_tlgrm_answercallbackquery_push(&update.callback_query.id, NULL, 0);

		nxs_error(rc, handler_callback_exec(&update, sess_ctx, cache_ctx, &user_ctx), error);
	}
	else {

		nxs_error(rc, handler_message_exec(&update, sess_ctx, cache_ctx, &user_ctx), error);
	}

error:

	nxs_string_free(&base64_decoded);

	nxs_chat_srv_c_tlgrm_update_free(&update);

	nxs_chat_srv_c_user_ctx_free(&user_ctx);

	sess_ctx  = nxs_chat_srv_u_db_sess_free(sess_ctx);
	cache_ctx = nxs_chat_srv_u_db_cache_free(cache_ctx);
	ids_ctx   = nxs_chat_srv_u_db_ids_free(ids_ctx);

	return rc;
}

/* Module internal (static) functions */

static size_t check_user(nxs_chat_srv_u_db_cache_t *cache_ctx, nxs_chat_srv_m_user_ctx_t *user_ctx, nxs_chat_srv_m_tlgrm_update_t *update)
{
	nxs_string_t *tlgrm_username, *tlgrm_userlang, *s;
	size_t        tlgrm_user_id, i;
	nxs_bool_t    f;

	if(update->callback_query._is_used == NXS_YES) {

		tlgrm_user_id  = update->callback_query.from.id;
		tlgrm_username = &update->callback_query.from.username;
		tlgrm_userlang = &update->callback_query.from.language_code;
	}
	else {

		tlgrm_user_id  = update->message.from.id;
		tlgrm_username = &update->message.from.username;
		tlgrm_userlang = &update->message.from.language_code;
	}

	if(nxs_array_count(&nxs_chat_srv_cfg.dev_accounts) > 0) {

		for(f = NXS_NO, i = 0; i < nxs_array_count(&nxs_chat_srv_cfg.dev_accounts); i++) {

			s = nxs_array_get(&nxs_chat_srv_cfg.dev_accounts, i);

			if(nxs_string_cmp(s, 0, tlgrm_username, 0) == NXS_YES) {

				f = NXS_YES;

				break;
			}
		}

		if(f == NXS_NO) {

			return 0;
		}
	}

	if(nxs_chat_srv_u_db_cache_user_get(cache_ctx, tlgrm_username, user_ctx) == NXS_CHAT_SRV_E_OK) {

		nxs_string_clone(&user_ctx->t_userlang, tlgrm_userlang);

		return tlgrm_user_id;
	}

	return 0;
}

static nxs_chat_srv_err_t handler_callback_exec(nxs_chat_srv_m_tlgrm_update_t *update,
                                                nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                nxs_chat_srv_u_db_cache_t *    cache_ctx,
                                                nxs_chat_srv_m_user_ctx_t *    user_ctx)
{
	size_t                               i;
	nxs_chat_srv_m_db_sess_type_t        type;
	nxs_chat_srv_m_tlgrm_bttn_callback_t callback;
	nxs_chat_srv_err_t                   rc;

	rc = NXS_CHAT_SRV_E_OK;

	if(nxs_chat_srv_u_db_sess_check_exist(sess_ctx) == NXS_NO) {

		/* user has no sessions, skipping request */
	}
	else {

		if(nxs_chat_srv_c_tlgrm_bttn_callback_deserialize(&callback, &update->callback_query.data)) {

			nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->callback_query.message.chat.id, update, NULL);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		type = nxs_chat_srv_u_db_sess_get_type(sess_ctx);

		for(i = 0; handlers_callback[i].type != NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE; i++) {

			if(handlers_callback[i].type == type) {

				if(handlers_callback[i].handler != NULL) {

					nxs_error(
					        rc, handlers_callback[i].handler(&callback, update, sess_ctx, cache_ctx, user_ctx), error);
				}
				else {

					nxs_error(rc, NXS_CHAT_SRV_E_OK, error);
				}
			}
		}

		if(handlers_callback[i].handler != NULL) {

			nxs_error(rc, handlers_callback[i].handler(&callback, update, sess_ctx, cache_ctx, user_ctx), error);
		}
	}

error:

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.handler_callback_exec' (user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   nxs_chat_srv_u_db_sess_get_tlgrm_userid(sess_ctx),
		                   rc);
	}

	return rc;
}

static nxs_chat_srv_err_t handler_message_exec(nxs_chat_srv_m_tlgrm_update_t *update,
                                               nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                               nxs_chat_srv_u_db_cache_t *    cache_ctx,
                                               nxs_chat_srv_m_user_ctx_t *    user_ctx)
{
	size_t                        i, tlgrm_userid;
	nxs_chat_srv_m_db_sess_type_t type;
	nxs_chat_srv_err_t            rc;

	rc = NXS_CHAT_SRV_E_OK;

	tlgrm_userid = nxs_chat_srv_u_db_sess_get_tlgrm_userid(sess_ctx);

	if(update->message.reply_to_message != NULL) {

		/* reply to message */

		if(handler_message_exec_reply(update, user_ctx, tlgrm_userid) == NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_OK, error);
		}

		nxs_chat_srv_p_queue_worker_tlgrm_update_win_wrong_reply_runtime(update->message.chat.id, NULL);
	}

	/* empty reply or issue ID is not determined */

	if(nxs_chat_srv_u_db_sess_check_exist(sess_ctx) == NXS_NO) {

		/* user has no sessions, creating new one and starting dialog */

		if(handler_message_sess_type_empty(update, sess_ctx, cache_ctx, user_ctx) != NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}
	else {

		type = nxs_chat_srv_u_db_sess_get_type(sess_ctx);

		for(i = 0; handlers_message[i].type != NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE; i++) {

			if(handlers_message[i].type == type) {

				if(handlers_message[i].handler != NULL) {

					nxs_error(rc, handlers_message[i].handler(update, sess_ctx, cache_ctx, user_ctx), error);
				}
				else {

					nxs_error(rc, NXS_CHAT_SRV_E_OK, error);
				}
			}
		}

		if(handlers_message[i].handler != NULL) {

			nxs_error(rc, handlers_message[i].handler(update, sess_ctx, cache_ctx, user_ctx), error);
		}
	}

error:

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.handler_message_exec' (user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   tlgrm_userid,
		                   rc);
	}

	return rc;
}

static nxs_chat_srv_err_t handler_callback_sess_type_message(nxs_chat_srv_m_tlgrm_bttn_callback_t *callback,
                                                             nxs_chat_srv_m_tlgrm_update_t *       update,
                                                             nxs_chat_srv_u_db_sess_t *            sess_ctx,
                                                             nxs_chat_srv_u_db_cache_t *           cache_ctx,
                                                             nxs_chat_srv_m_user_ctx_t *           user_ctx)
{
	size_t                                   chat_id, message_id;
	nxs_chat_srv_err_t                       rc;
	nxs_chat_srv_m_db_cache_issue_priority_t issue_priority;
	nxs_array_t                              cache_projects;

	rc = NXS_CHAT_SRV_E_OK;

	chat_id    = nxs_chat_srv_u_db_sess_get_chat_id(sess_ctx);
	message_id = nxs_chat_srv_u_db_sess_get_message_id(sess_ctx);

	nxs_array_init2(&cache_projects, nxs_chat_srv_m_db_cache_project_t);

	switch(callback->type) {

		case NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_NEW_ISSUE:

			/* new issue creation process */

			nxs_chat_srv_u_db_cache_prio_get_default(cache_ctx, &issue_priority);

			nxs_chat_srv_u_db_cache_projects_get(cache_ctx, &cache_projects);

			if(nxs_chat_srv_u_db_sess_t_conv_to_new_issue(sess_ctx,
			                                              &cache_projects,
			                                              user_ctx->r_userid,
			                                              issue_priority.id,
			                                              issue_priority.name,
			                                              NULL,
			                                              NULL,
			                                              NULL) != NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_new_issue(sess_ctx, chat_id, message_id, update, NULL) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		case NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE:

			/* add comment to selected issue process */

			if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_added_to_issue(
			           sess_ctx, chat_id, 0, update, NULL, callback->object_id) != NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			nxs_chat_srv_u_db_sess_destroy(sess_ctx);

			break;

		case NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SESSION_DESTROY:

			nxs_chat_srv_u_db_sess_destroy(sess_ctx);

			if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_session_destroyed(
			           sess_ctx, update->callback_query.message.chat.id, update, NULL) != NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		default:

			nxs_log_write_warn(&process,
			                   "[%s]: wrong callback type (callback type: %d, "
			                   "user id: %zu)",
			                   nxs_proc_get_name(&process),
			                   callback->type,
			                   nxs_chat_srv_u_db_sess_get_tlgrm_userid(sess_ctx),
			                   rc);

			nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->callback_query.message.chat.id, update, NULL);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);

			break;
	}

error:

	nxs_array_free(&cache_projects);

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.handler_callback_sess_type_message' ("
		                   "user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   nxs_chat_srv_u_db_sess_get_tlgrm_userid(sess_ctx),
		                   rc);
	}

	return rc;
}

static nxs_chat_srv_err_t handler_callback_sess_type_new_issue(nxs_chat_srv_m_tlgrm_bttn_callback_t *callback,
                                                               nxs_chat_srv_m_tlgrm_update_t *       update,
                                                               nxs_chat_srv_u_db_sess_t *            sess_ctx,
                                                               nxs_chat_srv_u_db_cache_t *           cache_ctx,
                                                               nxs_chat_srv_m_user_ctx_t *           user_ctx)
{
	size_t                                   chat_id, message_id, projects_count;
	nxs_array_t                              projects;
	nxs_chat_srv_err_t                       rc;
	nxs_chat_srv_m_db_cache_issue_priority_t issue_priority;

	nxs_array_init2(&projects, nxs_chat_srv_m_db_sess_project_t);

	rc = NXS_CHAT_SRV_E_OK;

	chat_id    = nxs_chat_srv_u_db_sess_get_chat_id(sess_ctx);
	message_id = nxs_chat_srv_u_db_sess_get_message_id(sess_ctx);

	switch(callback->type) {

		case NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_CREATE_ISSUE:

			if(nxs_chat_srv_u_db_sess_set_wait_for(sess_ctx, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_SUBJECT) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_enter_subject(sess_ctx, chat_id, 0, update, NULL) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		case NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECT_PROJECT:

			if(nxs_chat_srv_u_db_sess_get_wait_for(sess_ctx) != NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_PROJECTNAME_REGEX) {

				/* skipping set 'wait for' if it already in correct value */

				if(nxs_chat_srv_u_db_sess_set_wait_for(sess_ctx, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_PROJECTNAME_REGEX) !=
				   NXS_CHAT_SRV_E_OK) {

					nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
					        sess_ctx, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}
			}

			if(nxs_chat_srv_u_db_sess_t_get_new_issue_projects(
			           sess_ctx, &projects, callback->object_id, NXS_CHAT_SRV_TLGRM_PROJECTS_LIMIT, &projects_count) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_log_write_warn(&process,
				                   "[%s]: can't find user selected projects from session (callback type: %d, "
				                   "user id: %zu)",
				                   nxs_proc_get_name(&process),
				                   callback->type,
				                   nxs_chat_srv_u_db_sess_get_tlgrm_userid(sess_ctx),
				                   rc);

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_select_project(
			           sess_ctx, chat_id, message_id, update, &projects, callback->object_id, projects_count, NULL) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		case NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECTED_PROJECT:

			if(nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx, callback->object_id, 0, NULL, NULL, NULL, NULL) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_new_issue(sess_ctx, chat_id, message_id, update, NULL) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		case NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECT_PRIORITY:

			if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_select_priority(
			           sess_ctx, cache_ctx, chat_id, message_id, update, NULL) != NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		case NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECTED_PRIORITY:

			if(nxs_chat_srv_u_db_cache_prio_get_by_id(cache_ctx, callback->object_id, &issue_priority) != NXS_CHAT_SRV_E_OK) {

				nxs_log_write_warn(&process,
				                   "[%s]: issue priority id does not exist (issue priority id: %zu, "
				                   "user id: %zu)",
				                   nxs_proc_get_name(&process),
				                   callback->object_id,
				                   nxs_chat_srv_u_db_sess_get_tlgrm_userid(sess_ctx),
				                   rc);

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx, 0, issue_priority.id, issue_priority.name, NULL, NULL, NULL) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_new_issue(sess_ctx, chat_id, message_id, update, NULL) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		case NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_CHANGE_DESCRIPTION:

			if(nxs_chat_srv_u_db_sess_set_wait_for(sess_ctx, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_DESCRIPTION) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_enter_description(sess_ctx, chat_id, 0, update, NULL) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		case NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SESSION_DESTROY:

			nxs_chat_srv_u_db_sess_destroy(sess_ctx);

			if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_session_destroyed(
			           sess_ctx, update->callback_query.message.chat.id, update, NULL) != NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		case NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_BACK:

			/*nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_create_issue(
			        sess_ctx, sess_id);*/

			if(nxs_chat_srv_u_db_sess_t_conv_to_message(sess_ctx, NULL) != NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_begin(sess_ctx, chat_id, message_id, update, NULL) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		default:

			nxs_log_write_warn(&process,
			                   "[%s]: wrong callback type (callback type: %d, "
			                   "user id: %zu)",
			                   nxs_proc_get_name(&process),
			                   callback->type,
			                   nxs_chat_srv_u_db_sess_get_tlgrm_userid(sess_ctx),
			                   rc);

			nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->callback_query.message.chat.id, update, NULL);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);

			break;
	}

error:

	nxs_array_free(&projects);

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.handler_callback_sess_type_new_issue' ("
		                   "user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   nxs_chat_srv_u_db_sess_get_tlgrm_userid(sess_ctx),
		                   rc);
	}

	return rc;
}

static nxs_chat_srv_err_t handler_message_sess_type_empty(nxs_chat_srv_m_tlgrm_update_t *update,
                                                          nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                          nxs_chat_srv_u_db_cache_t *    cache_ctx,
                                                          nxs_chat_srv_m_user_ctx_t *    user_ctx)
{
	nxs_chat_srv_m_tlgrm_message_t response_message;
	nxs_bool_t                     response_status;
	nxs_buf_t                      response_buf;
	nxs_chat_srv_err_t             rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_chat_srv_c_tlgrm_message_init(&response_message);
	nxs_buf_init(&response_buf, 1);

	if(nxs_chat_srv_u_db_sess_start(sess_ctx, 0, 0, NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_db_sess_t_set_message(sess_ctx, &update->message.text) != NXS_CHAT_SRV_E_OK) {

		nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_begin(sess_ctx, update->message.chat.id, 0, update, &response_buf) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_c_tlgrm_message_result_pull_json(&response_message, &response_status, &response_buf) != NXS_CHAT_SRV_E_OK) {

		nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_db_sess_set_ids(sess_ctx, response_message.chat.id, response_message.message_id) != NXS_CHAT_SRV_E_OK) {

		nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_buf_free(&response_buf);

	nxs_chat_srv_c_tlgrm_message_free(&response_message);

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.handler_message_sess_type_empty' ("
		                   "user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   nxs_chat_srv_u_db_sess_get_tlgrm_userid(sess_ctx),
		                   rc);
	}

	return rc;
}

static nxs_chat_srv_err_t handler_message_sess_type_message(nxs_chat_srv_m_tlgrm_update_t *update,
                                                            nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                            nxs_chat_srv_u_db_cache_t *    cache_ctx,
                                                            nxs_chat_srv_m_user_ctx_t *    user_ctx)
{
	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	switch(nxs_chat_srv_u_db_sess_get_wait_for(sess_ctx)) {

		default:

			nxs_error(
			        rc,
			        nxs_chat_srv_p_queue_worker_tlgrm_update_win_wrong_action(sess_ctx, update->message.chat.id, update, NULL),
			        error);

			break;
	}

error:

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.handler_message_sess_type_message' ("
		                   "user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   nxs_chat_srv_u_db_sess_get_tlgrm_userid(sess_ctx),
		                   rc);
	}

	return rc;
}

/* TODO: сейчас происходит двойное получение информации данных из сессии в этой функции и функциях comment_*, необходимо это устранить */
static nxs_chat_srv_err_t handler_message_sess_type_new_issue(nxs_chat_srv_m_tlgrm_update_t *update,
                                                              nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                              nxs_chat_srv_u_db_cache_t *    cache_ctx,
                                                              nxs_chat_srv_m_user_ctx_t *    user_ctx)
{
	nxs_string_t       description, subject, project_name_regex;
	size_t             chat_id, message_id, projects_count;
	nxs_array_t        projects;
	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&description);
	nxs_string_init(&subject);
	nxs_string_init(&project_name_regex);

	nxs_array_init2(&projects, nxs_chat_srv_m_db_sess_project_t);

	chat_id    = nxs_chat_srv_u_db_sess_get_chat_id(sess_ctx);
	message_id = nxs_chat_srv_u_db_sess_get_message_id(sess_ctx);

	switch(nxs_chat_srv_u_db_sess_get_wait_for(sess_ctx)) {

		case NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_SUBJECT:

			/* set subject process */

			if(nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx, NULL, NULL, NULL, NULL, &subject, NULL, NULL) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_u_db_sess_set_wait_for(sess_ctx, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE) != NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_string_cmp(&subject, 0, &update->message.text, 0) != NXS_YES) {

				/* if subject is changed */

				if(nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx, 0, 0, NULL, &update->message.text, NULL, NULL) !=
				   NXS_CHAT_SRV_E_OK) {

					nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_new_issue(sess_ctx, chat_id, message_id, update, NULL) !=
				   NXS_CHAT_SRV_E_OK) {

					nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}
			}

			if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_issue_created(sess_ctx, chat_id, 0, update, NULL) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			nxs_chat_srv_u_db_sess_destroy(sess_ctx);

			break;

		case NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_DESCRIPTION:

			/* set description process */

			if(nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx, NULL, NULL, NULL, NULL, NULL, &description, NULL) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_u_db_sess_set_wait_for(sess_ctx, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE) != NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_string_cmp(&description, 0, &update->message.text, 0) != NXS_YES) {

				/* if description is changed */

				if(nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx, 0, 0, NULL, NULL, &update->message.text, NULL) !=
				   NXS_CHAT_SRV_E_OK) {

					nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_new_issue(sess_ctx, chat_id, message_id, update, NULL) !=
				   NXS_CHAT_SRV_E_OK) {

					nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}
			}

			if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_description_changed(sess_ctx, chat_id, update, NULL) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		case NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_PROJECTNAME_REGEX:

			/* set description process */

			if(nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx, NULL, NULL, NULL, NULL, NULL, NULL, &project_name_regex) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_string_cmp(&project_name_regex, 0, &update->message.text, 0) != NXS_YES) {

				/* if project name regex is changed */

				if(nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx, 0, 0, NULL, NULL, NULL, &update->message.text) !=
				   NXS_CHAT_SRV_E_OK) {

					nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(nxs_chat_srv_u_db_sess_t_get_new_issue_projects(
				           sess_ctx, &projects, 0, NXS_CHAT_SRV_TLGRM_PROJECTS_LIMIT, &projects_count) !=
				   NXS_CHAT_SRV_E_OK) {

					nxs_log_write_warn(&process,
					                   "[%s]: can't find user projects in cache ("
					                   "user id: %zu)",
					                   nxs_proc_get_name(&process),
					                   nxs_chat_srv_u_db_sess_get_tlgrm_userid(sess_ctx),
					                   rc);

					nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
					        sess_ctx, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_select_project(
				           sess_ctx, chat_id, message_id, update, &projects, 0, projects_count, NULL) !=
				   NXS_CHAT_SRV_E_OK) {

					nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
					        sess_ctx, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}
			}

			break;

		default:

			nxs_error(
			        rc,
			        nxs_chat_srv_p_queue_worker_tlgrm_update_win_wrong_action(sess_ctx, update->message.chat.id, update, NULL),
			        error);

			break;
	}

error:

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.handler_message_sess_type_new_issue' ("
		                   "user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   nxs_chat_srv_u_db_sess_get_tlgrm_userid(sess_ctx),
		                   rc);
	}

	nxs_array_free(&projects);

	nxs_string_free(&description);
	nxs_string_free(&subject);
	nxs_string_free(&project_name_regex);

	return rc;
}

static nxs_chat_srv_err_t
        handler_message_exec_reply(nxs_chat_srv_m_tlgrm_update_t *update, nxs_chat_srv_m_user_ctx_t *user_ctx, size_t tlgrm_userid)
{
	nxs_chat_srv_u_db_issues_t *  db_issue_ctx;
	nxs_chat_srv_u_rdmn_user_t *  rdmn_user_ctx;
	nxs_chat_srv_u_rdmn_issues_t *rdmn_issues_ctx;
	nxs_string_t *                api_key;
	nxs_chat_srv_err_t            rc;
	size_t                        issue_id;

	rc = NXS_CHAT_SRV_E_OK;

	db_issue_ctx    = nxs_chat_srv_u_db_issues_init();
	rdmn_user_ctx   = nxs_chat_srv_u_rdmn_user_init();
	rdmn_issues_ctx = nxs_chat_srv_u_rdmn_issues_init();

	issue_id = 0;

	if(nxs_chat_srv_u_db_issues_get(db_issue_ctx, tlgrm_userid, update->message.reply_to_message->message_id, &issue_id) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: can't send user reply into Redmine: can't get issue_id ("
		                   "user id: %zu)",
		                   nxs_proc_get_name(&process),
		                   tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(issue_id == 0) {

		nxs_log_write_warn(&process,
		                   "[%s]: can't send user reply into Redmine: issue_id is 0("
		                   "user id: %zu)",
		                   nxs_proc_get_name(&process),
		                   tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);
	}

	if(nxs_chat_srv_u_rdmn_user_pull(rdmn_user_ctx, user_ctx->r_userid) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: can't send user reply into Redmine: can't get user data from Redmine ("
		                   "user id: %zu)",
		                   nxs_proc_get_name(&process),
		                   tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	api_key = nxs_chat_srv_u_rdmn_user_get_api_key(rdmn_user_ctx);

	if(nxs_string_len(api_key) == 0) {

		nxs_log_write_warn(&process,
		                   "[%s]: can't send user reply into Redmine: user api key has zero size (user id: %zu)",
		                   nxs_proc_get_name(&process),
		                   tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_rdmn_issues_add_note(rdmn_issues_ctx, issue_id, &update->message.text, api_key) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: can't send user reply into Redmine: can't add not into Redmine issue ("
		                   "user id: %zu)",
		                   nxs_proc_get_name(&process),
		                   tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_db_issues_set(db_issue_ctx, tlgrm_userid, update->message.message_id, issue_id) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: can't send user reply into Redmine: can't set value into db-issues ("
		                   "user id: %zu)",
		                   nxs_proc_get_name(&process),
		                   tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	db_issue_ctx    = nxs_chat_srv_u_db_issues_free(db_issue_ctx);
	rdmn_user_ctx   = nxs_chat_srv_u_rdmn_user_free(rdmn_user_ctx);
	rdmn_issues_ctx = nxs_chat_srv_u_rdmn_issues_free(rdmn_issues_ctx);

	return rc;
}
