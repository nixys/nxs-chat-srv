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

#define PROJECTS_LIMIT	5

#define MESSAGE_ISSUE	"*Создание новой задачи*\n\n" \
			"*Проект:* %r\n" \
			"*Приоритет:* %r\n" \
			"*Тема:* %r\n" \
			"*Описание:* %r\n---------\n" \
			"_Ниже Вы можете изменить параметры новой задачи_"


#define BUTTON_ADD_LAST_ACTIVE		(u_char *)"Добавить в последнюю активную"
#define BUTTON_NEW_ISSUE		(u_char *)"Создать новую задачу"
#define BUTTON_ADD_ADD_TO_ISSUE		(u_char *)"Добавить в #%zu - %s"
#define BUTTON_PROJECT			(u_char *)"Проект"
#define BUTTON_PRIORITY			(u_char *)"Приоритет"
#define BUTTON_DESCRIPTION		(u_char *)"Описание"
#define BUTTON_CREATE_ISSUE		(u_char *)"Создать задачу %s"
#define BUTTON_BACK			(u_char *)"<< Назад"
#define BUTTON_FORWARD			(u_char *)"Вперёд >>"
#define BUTTON_DESTROY_SESSION		(u_char *)"Начать диалог заново"

/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef enum
{
	CALLBACK_TYPE_NONE,
	CALLBACK_TYPE_NEW_ISSUE,		/* create new issue */
	CALLBACK_TYPE_TO_ISSUE,			/* add comment to existing issue */
	CALLBACK_TYPE_CREATE_ISSUE,		/* create new issue */
	CALLBACK_TYPE_SELECT_PROJECT,		/* select project */
	CALLBACK_TYPE_SELECTED_PROJECT,		/* selected project */
	CALLBACK_TYPE_SELECT_PRIORITY,		/* select priority */
	CALLBACK_TYPE_SELECTED_PRIORITY,	/* selected priority */
	CALLBACK_TYPE_CHANGE_DESCRIPTION,	/* change issue description */
	CALLBACK_TYPE_SESSION_DESTROY,		/* destroy current session */
	CALLBACK_TYPE_BACK,			/* back to previous 'window' */
} callback_type_t;

typedef struct
{
	size_t			sess_id;
	callback_type_t		type;
	size_t			object_id;
} callback_t;

typedef struct
{
	nxs_chat_srv_m_db_sess_type_t		type;
	nxs_chat_srv_err_t			(*handler)(callback_t *callback, nxs_chat_srv_m_tlgrm_update_t *update, nxs_chat_srv_u_db_sess_t *sess_ctx, nxs_chat_srv_u_db_users_t *users_ctx, size_t sess_id, size_t user_id, nxs_chat_srv_u_queue_cache_t *queue_cache);
} callback_handlers_t;

typedef struct
{
	nxs_chat_srv_m_db_sess_type_t		type;
	nxs_chat_srv_err_t			(*handler)(nxs_chat_srv_m_tlgrm_update_t *update, nxs_chat_srv_u_db_sess_t *sess_ctx, nxs_chat_srv_u_db_users_t *users_ctx, size_t sess_id, size_t user_id, nxs_chat_srv_u_queue_cache_t *queue_cache);
} message_handlers_t;

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_bool_t check_user(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                             nxs_chat_srv_u_db_users_t *    users_ctx,
                             nxs_chat_srv_m_tlgrm_update_t *update,
                             size_t *                       sess_id,
                             size_t *                       user_id);

static nxs_chat_srv_err_t callback_handler_exec(nxs_chat_srv_m_tlgrm_update_t *update,
                                                nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                nxs_chat_srv_u_db_users_t *    users_ctx,
                                                size_t                         sess_id,
                                                size_t                         user_id,
                                                nxs_chat_srv_u_queue_cache_t * queue_cache);

static nxs_chat_srv_err_t message_handler_exec(nxs_chat_srv_m_tlgrm_update_t *update,
                                               nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                               nxs_chat_srv_u_db_users_t *    users_ctx,
                                               size_t                         sess_id,
                                               size_t                         user_id,
                                               nxs_chat_srv_u_queue_cache_t * queue_cache);

static nxs_chat_srv_err_t callback_handler_sess_type_message(callback_t *                   callback,
                                                             nxs_chat_srv_m_tlgrm_update_t *update,
                                                             nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                             nxs_chat_srv_u_db_users_t *    users_ctx,
                                                             size_t                         sess_id,
                                                             size_t                         user_id,
                                                             nxs_chat_srv_u_queue_cache_t * queue_cache);
static nxs_chat_srv_err_t callback_handler_sess_type_new_issue(callback_t *                   callback,
                                                               nxs_chat_srv_m_tlgrm_update_t *update,
                                                               nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                               nxs_chat_srv_u_db_users_t *    users_ctx,
                                                               size_t                         sess_id,
                                                               size_t                         user_id,
                                                               nxs_chat_srv_u_queue_cache_t * queue_cache);

static nxs_chat_srv_err_t message_handler_sess_type_message(nxs_chat_srv_m_tlgrm_update_t *update,
                                                            nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                            nxs_chat_srv_u_db_users_t *    users_ctx,
                                                            size_t                         sess_id,
                                                            size_t                         user_id,
                                                            nxs_chat_srv_u_queue_cache_t * queue_cache);
static nxs_chat_srv_err_t message_handler_sess_type_new_issue(nxs_chat_srv_m_tlgrm_update_t *update,
                                                              nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                              nxs_chat_srv_u_db_users_t *    users_ctx,
                                                              size_t                         sess_id,
                                                              size_t                         user_id,
                                                              nxs_chat_srv_u_queue_cache_t * queue_cache);

static nxs_chat_srv_err_t comment_begin(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                        size_t                         sess_id,
                                        size_t                         chat_id,
                                        size_t                         message_id,
                                        nxs_chat_srv_m_tlgrm_update_t *update,
                                        nxs_buf_t *                    response_buf);
static nxs_chat_srv_err_t comment_new_issue(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                            size_t                         sess_id,
                                            size_t                         chat_id,
                                            size_t                         message_id,
                                            nxs_chat_srv_m_tlgrm_update_t *update,
                                            nxs_buf_t *                    response_buf);
static nxs_chat_srv_err_t comment_select_project(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                 size_t                         sess_id,
                                                 size_t                         chat_id,
                                                 size_t                         message_id,
                                                 nxs_chat_srv_m_tlgrm_update_t *update,
                                                 nxs_array_t *                  projects,
                                                 size_t                         offset,
                                                 size_t                         projects_count,
                                                 nxs_buf_t *                    response_buf);
static nxs_chat_srv_err_t comment_select_priority(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                  size_t                         sess_id,
                                                  size_t                         chat_id,
                                                  size_t                         message_id,
                                                  nxs_chat_srv_m_tlgrm_update_t *update,
                                                  nxs_buf_t *                    response_buf);
static nxs_chat_srv_err_t comment_enter_subject(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                size_t                         sess_id,
                                                size_t                         chat_id,
                                                size_t                         message_id,
                                                nxs_chat_srv_m_tlgrm_update_t *update,
                                                nxs_buf_t *                    response_buf);
static nxs_chat_srv_err_t comment_enter_description(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                    size_t                         sess_id,
                                                    size_t                         chat_id,
                                                    size_t                         message_id,
                                                    nxs_chat_srv_m_tlgrm_update_t *update,
                                                    nxs_buf_t *                    response_buf);
static nxs_chat_srv_err_t comment_issue_created(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                size_t                         sess_id,
                                                size_t                         chat_id,
                                                size_t                         message_id,
                                                nxs_chat_srv_m_tlgrm_update_t *update,
                                                nxs_buf_t *                    response_buf);
static nxs_chat_srv_err_t comment_to_issue(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                           size_t                         sess_id,
                                           size_t                         chat_id,
                                           size_t                         message_id,
                                           nxs_chat_srv_m_tlgrm_update_t *update,
                                           nxs_buf_t *                    response_buf,
                                           size_t                         issue_id);
static nxs_chat_srv_err_t comment_error(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                        size_t                         sess_id,
                                        size_t                         chat_id,
                                        nxs_chat_srv_m_tlgrm_update_t *update,
                                        nxs_buf_t *                    response_buf);
static nxs_chat_srv_err_t comment_wrong_action(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                               size_t                         sess_id,
                                               size_t                         chat_id,
                                               nxs_chat_srv_m_tlgrm_update_t *update,
                                               nxs_buf_t *                    response_buf);
static nxs_chat_srv_err_t comment_session_destroyed(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                    size_t                         sess_id,
                                                    size_t                         chat_id,
                                                    nxs_chat_srv_m_tlgrm_update_t *update,
                                                    nxs_buf_t *                    response_buf);
static nxs_chat_srv_err_t comment_description_changed(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                      size_t                         sess_id,
                                                      size_t                         chat_id,
                                                      nxs_chat_srv_m_tlgrm_update_t *update,
                                                      nxs_buf_t *                    response_buf);

static void button_create_callback_inline_sendmessage(nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx,
                                                      size_t                              sess_id,
                                                      callback_type_t                     type,
                                                      size_t                              object_id,
                                                      size_t                              pos_y,
                                                      size_t                              pos_x,
                                                      const u_char *                      caption,
                                                      ...);
static void button_create_callback_inline_editmessagetext(nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx,
                                                          size_t                                  sess_id,
                                                          callback_type_t                         type,
                                                          size_t                                  object_id,
                                                          size_t                                  pos_y,
                                                          size_t                                  pos_x,
                                                          const u_char *                          caption,
                                                          ...);

static void callback_serialize(callback_t callback, nxs_string_t *callback_str);
static nxs_chat_srv_err_t callback_deserialize(callback_t *callback, nxs_string_t *callback_str);

// clang-format off

/* Module initializations */

static u_char		_s_no_entry_sign[]		= {0xE2, 0x9C, 0x85, 0x0};
static u_char		_s_exclamation[]		= {0xE2, 0x9D, 0x97, 0x0};

static nxs_string_t	_s_msg_begin			= nxs_string("Как поступить с Вашим комментарием? Добавить в последнюю активную задачу, в другую существующую или создать новую?\n---------\nПоследняя активная задача: #44938 - *Установка и "
									"настройка SpamAssassin*");
static nxs_string_t	_s_msg_specify_subject		= nxs_string("Как назвать задачу?");
static nxs_string_t	_s_msg_error			= nxs_string("%s Произошла внутрення ошибка сервера, мы уже получили уведомление об этом и работаем над решением проблемы. Пожалуйста, [воспользуйтесь web-версией системы задач](https://task.nixys.ru/) или повторите свой запрос позже.");
static nxs_string_t	_s_msg_wrong_action		= nxs_string("Не верное действие. Вам необходимо либо следовать указаниям мастера, либо Вы можете отменить текущий диалог.");
static nxs_string_t	_s_msg_session_destroyed	= nxs_string("Предыдущий диалог завершён.");
static nxs_string_t	_s_msg_description_changed	= nxs_string("Описание изменено.");
static nxs_string_t	_s_msg_specify_description	= nxs_string("Как изменить описание задачи?");
static nxs_string_t	_s_msg_issue_created		= nxs_string("Новая задача [#47983](https://task.nixys.ru/issues/47983) создана, в ближайшее время наши сотрудники займутся ей. Благодарим за обращение!");
static nxs_string_t	_s_msg_added_to_issue		= nxs_string("Ваш комментарий отправлен в задачу [#%zu](https://task.nixys.ru/issues/%zu). Благодарим за обращение!");
static nxs_string_t	_s_msg_empty_subject		= nxs_string("_Будет задана при создании задачи_");

static nxs_string_t	_s_par_s_id			= nxs_string("s_id");
static nxs_string_t	_s_par_t			= nxs_string("t");
static nxs_string_t	_s_par_o_id			= nxs_string("o_id");

static callback_handlers_t callback_handlers[] =
{
	{NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE,		&callback_handler_sess_type_message},
	{NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE,		&callback_handler_sess_type_new_issue},

	{NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE,		NULL}
};

static message_handlers_t message_handlers[] =
{
	{NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE,		&message_handler_sess_type_message},
	{NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE,		&message_handler_sess_type_new_issue},

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
	size_t                        sess_id, user_id;

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

	if(nxs_chat_srv_u_db_sess_pull(sess_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(check_user(sess_ctx, users_ctx, &update, &sess_id, &user_id) == NXS_NO) {

		/* User not found */

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(update.callback_query._is_used == NXS_YES) {

		/* callback query */

		nxs_chat_srv_u_tlgrm_answercallbackquery_push(&update.callback_query.id, NULL, 0);

		nxs_error(rc, callback_handler_exec(&update, sess_ctx, users_ctx, sess_id, user_id, queue_cache), error);
	}
	else {

		nxs_error(rc, message_handler_exec(&update, sess_ctx, users_ctx, sess_id, user_id, queue_cache), error);
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
                             size_t *                       sess_id,
                             size_t *                       user_id)
{
	nxs_chat_srv_m_db_users_t m_user;

	*sess_id = 0;
	*user_id = 0;

	if(update->callback_query._is_used == NXS_YES) {

		*user_id = update->callback_query.from.id;
	}
	else {

		*user_id = update->message.from.id;
	}

	if(nxs_chat_srv_u_db_users_find_by_tid(users_ctx, *user_id, &m_user) == NXS_CHAT_SRV_E_OK) {

		*sess_id = nxs_chat_srv_u_db_sess_get_id(sess_ctx, *user_id);

		return NXS_YES;
	}

	return NXS_NO;
}

static nxs_chat_srv_err_t callback_handler_exec(nxs_chat_srv_m_tlgrm_update_t *update,
                                                nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                nxs_chat_srv_u_db_users_t *    users_ctx,
                                                size_t                         sess_id,
                                                size_t                         user_id,
                                                nxs_chat_srv_u_queue_cache_t * queue_cache)
{
	size_t                        i;
	nxs_chat_srv_m_db_sess_type_t type;
	callback_t                    callback;
	nxs_chat_srv_err_t            rc;

	rc = NXS_CHAT_SRV_E_OK;

	if(sess_id == 0) {

		/* user has no sessions, skipping request */
	}
	else {

		if(callback_deserialize(&callback, &update->callback_query.data)) {

			comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		type = nxs_chat_srv_u_db_sess_get_type(sess_ctx, sess_id);

		for(i = 0; callback_handlers[i].type != NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE; i++) {

			if(callback_handlers[i].type == type) {

				if(callback_handlers[i].handler != NULL) {

					nxs_error(rc,
					          callback_handlers[i].handler(
					                  &callback, update, sess_ctx, users_ctx, sess_id, user_id, queue_cache),
					          error);
				}
				else {

					nxs_error(rc, NXS_CHAT_SRV_E_OK, error);
				}
			}
		}

		if(callback_handlers[i].handler != NULL) {

			nxs_error(rc,
			          callback_handlers[i].handler(&callback, update, sess_ctx, users_ctx, sess_id, user_id, queue_cache),
			          error);
		}
	}

error:

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.callback_handler_exec' (session id: %zu, user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   sess_id,
		                   user_id,
		                   rc);
	}

	return rc;
}

static nxs_chat_srv_err_t message_handler_exec(nxs_chat_srv_m_tlgrm_update_t *update,
                                               nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                               nxs_chat_srv_u_db_users_t *    users_ctx,
                                               size_t                         sess_id,
                                               size_t                         user_id,
                                               nxs_chat_srv_u_queue_cache_t * queue_cache)
{
	size_t                         i;
	nxs_chat_srv_m_db_sess_type_t  type;
	nxs_chat_srv_m_tlgrm_message_t response_message;
	nxs_bool_t                     response_status;
	nxs_buf_t                      response_buf;
	nxs_chat_srv_err_t             rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_chat_srv_c_tlgrm_message_init(&response_message);
	nxs_buf_init(&response_buf, 1);

	if(update->message.reply_to_message != NULL) {

		/* reply to message */
	}
	else {

		/* empty reply */

		if(sess_id == 0) {

			/* user has no sessions, creating new one and starting dialog */

			if(nxs_chat_srv_u_db_sess_start(sess_ctx,
			                                &sess_id,
			                                update->message.from.id,
			                                0,
			                                0,
			                                NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE,
			                                NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE) != NXS_CHAT_SRV_E_OK) {

				comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_u_db_sess_t_set_message(sess_ctx, sess_id, &update->message.text) != NXS_CHAT_SRV_E_OK) {

				comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(comment_begin(sess_ctx, sess_id, update->message.chat.id, 0, update, &response_buf) != NXS_CHAT_SRV_E_OK) {

				comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_c_tlgrm_message_result_pull_json(&response_message, &response_status, &response_buf) !=
			   NXS_CHAT_SRV_E_OK) {

				comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_u_db_sess_set_ids(sess_ctx, sess_id, response_message.chat.id, response_message.message_id) !=
			   NXS_CHAT_SRV_E_OK) {

				comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}
		}
		else {

			type = nxs_chat_srv_u_db_sess_get_type(sess_ctx, sess_id);

			for(i = 0; message_handlers[i].type != NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE; i++) {

				if(message_handlers[i].type == type) {

					if(message_handlers[i].handler != NULL) {

						nxs_error(rc,
						          message_handlers[i].handler(
						                  update, sess_ctx, users_ctx, sess_id, user_id, queue_cache),
						          error);
					}
					else {

						nxs_error(rc, NXS_CHAT_SRV_E_OK, error);
					}
				}
			}

			if(message_handlers[i].handler != NULL) {

				nxs_error(
				        rc, message_handlers[i].handler(update, sess_ctx, users_ctx, sess_id, user_id, queue_cache), error);
			}
		}
	}

error:

	nxs_buf_free(&response_buf);

	nxs_chat_srv_c_tlgrm_message_free(&response_message);

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.message_handler_exec' (session id: %zu, user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   sess_id,
		                   user_id,
		                   rc);
	}

	return rc;
}

static nxs_chat_srv_err_t callback_handler_sess_type_message(callback_t *                   callback,
                                                             nxs_chat_srv_m_tlgrm_update_t *update,
                                                             nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                             nxs_chat_srv_u_db_users_t *    users_ctx,
                                                             size_t                         sess_id,
                                                             size_t                         user_id,
                                                             nxs_chat_srv_u_queue_cache_t * queue_cache)
{
	size_t                    chat_id, message_id, project_id;
	nxs_chat_srv_m_db_users_t user;
	nxs_string_t              project_name;
	nxs_chat_srv_err_t        rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&project_name);

	if(sess_id != callback->sess_id) {

		nxs_log_write_warn(&process,
		                   "[%s]: wrong callback sess_id (session id: %zu, callback session id: %zu, "
		                   "user id: %zu)",
		                   nxs_proc_get_name(&process),
		                   sess_id,
		                   callback->sess_id,
		                   user_id,
		                   rc);

		comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}
	else {

		chat_id    = nxs_chat_srv_u_db_sess_get_chat_id(sess_ctx, sess_id);
		message_id = nxs_chat_srv_u_db_sess_get_message_id(sess_ctx, sess_id);

		switch(callback->type) {

			case CALLBACK_TYPE_NEW_ISSUE:

				/* new issue creation process */

				if(nxs_chat_srv_u_db_users_find_by_tid(users_ctx, user_id, &user) != NXS_CHAT_SRV_E_OK) {

					nxs_log_write_warn(&process,
					                   "[%s]: can't find user in users ctx (session id: %zu, callback type: %d, "
					                   "user id: %zu)",
					                   nxs_proc_get_name(&process),
					                   sess_id,
					                   callback->type,
					                   user_id,
					                   rc);

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				nxs_chat_srv_u_queue_cache_get_user_project_first(
				        queue_cache, user.rdmn_userid, &project_id, &project_name);

				if(nxs_chat_srv_u_db_sess_t_conv_to_new_issue(
				           sess_ctx, sess_id, project_id, &project_name, 0, &tmp_priority[1], NULL, NULL, NULL) !=
				   NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(comment_new_issue(sess_ctx, sess_id, chat_id, message_id, update, NULL) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				break;

			case CALLBACK_TYPE_TO_ISSUE:

				/* new issue creation process */

				if(comment_to_issue(sess_ctx, sess_id, chat_id, 0, update, NULL, callback->object_id) !=
				   NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				nxs_chat_srv_u_db_sess_destroy(sess_ctx, sess_id);

				break;

			case CALLBACK_TYPE_SESSION_DESTROY:

				nxs_chat_srv_u_db_sess_destroy(sess_ctx, sess_id);

				if(comment_session_destroyed(sess_ctx, 0, update->callback_query.message.chat.id, update, NULL) !=
				   NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				break;

			default:

				nxs_log_write_warn(&process,
				                   "[%s]: wrong callback type (session id: %zu, callback type: %d, "
				                   "user id: %zu)",
				                   nxs_proc_get_name(&process),
				                   sess_id,
				                   callback->type,
				                   user_id,
				                   rc);

				comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);

				break;
		}
	}

error:

	nxs_string_free(&project_name);

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.callback_handler_sess_type_message' (session id: %zu, "
		                   "user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   sess_id,
		                   user_id,
		                   rc);
	}

	return rc;
}

static nxs_chat_srv_err_t callback_handler_sess_type_new_issue(callback_t *                   callback,
                                                               nxs_chat_srv_m_tlgrm_update_t *update,
                                                               nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                               nxs_chat_srv_u_db_users_t *    users_ctx,
                                                               size_t                         sess_id,
                                                               size_t                         user_id,
                                                               nxs_chat_srv_u_queue_cache_t * queue_cache)
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

	if(sess_id != callback->sess_id) {

		nxs_log_write_warn(&process,
		                   "[%s]: wrong callback sess_id (session id: %zu, callback session id: %zu, "
		                   "user id: %zu)",
		                   nxs_proc_get_name(&process),
		                   sess_id,
		                   callback->sess_id,
		                   user_id,
		                   rc);

		comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}
	else {

		chat_id    = nxs_chat_srv_u_db_sess_get_chat_id(sess_ctx, sess_id);
		message_id = nxs_chat_srv_u_db_sess_get_message_id(sess_ctx, sess_id);

		switch(callback->type) {

			case CALLBACK_TYPE_CREATE_ISSUE:

				if(nxs_chat_srv_u_db_sess_set_wait_for(
				           sess_ctx, sess_id, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_SUBJECT) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(comment_enter_subject(sess_ctx, sess_id, chat_id, 0, update, NULL) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				break;

			case CALLBACK_TYPE_SELECT_PROJECT:

				if(nxs_chat_srv_u_db_sess_get_wait_for(sess_ctx, sess_id) !=
				   NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_PROJECTNAME_REGEX) {

					/* skipping set 'wait for' if it already in correct value */

					if(nxs_chat_srv_u_db_sess_set_wait_for(
					           sess_ctx, sess_id, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_PROJECTNAME_REGEX) !=
					   NXS_CHAT_SRV_E_OK) {

						comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

						nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
					}
				}

				if(nxs_chat_srv_u_db_users_find_by_tid(users_ctx, user_id, &user) != NXS_CHAT_SRV_E_OK) {

					nxs_log_write_warn(&process,
					                   "[%s]: can't find user in users ctx (session id: %zu, callback type: %d, "
					                   "user id: %zu)",
					                   nxs_proc_get_name(&process),
					                   sess_id,
					                   callback->type,
					                   user_id,
					                   rc);

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
				                                          sess_id,
				                                          &project_id,
				                                          &project_name,
				                                          &priority_id,
				                                          &priority_name,
				                                          &subject,
				                                          &description,
				                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(nxs_chat_srv_u_queue_cache_get_user_projects(queue_cache,
				                                                user.rdmn_userid,
				                                                &projects,
				                                                &project_name_regex,
				                                                callback->object_id,
				                                                PROJECTS_LIMIT,
				                                                &projects_count) != NXS_CHAT_SRV_E_OK) {

					nxs_log_write_warn(&process,
					                   "[%s]: can't find user projects in cache (session id: %zu, callback type: %d, "
					                   "user id: %zu)",
					                   nxs_proc_get_name(&process),
					                   sess_id,
					                   callback->type,
					                   user_id,
					                   rc);

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(comment_select_project(sess_ctx,
				                          sess_id,
				                          chat_id,
				                          message_id,
				                          update,
				                          &projects,
				                          callback->object_id,
				                          projects_count,
				                          NULL) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				break;

			case CALLBACK_TYPE_SELECTED_PROJECT:

				if(nxs_chat_srv_u_db_users_find_by_tid(users_ctx, user_id, &user) != NXS_CHAT_SRV_E_OK) {

					nxs_log_write_warn(&process,
					                   "[%s]: can't find user in users ctx (session id: %zu, callback type: %d, "
					                   "user id: %zu)",
					                   nxs_proc_get_name(&process),
					                   sess_id,
					                   callback->type,
					                   user_id,
					                   rc);

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
				                                          sess_id,
				                                          &project_id,
				                                          &project_name,
				                                          &priority_id,
				                                          &priority_name,
				                                          &subject,
				                                          &description,
				                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				nxs_chat_srv_u_queue_cache_get_user_project_by_id(
				        queue_cache, user.rdmn_userid, callback->object_id, &project_name);

				if(nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx,
				                                          sess_id,
				                                          callback->object_id,
				                                          &project_name,
				                                          priority_id,
				                                          &priority_name,
				                                          &subject,
				                                          &description,
				                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(comment_new_issue(sess_ctx, sess_id, chat_id, message_id, update, NULL) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				break;

			case CALLBACK_TYPE_SELECT_PRIORITY:

				if(comment_select_priority(sess_ctx, sess_id, chat_id, message_id, update, NULL) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				break;

			case CALLBACK_TYPE_SELECTED_PRIORITY:

				if(nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
				                                          sess_id,
				                                          &project_id,
				                                          &project_name,
				                                          &priority_id,
				                                          &priority_name,
				                                          &subject,
				                                          &description,
				                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx,
				                                          sess_id,
				                                          project_id,
				                                          &project_name,
				                                          callback->object_id,
				                                          &tmp_priority[callback->object_id],
				                                          &subject,
				                                          &description,
				                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(comment_new_issue(sess_ctx, sess_id, chat_id, message_id, update, NULL) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				break;

			case CALLBACK_TYPE_CHANGE_DESCRIPTION:

				if(nxs_chat_srv_u_db_sess_set_wait_for(
				           sess_ctx, sess_id, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_DESCRIPTION) !=
				   NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(comment_enter_description(sess_ctx, sess_id, chat_id, 0, update, NULL) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				break;

			case CALLBACK_TYPE_SESSION_DESTROY:

				nxs_chat_srv_u_db_sess_destroy(sess_ctx, sess_id);

				if(comment_session_destroyed(sess_ctx, 0, update->callback_query.message.chat.id, update, NULL) !=
				   NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				break;

			case CALLBACK_TYPE_BACK:

				/*nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_create_issue(
				        sess_ctx, sess_id);*/

				if(nxs_chat_srv_u_db_sess_t_conv_to_message(sess_ctx, sess_id, NULL) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(comment_begin(sess_ctx, sess_id, chat_id, message_id, update, NULL) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				break;

			default:

				nxs_log_write_warn(&process,
				                   "[%s]: wrong callback type (session id: %zu, callback type: %d, "
				                   "user id: %zu)",
				                   nxs_proc_get_name(&process),
				                   sess_id,
				                   callback->type,
				                   user_id,
				                   rc);

				comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);

				break;
		}
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
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.callback_handler_sess_type_new_issue' (session id: %zu, "
		                   "user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   sess_id,
		                   user_id,
		                   rc);
	}

	return rc;
}

static nxs_chat_srv_err_t message_handler_sess_type_message(nxs_chat_srv_m_tlgrm_update_t *update,
                                                            nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                            nxs_chat_srv_u_db_users_t *    users_ctx,
                                                            size_t                         sess_id,
                                                            size_t                         user_id,
                                                            nxs_chat_srv_u_queue_cache_t * queue_cache)
{
	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	switch(nxs_chat_srv_u_db_sess_get_wait_for(sess_ctx, sess_id)) {

		default:

			nxs_error(rc, comment_wrong_action(sess_ctx, sess_id, update->message.chat.id, update, NULL), error);

			break;
	}

error:

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.message_handler_sess_type_message' (session id: %zu, "
		                   "user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   sess_id,
		                   user_id,
		                   rc);
	}

	return rc;
}

/* TODO: сейчас происходит двойное получение информации данных из сессии в этой функции и функциях comment_*, необходимо это устранить */
static nxs_chat_srv_err_t message_handler_sess_type_new_issue(nxs_chat_srv_m_tlgrm_update_t *update,
                                                              nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                              nxs_chat_srv_u_db_users_t *    users_ctx,
                                                              size_t                         sess_id,
                                                              size_t                         user_id,
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

	chat_id    = nxs_chat_srv_u_db_sess_get_chat_id(sess_ctx, sess_id);
	message_id = nxs_chat_srv_u_db_sess_get_message_id(sess_ctx, sess_id);

	switch(nxs_chat_srv_u_db_sess_get_wait_for(sess_ctx, sess_id)) {

		case NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_SUBJECT:

			/* set subject process */

			if(nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
			                                          sess_id,
			                                          &project_id,
			                                          &project_name,
			                                          &priority_id,
			                                          &priority_name,
			                                          &subject,
			                                          &description,
			                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

				comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_u_db_sess_set_wait_for(sess_ctx, sess_id, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE) !=
			   NXS_CHAT_SRV_E_OK) {

				comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_string_cmp(&subject, 0, &update->message.text, 0) != NXS_YES) {

				/* if subject is changed */

				if(nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx,
				                                          sess_id,
				                                          project_id,
				                                          &project_name,
				                                          priority_id,
				                                          &priority_name,
				                                          &update->message.text,
				                                          &description,
				                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(comment_new_issue(sess_ctx, sess_id, chat_id, message_id, update, NULL) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}
			}

			if(comment_issue_created(sess_ctx, sess_id, chat_id, 0, update, NULL) != NXS_CHAT_SRV_E_OK) {

				comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			nxs_chat_srv_u_db_sess_destroy(sess_ctx, sess_id);

			break;

		case NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_DESCRIPTION:

			/* set description process */

			if(nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
			                                          sess_id,
			                                          &project_id,
			                                          &project_name,
			                                          &priority_id,
			                                          &priority_name,
			                                          &subject,
			                                          &description,
			                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

				comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_u_db_sess_set_wait_for(sess_ctx, sess_id, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE) !=
			   NXS_CHAT_SRV_E_OK) {

				comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_string_cmp(&description, 0, &update->message.text, 0) != NXS_YES) {

				/* if description is changed */

				if(nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx,
				                                          sess_id,
				                                          project_id,
				                                          &project_name,
				                                          priority_id,
				                                          &priority_name,
				                                          &subject,
				                                          &update->message.text,
				                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(comment_new_issue(sess_ctx, sess_id, chat_id, message_id, update, NULL) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}
			}

			if(comment_description_changed(sess_ctx, sess_id, chat_id, update, NULL) != NXS_CHAT_SRV_E_OK) {

				comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		case NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_PROJECTNAME_REGEX:

			/* set description process */

			if(nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
			                                          sess_id,
			                                          &project_id,
			                                          &project_name,
			                                          &priority_id,
			                                          &priority_name,
			                                          &subject,
			                                          &description,
			                                          &project_name_regex) != NXS_CHAT_SRV_E_OK) {

				comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_string_cmp(&project_name_regex, 0, &update->message.text, 0) != NXS_YES) {

				/* if project name regex is changed */

				if(nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx,
				                                          sess_id,
				                                          project_id,
				                                          &project_name,
				                                          priority_id,
				                                          &priority_name,
				                                          &subject,
				                                          &description,
				                                          &update->message.text) != NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(nxs_chat_srv_u_db_users_find_by_tid(users_ctx, user_id, &user) != NXS_CHAT_SRV_E_OK) {

					nxs_log_write_warn(&process,
					                   "[%s]: can't find user in users ctx (session id: %zu, "
					                   "user id: %zu)",
					                   nxs_proc_get_name(&process),
					                   sess_id,
					                   user_id,
					                   rc);

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(nxs_chat_srv_u_queue_cache_get_user_projects(queue_cache,
				                                                user.rdmn_userid,
				                                                &projects,
				                                                &update->message.text,
				                                                0,
				                                                PROJECTS_LIMIT,
				                                                &projects_count) != NXS_CHAT_SRV_E_OK) {

					nxs_log_write_warn(&process,
					                   "[%s]: can't find user projects in cache (session id: %zu, "
					                   "user id: %zu)",
					                   nxs_proc_get_name(&process),
					                   sess_id,
					                   user_id,
					                   rc);

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				if(comment_select_project(
				           sess_ctx, sess_id, chat_id, message_id, update, &projects, 0, projects_count, NULL) !=
				   NXS_CHAT_SRV_E_OK) {

					comment_error(sess_ctx, sess_id, update->callback_query.message.chat.id, update, NULL);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}
			}

			break;

		default:

			nxs_error(rc, comment_wrong_action(sess_ctx, sess_id, update->message.chat.id, update, NULL), error);

			break;
	}

error:

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: error in 'proc.queue-worker.tlgrm-update.message_handler_sess_type_new_issue' (session id: %zu, "
		                   "user id: %zu, "
		                   "error code: %d)",
		                   nxs_proc_get_name(&process),
		                   sess_id,
		                   user_id,
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

static nxs_chat_srv_err_t comment_begin(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                        size_t                         sess_id,
                                        size_t                         chat_id,
                                        size_t                         message_id,
                                        nxs_chat_srv_m_tlgrm_update_t *update,
                                        nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *    tlgrm_sendmessage_ctx;
	nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx;
	nxs_string_t                            callback_str;
	nxs_buf_t *                             b;
	nxs_chat_srv_err_t                      rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);

	tlgrm_sendmessage_ctx     = nxs_chat_srv_u_tlgrm_sendmessage_init();
	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

	if(message_id == 0) {

		/* create new comment */

		nxs_chat_srv_u_tlgrm_sendmessage_add(
		        tlgrm_sendmessage_ctx, chat_id, &_s_msg_begin, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

		button_create_callback_inline_sendmessage(
		        tlgrm_sendmessage_ctx, sess_id, CALLBACK_TYPE_TO_ISSUE, 44938, 0, 0, BUTTON_ADD_LAST_ACTIVE);

		button_create_callback_inline_sendmessage(
		        tlgrm_sendmessage_ctx, sess_id, CALLBACK_TYPE_NEW_ISSUE, 0, 0, 1, BUTTON_NEW_ISSUE);

		button_create_callback_inline_sendmessage(
		        tlgrm_sendmessage_ctx,
		        sess_id,
		        CALLBACK_TYPE_TO_ISSUE,
		        46470,
		        1,
		        0,
		        BUTTON_ADD_ADD_TO_ISSUE,
		        46470,
		        "Подключение услуг к серверу 148.251.68.145 в соответствии с выбранным тарифным планом Nixys Standard Plus");

		button_create_callback_inline_sendmessage(tlgrm_sendmessage_ctx,
		                                          sess_id,
		                                          CALLBACK_TYPE_TO_ISSUE,
		                                          46653,
		                                          2,
		                                          0,
		                                          BUTTON_ADD_ADD_TO_ISSUE,
		                                          46653,
		                                          "Monitoring Alert: Spamassassin is not running on Seolinks-s1");

		// nxs_chat_srv_u_tlgrm_sendmessage_force_reply_set(tlgrm_sendmessage_ctx, 0);

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
		        tlgrm_editmessagetext_ctx, chat_id, message_id, &_s_msg_begin, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

		button_create_callback_inline_editmessagetext(
		        tlgrm_editmessagetext_ctx, sess_id, CALLBACK_TYPE_TO_ISSUE, 44938, 0, 0, BUTTON_ADD_LAST_ACTIVE);

		button_create_callback_inline_editmessagetext(
		        tlgrm_editmessagetext_ctx, sess_id, CALLBACK_TYPE_NEW_ISSUE, 0, 0, 1, BUTTON_NEW_ISSUE);

		button_create_callback_inline_editmessagetext(
		        tlgrm_editmessagetext_ctx,
		        sess_id,
		        CALLBACK_TYPE_TO_ISSUE,
		        46470,
		        1,
		        0,
		        BUTTON_ADD_ADD_TO_ISSUE,
		        46470,
		        "Подключение услуг к серверу 148.251.68.145 в соответствии с выбранным тарифным планом Nixys Standard Plus");

		button_create_callback_inline_editmessagetext(tlgrm_editmessagetext_ctx,
		                                              sess_id,
		                                              CALLBACK_TYPE_TO_ISSUE,
		                                              46653,
		                                              2,
		                                              0,
		                                              BUTTON_ADD_ADD_TO_ISSUE,
		                                              46653,
		                                              "Monitoring Alert: Spamassassin is not running on Seolinks-s1");

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

	tlgrm_sendmessage_ctx     = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);
	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_free(tlgrm_editmessagetext_ctx);

	return rc;
}

static nxs_chat_srv_err_t comment_new_issue(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                            size_t                         sess_id,
                                            size_t                         chat_id,
                                            size_t                         message_id,
                                            nxs_chat_srv_m_tlgrm_update_t *update,
                                            nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx;
	nxs_string_t       callback_str, description, subject, project_name, priority_name, project_name_regex, message;
	size_t             priority_id, project_id;
	nxs_buf_t *        b;
	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);
	nxs_string_init(&description);
	nxs_string_init(&subject);
	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&project_name_regex);
	nxs_string_init(&message);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

	if(message_id == 0) {

		/* create new comment */
	}
	else {

		/* update existing comment */

		nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
		                                       sess_id,
		                                       &project_id,
		                                       &project_name,
		                                       &priority_id,
		                                       &priority_name,
		                                       &subject,
		                                       &description,
		                                       &project_name_regex);

		nxs_string_printf(&message,
		                  MESSAGE_ISSUE,
		                  &project_name,
		                  &priority_name,
		                  nxs_string_len(&subject) > 0 ? &subject : &_s_msg_empty_subject,
		                  &description);

		tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

		nxs_chat_srv_u_tlgrm_editmessagetext_add(
		        tlgrm_editmessagetext_ctx, chat_id, message_id, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

		button_create_callback_inline_editmessagetext(
		        tlgrm_editmessagetext_ctx, sess_id, CALLBACK_TYPE_SELECT_PROJECT, 0, 0, 0, BUTTON_PROJECT);

		button_create_callback_inline_editmessagetext(
		        tlgrm_editmessagetext_ctx, sess_id, CALLBACK_TYPE_SELECT_PRIORITY, 0, 0, 1, BUTTON_PRIORITY);

		button_create_callback_inline_editmessagetext(
		        tlgrm_editmessagetext_ctx, sess_id, CALLBACK_TYPE_CHANGE_DESCRIPTION, 0, 0, 2, BUTTON_DESCRIPTION);

		button_create_callback_inline_editmessagetext(
		        tlgrm_editmessagetext_ctx, sess_id, CALLBACK_TYPE_CREATE_ISSUE, 0, 1, 0, BUTTON_CREATE_ISSUE, _s_no_entry_sign);

		button_create_callback_inline_editmessagetext(tlgrm_editmessagetext_ctx, sess_id, CALLBACK_TYPE_BACK, 0, 2, 0, BUTTON_BACK);

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
	nxs_string_free(&project_name_regex);
	nxs_string_free(&message);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_free(tlgrm_editmessagetext_ctx);

	return rc;
}

static nxs_chat_srv_err_t comment_select_project(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                 size_t                         sess_id,
                                                 size_t                         chat_id,
                                                 size_t                         message_id,
                                                 nxs_chat_srv_m_tlgrm_update_t *update,
                                                 nxs_array_t *                  projects,
                                                 size_t                         offset,
                                                 size_t                         projects_count,
                                                 nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx;
	nxs_string_t                          callback_str, description, subject, project_name, priority_name, project_name_regex, message;
	size_t                                priority_id, project_id;
	nxs_buf_t *                           b;
	nxs_chat_srv_m_queue_cache_project_t *p;
	size_t                                i;
	nxs_chat_srv_err_t                    rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);
	nxs_string_init(&description);
	nxs_string_init(&subject);
	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&project_name_regex);
	nxs_string_init(&message);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

	if(message_id == 0) {

		/* create new comment */
	}
	else {

		/* update existing comment */

		nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
		                                       sess_id,
		                                       &project_id,
		                                       &project_name,
		                                       &priority_id,
		                                       &priority_name,
		                                       &subject,
		                                       &description,
		                                       &project_name_regex);

		nxs_string_printf(&message,
		                  MESSAGE_ISSUE,
		                  &project_name,
		                  &priority_name,
		                  nxs_string_len(&subject) > 0 ? &subject : &_s_msg_empty_subject,
		                  &description);

		tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

		nxs_chat_srv_u_tlgrm_editmessagetext_add(
		        tlgrm_editmessagetext_ctx, chat_id, message_id, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

		for(i = 0; i < nxs_array_count(projects); i++) {

			p = nxs_array_get(projects, i);

			button_create_callback_inline_editmessagetext(
			        tlgrm_editmessagetext_ctx, sess_id, CALLBACK_TYPE_SELECTED_PROJECT, p->id, i, 0, nxs_string_str(p->name));
		}

		if(offset > 0) {

			button_create_callback_inline_editmessagetext(tlgrm_editmessagetext_ctx,
			                                              sess_id,
			                                              CALLBACK_TYPE_SELECT_PROJECT,
			                                              offset - PROJECTS_LIMIT,
			                                              i,
			                                              0,
			                                              BUTTON_BACK);
		}

		if(projects_count > offset + PROJECTS_LIMIT) {

			button_create_callback_inline_editmessagetext(tlgrm_editmessagetext_ctx,
			                                              sess_id,
			                                              CALLBACK_TYPE_SELECT_PROJECT,
			                                              offset + PROJECTS_LIMIT,
			                                              i,
			                                              1,
			                                              BUTTON_FORWARD);
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
	nxs_string_free(&project_name_regex);
	nxs_string_free(&message);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_free(tlgrm_editmessagetext_ctx);

	return rc;
}

static nxs_chat_srv_err_t comment_select_priority(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                  size_t                         sess_id,
                                                  size_t                         chat_id,
                                                  size_t                         message_id,
                                                  nxs_chat_srv_m_tlgrm_update_t *update,
                                                  nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx;
	nxs_string_t       callback_str, description, subject, project_name, priority_name, project_name_regex, message;
	size_t             priority_id, project_id;
	nxs_buf_t *        b;
	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);
	nxs_string_init(&description);
	nxs_string_init(&subject);
	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&project_name_regex);
	nxs_string_init(&message);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

	if(message_id == 0) {

		/* create new comment */
	}
	else {

		/* update existing comment */

		nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
		                                       sess_id,
		                                       &project_id,
		                                       &project_name,
		                                       &priority_id,
		                                       &priority_name,
		                                       &subject,
		                                       &description,
		                                       &project_name_regex);

		nxs_string_printf(&message,
		                  MESSAGE_ISSUE,
		                  &project_name,
		                  &priority_name,
		                  nxs_string_len(&subject) > 0 ? &subject : &_s_msg_empty_subject,
		                  &description);

		tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

		nxs_chat_srv_u_tlgrm_editmessagetext_add(
		        tlgrm_editmessagetext_ctx, chat_id, message_id, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

		button_create_callback_inline_editmessagetext(
		        tlgrm_editmessagetext_ctx, sess_id, CALLBACK_TYPE_SELECTED_PRIORITY, 0, 0, 0, nxs_string_str(&tmp_priority[0]));

		button_create_callback_inline_editmessagetext(
		        tlgrm_editmessagetext_ctx, sess_id, CALLBACK_TYPE_SELECTED_PRIORITY, 1, 1, 0, nxs_string_str(&tmp_priority[1]));

		button_create_callback_inline_editmessagetext(
		        tlgrm_editmessagetext_ctx, sess_id, CALLBACK_TYPE_SELECTED_PRIORITY, 2, 2, 0, nxs_string_str(&tmp_priority[2]));

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
	nxs_string_free(&project_name_regex);
	nxs_string_free(&message);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_free(tlgrm_editmessagetext_ctx);

	return rc;
}

static nxs_chat_srv_err_t comment_enter_subject(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                size_t                         sess_id,
                                                size_t                         chat_id,
                                                size_t                         message_id,
                                                nxs_chat_srv_m_tlgrm_update_t *update,
                                                nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_buf_t *                         b;

	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();

	if(message_id == 0) {

		/* create new comment */

		nxs_chat_srv_u_tlgrm_sendmessage_add(
		        tlgrm_sendmessage_ctx, chat_id, &_s_msg_specify_subject, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

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
	}

error:

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);

	return rc;
}

static nxs_chat_srv_err_t comment_enter_description(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                    size_t                         sess_id,
                                                    size_t                         chat_id,
                                                    size_t                         message_id,
                                                    nxs_chat_srv_m_tlgrm_update_t *update,
                                                    nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_buf_t *                         b;

	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();

	if(message_id == 0) {

		/* create new comment */

		nxs_chat_srv_u_tlgrm_sendmessage_add(
		        tlgrm_sendmessage_ctx, chat_id, &_s_msg_specify_description, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

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
	}

error:

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);

	return rc;
}

static nxs_chat_srv_err_t comment_issue_created(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                size_t                         sess_id,
                                                size_t                         chat_id,
                                                size_t                         message_id,
                                                nxs_chat_srv_m_tlgrm_update_t *update,
                                                nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_buf_t *                         b;

	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();

	if(message_id == 0) {

		/* create new comment */

		nxs_chat_srv_u_tlgrm_sendmessage_add(
		        tlgrm_sendmessage_ctx, chat_id, &_s_msg_issue_created, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

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
	}

error:

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);

	return rc;
}

static nxs_chat_srv_err_t comment_to_issue(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                           size_t                         sess_id,
                                           size_t                         chat_id,
                                           size_t                         message_id,
                                           nxs_chat_srv_m_tlgrm_update_t *update,
                                           nxs_buf_t *                    response_buf,
                                           size_t                         issue_id)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_string_t                        msg;
	nxs_buf_t *                         b;

	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&msg);

	nxs_string_printf(&msg, (char *)nxs_string_str(&_s_msg_added_to_issue), issue_id);

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();

	if(message_id == 0) {

		/* create new comment */

		nxs_chat_srv_u_tlgrm_sendmessage_add(tlgrm_sendmessage_ctx, chat_id, &msg, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

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
	}

error:

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);

	nxs_string_free(&msg);

	return rc;
}

static nxs_chat_srv_err_t comment_error(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                        size_t                         sess_id,
                                        size_t                         chat_id,
                                        nxs_chat_srv_m_tlgrm_update_t *update,
                                        nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_buf_t *                         b;
	nxs_string_t                        s;

	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&s);

	nxs_string_printf(&s, (const char *)nxs_string_str(&_s_msg_error), _s_exclamation);

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();

	nxs_chat_srv_u_tlgrm_sendmessage_add(tlgrm_sendmessage_ctx, chat_id, &s, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

	if(nxs_chat_srv_u_tlgrm_sendmessage_push(tlgrm_sendmessage_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(response_buf != NULL) {

		b = nxs_chat_srv_u_tlgrm_sendmessage_get_response_buf(tlgrm_sendmessage_ctx);

		nxs_buf_clone(response_buf, b);
	}
error:

	nxs_string_free(&s);

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);

	return rc;
}

static nxs_chat_srv_err_t comment_wrong_action(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                               size_t                         sess_id,
                                               size_t                         chat_id,
                                               nxs_chat_srv_m_tlgrm_update_t *update,
                                               nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_buf_t *                         b;

	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();

	nxs_chat_srv_u_tlgrm_sendmessage_add(
	        tlgrm_sendmessage_ctx, chat_id, &_s_msg_wrong_action, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

	button_create_callback_inline_sendmessage(
	        tlgrm_sendmessage_ctx, sess_id, CALLBACK_TYPE_SESSION_DESTROY, 0, 0, 0, BUTTON_DESTROY_SESSION);

	if(nxs_chat_srv_u_tlgrm_sendmessage_push(tlgrm_sendmessage_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(response_buf != NULL) {

		b = nxs_chat_srv_u_tlgrm_sendmessage_get_response_buf(tlgrm_sendmessage_ctx);

		nxs_buf_clone(response_buf, b);
	}
error:

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);

	return rc;
}

static nxs_chat_srv_err_t comment_session_destroyed(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                    size_t                         sess_id,
                                                    size_t                         chat_id,
                                                    nxs_chat_srv_m_tlgrm_update_t *update,
                                                    nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_buf_t *                         b;

	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();

	nxs_chat_srv_u_tlgrm_sendmessage_add(
	        tlgrm_sendmessage_ctx, chat_id, &_s_msg_session_destroyed, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

	if(nxs_chat_srv_u_tlgrm_sendmessage_push(tlgrm_sendmessage_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(response_buf != NULL) {

		b = nxs_chat_srv_u_tlgrm_sendmessage_get_response_buf(tlgrm_sendmessage_ctx);

		nxs_buf_clone(response_buf, b);
	}
error:

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);

	return rc;
}

static nxs_chat_srv_err_t comment_description_changed(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                      size_t                         sess_id,
                                                      size_t                         chat_id,
                                                      nxs_chat_srv_m_tlgrm_update_t *update,
                                                      nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_buf_t *                         b;

	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();

	nxs_chat_srv_u_tlgrm_sendmessage_add(
	        tlgrm_sendmessage_ctx, chat_id, &_s_msg_description_changed, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

	if(nxs_chat_srv_u_tlgrm_sendmessage_push(tlgrm_sendmessage_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(response_buf != NULL) {

		b = nxs_chat_srv_u_tlgrm_sendmessage_get_response_buf(tlgrm_sendmessage_ctx);

		nxs_buf_clone(response_buf, b);
	}
error:

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);

	return rc;
}

/* create callback button for tlgrm_sendmessage context */
static void button_create_callback_inline_sendmessage(nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx,
                                                      size_t                              sess_id,
                                                      callback_type_t                     type,
                                                      size_t                              object_id,
                                                      size_t                              pos_y,
                                                      size_t                              pos_x,
                                                      const u_char *                      caption,
                                                      ...)
{
	va_list      va_caption;
	nxs_string_t b, callback_str;
	callback_t   callback;

	if(caption == NULL) {

		return;
	}

	nxs_string_init(&b);
	nxs_string_init(&callback_str);

	va_start(va_caption, caption);
	nxs_string_vprintf(&b, (const char *)caption, va_caption);
	va_end(va_caption);

	callback.sess_id   = sess_id;
	callback.type      = type;
	callback.object_id = object_id;

	callback_serialize(callback, &callback_str);

	nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_add(tlgrm_sendmessage_ctx, pos_y, pos_x, &b, NULL, &callback_str);

	nxs_string_free(&b);
	nxs_string_free(&callback_str);
}

/* create callback button for tlgrm_editmessagetext context */
static void button_create_callback_inline_editmessagetext(nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx,
                                                          size_t                                  sess_id,
                                                          callback_type_t                         type,
                                                          size_t                                  object_id,
                                                          size_t                                  pos_y,
                                                          size_t                                  pos_x,
                                                          const u_char *                          caption,
                                                          ...)
{
	va_list      va_caption;
	nxs_string_t b, callback_str;
	callback_t   callback;

	if(caption == NULL) {

		return;
	}

	nxs_string_init(&b);
	nxs_string_init(&callback_str);

	va_start(va_caption, caption);
	nxs_string_vprintf(&b, (const char *)caption, va_caption);
	va_end(va_caption);

	callback.sess_id   = sess_id;
	callback.type      = type;
	callback.object_id = object_id;

	callback_serialize(callback, &callback_str);

	nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_add(tlgrm_editmessagetext_ctx, pos_y, pos_x, &b, NULL, &callback_str);

	nxs_string_free(&b);
	nxs_string_free(&callback_str);
}

static void callback_serialize(callback_t callback, nxs_string_t *callback_str)
{

	if(callback_str == NULL) {

		return;
	}

	nxs_string_printf(callback_str,
	                  "{\"%r\":%zu,\"%r\":%d,\"%r\":%zu}",
	                  &_s_par_s_id,
	                  callback.sess_id,
	                  &_s_par_t,
	                  callback.type,
	                  &_s_par_o_id,
	                  callback.object_id);

	nxs_string_escape(callback_str, NULL, NXS_STRING_ESCAPE_TYPE_JSON);
}

static nxs_chat_srv_err_t callback_deserialize(callback_t *callback, nxs_string_t *callback_str)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	callback->sess_id   = 0;
	callback->type      = CALLBACK_TYPE_NONE;
	callback->object_id = 0;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_s_id,	&callback->sess_id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_t,	&callback->type,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_o_id,	&callback->object_id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, (nxs_buf_t *)callback_str) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: tlgrm update rest api error: parse callback data error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}
