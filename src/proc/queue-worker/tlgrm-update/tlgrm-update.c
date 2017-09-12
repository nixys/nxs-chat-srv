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
	nxs_chat_srv_err_t			(*handler)(nxs_chat_srv_m_tlgrm_bttn_callback_t *callback, nxs_chat_srv_m_tlgrm_update_t *update, nxs_chat_srv_u_db_sess_t *sess_ctx, nxs_chat_srv_u_db_users_t *users_ctx, size_t tlgrm_userid, nxs_chat_srv_u_queue_cache_t *queue_cache);
} handlers_callback_t;

typedef struct
{
	nxs_chat_srv_m_db_sess_type_t		type;
	nxs_chat_srv_err_t			(*handler)(nxs_chat_srv_m_tlgrm_update_t *update, nxs_chat_srv_u_db_sess_t *sess_ctx, nxs_chat_srv_u_db_users_t *users_ctx, size_t tlgrm_userid, nxs_chat_srv_u_queue_cache_t *queue_cache);
} handlers_message_t;

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_bool_t check_user(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                             nxs_chat_srv_u_db_users_t *    users_ctx,
                             nxs_chat_srv_m_tlgrm_update_t *update,
                             size_t *                       tlgrm_user_id);

static nxs_chat_srv_err_t handler_callback_exec(nxs_chat_srv_m_tlgrm_update_t *update,
                                                nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                nxs_chat_srv_u_db_users_t *    users_ctx,
                                                size_t                         tlgrm_user_id,
                                                nxs_chat_srv_u_queue_cache_t * queue_cache);

static nxs_chat_srv_err_t handler_message_exec(nxs_chat_srv_m_tlgrm_update_t *update,
                                               nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                               nxs_chat_srv_u_db_users_t *    users_ctx,
                                               size_t                         tlgrm_user_id,
                                               nxs_chat_srv_u_queue_cache_t * queue_cache);

static nxs_chat_srv_err_t handler_callback_sess_type_message(nxs_chat_srv_m_tlgrm_bttn_callback_t *callback,
                                                             nxs_chat_srv_m_tlgrm_update_t *       update,
                                                             nxs_chat_srv_u_db_sess_t *            sess_ctx,
                                                             nxs_chat_srv_u_db_users_t *           users_ctx,
                                                             size_t                                tlgrm_userid,
                                                             nxs_chat_srv_u_queue_cache_t *        queue_cache);
static nxs_chat_srv_err_t handler_callback_sess_type_new_issue(nxs_chat_srv_m_tlgrm_bttn_callback_t *callback,
                                                               nxs_chat_srv_m_tlgrm_update_t *       update,
                                                               nxs_chat_srv_u_db_sess_t *            sess_ctx,
                                                               nxs_chat_srv_u_db_users_t *           users_ctx,
                                                               size_t                                tlgrm_userid,
                                                               nxs_chat_srv_u_queue_cache_t *        queue_cache);

static nxs_chat_srv_err_t handler_message_sess_type_empty(nxs_chat_srv_m_tlgrm_update_t *update,
                                                          nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                          nxs_chat_srv_u_db_users_t *    users_ctx,
                                                          size_t                         tlgrm_userid,
                                                          nxs_chat_srv_u_queue_cache_t * queue_cache);

static nxs_chat_srv_err_t handler_message_sess_type_message(nxs_chat_srv_m_tlgrm_update_t *update,
                                                            nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                            nxs_chat_srv_u_db_users_t *    users_ctx,
                                                            size_t                         tlgrm_userid,
                                                            nxs_chat_srv_u_queue_cache_t * queue_cache);
static nxs_chat_srv_err_t handler_message_sess_type_new_issue(nxs_chat_srv_m_tlgrm_update_t *update,
                                                              nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                              nxs_chat_srv_u_db_users_t *    users_ctx,
                                                              size_t                         tlgrm_userid,
                                                              nxs_chat_srv_u_queue_cache_t * queue_cache);

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

/* TODO: replace this by real redmine projects array (used by dev purposes) */
static nxs_string_t tmp_priority[] =
{
	nxs_string("Низкий"),
	nxs_string("Нормальный"),
	nxs_string("Высокий"),

	nxs_string("")
};

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_runtime(nxs_chat_srv_m_queue_com_types_t type,
                                                                    nxs_string_t *                   data,
                                                                    nxs_chat_srv_u_queue_cache_t *   queue_cache)
{
	nxs_string_t                  base64_decoded;
	nxs_chat_srv_m_tlgrm_update_t update;
	nxs_chat_srv_u_db_users_t *   users_ctx;
	nxs_chat_srv_u_db_sess_t *    sess_ctx;
	nxs_chat_srv_err_t            rc;
	size_t                        tlgrm_user_id;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&base64_decoded);

	nxs_chat_srv_c_tlgrm_update_init(&update);

	users_ctx = nxs_chat_srv_u_db_users_init();
	sess_ctx  = nxs_chat_srv_u_db_sess_init();

	nxs_base64_decode_string(&base64_decoded, data);

	if(nxs_chat_srv_c_tlgrm_update_pull_json(&update, (nxs_buf_t *)&base64_decoded) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_db_users_pull(users_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(check_user(sess_ctx, users_ctx, &update, &tlgrm_user_id) == NXS_NO) {

		/* User not found */

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_db_sess_pull(sess_ctx, tlgrm_user_id) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(update.callback_query._is_used == NXS_YES) {

		/* callback query */

		nxs_chat_srv_u_tlgrm_answercallbackquery_push(&update.callback_query.id, NULL, 0);

		nxs_error(rc, handler_callback_exec(&update, sess_ctx, users_ctx, tlgrm_user_id, queue_cache), error);
	}
	else {

		nxs_error(rc, handler_message_exec(&update, sess_ctx, users_ctx, tlgrm_user_id, queue_cache), error);
	}

error:

	nxs_string_free(&base64_decoded);

	nxs_chat_srv_c_tlgrm_update_free(&update);

	users_ctx = nxs_chat_srv_u_db_users_free(users_ctx);
	sess_ctx  = nxs_chat_srv_u_db_sess_free(sess_ctx);

	return rc;
}

/* Module internal (static) functions */

static nxs_bool_t check_user(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                             nxs_chat_srv_u_db_users_t *    users_ctx,
                             nxs_chat_srv_m_tlgrm_update_t *update,
                             size_t *                       tlgrm_user_id)
{
	nxs_chat_srv_m_db_users_t m_user;

	*tlgrm_user_id = 0;

	if(update->callback_query._is_used == NXS_YES) {

		*tlgrm_user_id = update->callback_query.from.id;
	}
	else {

		*tlgrm_user_id = update->message.from.id;
	}

	if(nxs_chat_srv_u_db_users_find_by_tid(users_ctx, *tlgrm_user_id, &m_user) == NXS_CHAT_SRV_E_OK) {

		return NXS_YES;
	}

	return NXS_NO;
}

static nxs_chat_srv_err_t handler_callback_exec(nxs_chat_srv_m_tlgrm_update_t *update,
                                                nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                nxs_chat_srv_u_db_users_t *    users_ctx,
                                                size_t                         tlgrm_user_id,
                                                nxs_chat_srv_u_queue_cache_t * queue_cache)
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

					nxs_error(rc,
					          handlers_callback[i].handler(
					                  &callback, update, sess_ctx, users_ctx, tlgrm_user_id, queue_cache),
					          error);
				}
				else {

					nxs_error(rc, NXS_CHAT_SRV_E_OK, error);
				}
			}
		}

		if(handlers_callback[i].handler != NULL) {

			nxs_error(rc,
			          handlers_callback[i].handler(&callback, update, sess_ctx, users_ctx, tlgrm_user_id, queue_cache),
			          error);
		}
	}

error:

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.handler_callback_exec' (user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   tlgrm_user_id,
		                   rc);
	}

	return rc;
}

static nxs_chat_srv_err_t handler_message_exec(nxs_chat_srv_m_tlgrm_update_t *update,
                                               nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                               nxs_chat_srv_u_db_users_t *    users_ctx,
                                               size_t                         tlgrm_user_id,
                                               nxs_chat_srv_u_queue_cache_t * queue_cache)
{
	size_t                        i;
	nxs_chat_srv_m_db_sess_type_t type;
	nxs_chat_srv_err_t            rc;

	rc = NXS_CHAT_SRV_E_OK;

	if(update->message.reply_to_message != NULL) {

		/* reply to message */
	}
	else {

		/* empty reply */

		if(nxs_chat_srv_u_db_sess_check_exist(sess_ctx) == NXS_NO) {

			/* user has no sessions, creating new one and starting dialog */

			if(handler_message_sess_type_empty(update, sess_ctx, users_ctx, tlgrm_user_id, queue_cache) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}
		}
		else {

			type = nxs_chat_srv_u_db_sess_get_type(sess_ctx);

			for(i = 0; handlers_message[i].type != NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE; i++) {

				if(handlers_message[i].type == type) {

					if(handlers_message[i].handler != NULL) {

						nxs_error(rc,
						          handlers_message[i].handler(
						                  update, sess_ctx, users_ctx, tlgrm_user_id, queue_cache),
						          error);
					}
					else {

						nxs_error(rc, NXS_CHAT_SRV_E_OK, error);
					}
				}
			}

			if(handlers_message[i].handler != NULL) {

				nxs_error(rc, handlers_message[i].handler(update, sess_ctx, users_ctx, tlgrm_user_id, queue_cache), error);
			}
		}
	}

error:

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.handler_message_exec' (user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   tlgrm_user_id,
		                   rc);
	}

	return rc;
}

static nxs_chat_srv_err_t handler_callback_sess_type_message(nxs_chat_srv_m_tlgrm_bttn_callback_t *callback,
                                                             nxs_chat_srv_m_tlgrm_update_t *       update,
                                                             nxs_chat_srv_u_db_sess_t *            sess_ctx,
                                                             nxs_chat_srv_u_db_users_t *           users_ctx,
                                                             size_t                                tlgrm_userid,
                                                             nxs_chat_srv_u_queue_cache_t *        queue_cache)
{
	size_t                    chat_id, message_id, project_id;
	nxs_chat_srv_m_db_users_t user;
	nxs_string_t              project_name;
	nxs_chat_srv_err_t        rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&project_name);

	chat_id    = nxs_chat_srv_u_db_sess_get_chat_id(sess_ctx);
	message_id = nxs_chat_srv_u_db_sess_get_message_id(sess_ctx);

	switch(callback->type) {

		case NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_NEW_ISSUE:

			/* new issue creation process */

			if(nxs_chat_srv_u_db_users_find_by_tid(users_ctx, tlgrm_userid, &user) != NXS_CHAT_SRV_E_OK) {

				nxs_log_write_warn(&process,
				                   "[%s]: can't find user in users ctx (callback type: %d, "
				                   "user id: %zu)",
				                   nxs_proc_get_name(&process),
				                   callback->type,
				                   tlgrm_userid,
				                   rc);

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			nxs_chat_srv_u_queue_cache_get_user_project_first(queue_cache, user.rdmn_userid, &project_id, &project_name);

			if(nxs_chat_srv_u_db_sess_t_conv_to_new_issue(
			           sess_ctx, project_id, &project_name, 0, &tmp_priority[1], NULL, NULL, NULL) != NXS_CHAT_SRV_E_OK) {

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
			                   tlgrm_userid,
			                   rc);

			nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->callback_query.message.chat.id, update, NULL);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);

			break;
	}

error:

	nxs_string_free(&project_name);

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.handler_callback_sess_type_message' ("
		                   "user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   tlgrm_userid,
		                   rc);
	}

	return rc;
}

static nxs_chat_srv_err_t handler_callback_sess_type_new_issue(nxs_chat_srv_m_tlgrm_bttn_callback_t *callback,
                                                               nxs_chat_srv_m_tlgrm_update_t *       update,
                                                               nxs_chat_srv_u_db_sess_t *            sess_ctx,
                                                               nxs_chat_srv_u_db_users_t *           users_ctx,
                                                               size_t                                tlgrm_userid,
                                                               nxs_chat_srv_u_queue_cache_t *        queue_cache)
{
	nxs_string_t              callback_str, description, subject, project_name, priority_name, project_name_regex, message;
	size_t                    priority_id, project_id, chat_id, message_id, projects_count;
	nxs_array_t               projects;
	nxs_chat_srv_m_db_users_t user;
	nxs_chat_srv_err_t        rc;

	nxs_string_init(&callback_str);
	nxs_string_init(&description);
	nxs_string_init(&subject);
	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&project_name_regex);
	nxs_string_init(&message);

	nxs_array_init2(&projects, nxs_chat_srv_m_queue_cache_project_t);

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

			if(nxs_chat_srv_u_db_users_find_by_tid(users_ctx, tlgrm_userid, &user) != NXS_CHAT_SRV_E_OK) {

				nxs_log_write_warn(&process,
				                   "[%s]: can't find user in users ctx (callback type: %d, "
				                   "user id: %zu)",
				                   nxs_proc_get_name(&process),
				                   callback->type,
				                   tlgrm_userid,
				                   rc);

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
			                                          &project_id,
			                                          &project_name,
			                                          &priority_id,
			                                          &priority_name,
			                                          &subject,
			                                          &description,
			                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_u_queue_cache_get_user_projects(queue_cache,
			                                                user.rdmn_userid,
			                                                &projects,
			                                                &project_name_regex,
			                                                callback->object_id,
			                                                NXS_CHAT_SRV_TLGRM_PROJECTS_LIMIT,
			                                                &projects_count) != NXS_CHAT_SRV_E_OK) {

				nxs_log_write_warn(&process,
				                   "[%s]: can't find user projects in cache (callback type: %d, "
				                   "user id: %zu)",
				                   nxs_proc_get_name(&process),
				                   callback->type,
				                   tlgrm_userid,
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

			if(nxs_chat_srv_u_db_users_find_by_tid(users_ctx, tlgrm_userid, &user) != NXS_CHAT_SRV_E_OK) {

				nxs_log_write_warn(&process,
				                   "[%s]: can't find user in users ctx (callback type: %d, "
				                   "user id: %zu)",
				                   nxs_proc_get_name(&process),
				                   callback->type,
				                   tlgrm_userid,
				                   rc);

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
			                                          &project_id,
			                                          &project_name,
			                                          &priority_id,
			                                          &priority_name,
			                                          &subject,
			                                          &description,
			                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			nxs_chat_srv_u_queue_cache_get_user_project_by_id(
			        queue_cache, user.rdmn_userid, callback->object_id, &project_name);

			if(nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx,
			                                          callback->object_id,
			                                          &project_name,
			                                          priority_id,
			                                          &priority_name,
			                                          &subject,
			                                          &description,
			                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

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

			if(nxs_chat_srv_p_queue_worker_tlgrm_update_win_select_priority(sess_ctx, chat_id, message_id, update, NULL) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		case NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECTED_PRIORITY:

			if(nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
			                                          &project_id,
			                                          &project_name,
			                                          &priority_id,
			                                          &priority_name,
			                                          &subject,
			                                          &description,
			                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
				        sess_ctx, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx,
			                                          project_id,
			                                          &project_name,
			                                          callback->object_id,
			                                          &tmp_priority[callback->object_id],
			                                          &subject,
			                                          &description,
			                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

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
			                   tlgrm_userid,
			                   rc);

			nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->callback_query.message.chat.id, update, NULL);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);

			break;
	}

error:

	nxs_string_free(&callback_str);
	nxs_string_free(&description);
	nxs_string_free(&subject);
	nxs_string_free(&project_name);
	nxs_string_free(&priority_name);
	nxs_string_free(&project_name_regex);
	nxs_string_free(&message);

	nxs_array_free(&projects);

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.handler_callback_sess_type_new_issue' ("
		                   "user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   tlgrm_userid,
		                   rc);
	}

	return rc;
}

static nxs_chat_srv_err_t handler_message_sess_type_empty(nxs_chat_srv_m_tlgrm_update_t *update,
                                                          nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                          nxs_chat_srv_u_db_users_t *    users_ctx,
                                                          size_t                         tlgrm_userid,
                                                          nxs_chat_srv_u_queue_cache_t * queue_cache)
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
		                   tlgrm_userid,
		                   rc);
	}

	return rc;
}

static nxs_chat_srv_err_t handler_message_sess_type_message(nxs_chat_srv_m_tlgrm_update_t *update,
                                                            nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                            nxs_chat_srv_u_db_users_t *    users_ctx,
                                                            size_t                         tlgrm_userid,
                                                            nxs_chat_srv_u_queue_cache_t * queue_cache)
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
		                   tlgrm_userid,
		                   rc);
	}

	return rc;
}

/* TODO: сейчас происходит двойное получение информации данных из сессии в этой функции и функциях comment_*, необходимо это устранить */
static nxs_chat_srv_err_t handler_message_sess_type_new_issue(nxs_chat_srv_m_tlgrm_update_t *update,
                                                              nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                              nxs_chat_srv_u_db_users_t *    users_ctx,
                                                              size_t                         tlgrm_userid,
                                                              nxs_chat_srv_u_queue_cache_t * queue_cache)
{
	nxs_string_t              callback_str, description, subject, project_name, priority_name, project_name_regex, message;
	size_t                    priority_id, project_id, chat_id, message_id, projects_count;
	nxs_chat_srv_m_db_users_t user;
	nxs_array_t               projects;
	nxs_chat_srv_err_t        rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);
	nxs_string_init(&description);
	nxs_string_init(&subject);
	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&project_name_regex);
	nxs_string_init(&message);

	nxs_array_init2(&projects, nxs_chat_srv_m_queue_cache_project_t);

	chat_id    = nxs_chat_srv_u_db_sess_get_chat_id(sess_ctx);
	message_id = nxs_chat_srv_u_db_sess_get_message_id(sess_ctx);

	switch(nxs_chat_srv_u_db_sess_get_wait_for(sess_ctx)) {

		case NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_SUBJECT:

			/* set subject process */

			if(nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
			                                          &project_id,
			                                          &project_name,
			                                          &priority_id,
			                                          &priority_name,
			                                          &subject,
			                                          &description,
			                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_u_db_sess_set_wait_for(sess_ctx, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE) != NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_string_cmp(&subject, 0, &update->message.text, 0) != NXS_YES) {

				/* if subject is changed */

				if(nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx,
				                                          project_id,
				                                          &project_name,
				                                          priority_id,
				                                          &priority_name,
				                                          &update->message.text,
				                                          &description,
				                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

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

			if(nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
			                                          &project_id,
			                                          &project_name,
			                                          &priority_id,
			                                          &priority_name,
			                                          &subject,
			                                          &description,
			                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_u_db_sess_set_wait_for(sess_ctx, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE) != NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_string_cmp(&description, 0, &update->message.text, 0) != NXS_YES) {

				/* if description is changed */

				if(nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx,
				                                          project_id,
				                                          &project_name,
				                                          priority_id,
				                                          &priority_name,
				                                          &subject,
				                                          &update->message.text,
				                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

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

			if(nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
			                                          &project_id,
			                                          &project_name,
			                                          &priority_id,
			                                          &priority_name,
			                                          &subject,
			                                          &description,
			                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

				nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_string_cmp(&project_name_regex, 0, &update->message.text, 0) != NXS_YES) {

				/* if project name regex is changed */

				if(nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx,
				                                          project_id,
				                                          &project_name,
				                                          priority_id,
				                                          &priority_name,
				                                          &subject,
				                                          &description,
				                                          &update->message.text) != NXS_CHAT_SRV_E_OK) {

					nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(sess_ctx, update->message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(nxs_chat_srv_u_db_users_find_by_tid(users_ctx, tlgrm_userid, &user) != NXS_CHAT_SRV_E_OK) {

					nxs_log_write_warn(&process,
					                   "[%s]: can't find user in users ctx ("
					                   "user id: %zu)",
					                   nxs_proc_get_name(&process),
					                   tlgrm_userid,
					                   rc);

					nxs_chat_srv_p_queue_worker_tlgrm_update_win_error(
					        sess_ctx, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(nxs_chat_srv_u_queue_cache_get_user_projects(queue_cache,
				                                                user.rdmn_userid,
				                                                &projects,
				                                                &update->message.text,
				                                                0,
				                                                NXS_CHAT_SRV_TLGRM_PROJECTS_LIMIT,
				                                                &projects_count) != NXS_CHAT_SRV_E_OK) {

					nxs_log_write_warn(&process,
					                   "[%s]: can't find user projects in cache ("
					                   "user id: %zu)",
					                   nxs_proc_get_name(&process),
					                   tlgrm_userid,
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
		                   tlgrm_userid,
		                   rc);
	}

	nxs_array_free(&projects);

	nxs_string_free(&callback_str);
	nxs_string_free(&description);
	nxs_string_free(&subject);
	nxs_string_free(&project_name);
	nxs_string_free(&priority_name);
	nxs_string_free(&project_name_regex);
	nxs_string_free(&message);

	return rc;
}
