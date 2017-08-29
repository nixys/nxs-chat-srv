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
#define BUTTON_BACK			(u_char *)"<< Вернуться назад"

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
	nxs_chat_srv_err_t			(*handler)(callback_t *callback, nxs_chat_srv_m_tlgrm_update_t *update, nxs_chat_srv_u_db_sess_t *sess_ctx, nxs_chat_srv_u_db_users_t *users_ctx, size_t sess_id, size_t user_id);
} callback_handlers_t;

typedef struct
{
	nxs_chat_srv_m_db_sess_type_t		type;
	nxs_chat_srv_err_t			(*handler)(nxs_chat_srv_m_tlgrm_update_t *update, nxs_chat_srv_u_db_sess_t *sess_ctx, nxs_chat_srv_u_db_users_t *users_ctx, size_t sess_id, size_t user_id);
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
                                                size_t                         user_id);

static nxs_chat_srv_err_t message_handler_exec(nxs_chat_srv_m_tlgrm_update_t *update,
                                               nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                               nxs_chat_srv_u_db_users_t *    users_ctx,
                                               size_t                         sess_id,
                                               size_t                         user_id);

static nxs_chat_srv_err_t callback_handler_sess_type_message(callback_t *                   callback,
                                                             nxs_chat_srv_m_tlgrm_update_t *update,
                                                             nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                             nxs_chat_srv_u_db_users_t *    users_ctx,
                                                             size_t                         sess_id,
                                                             size_t                         user_id);
static nxs_chat_srv_err_t callback_handler_sess_type_new_issue(callback_t *                   callback,
                                                               nxs_chat_srv_m_tlgrm_update_t *update,
                                                               nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                               nxs_chat_srv_u_db_users_t *    users_ctx,
                                                               size_t                         sess_id,
                                                               size_t                         user_id);

static nxs_chat_srv_err_t message_handler_sess_type_new_issue(nxs_chat_srv_m_tlgrm_update_t *update,
                                                              nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                              nxs_chat_srv_u_db_users_t *    users_ctx,
                                                              size_t                         sess_id,
                                                              size_t                         user_id);

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

static nxs_string_t	_s_msg_begin			= nxs_string("Пожалуйста, укажите как поступить с Вашим комментарием: добавить "
									"в последнюю активную задачу, добавить в другую существующую или "
									"создать новую?\n---------\nПоследняя задача по которой "
									"Вы оставляли свой комментарий:\n\n#44938 - *Установка и "
									"настройка SpamAssassin*");
static nxs_string_t	_s_msg_specify_subject		= nxs_string("Пожалуйста, напишите ниже тему новой задачи");
static nxs_string_t	_s_msg_specify_description	= nxs_string("Пожалуйста, напишите ниже новое описание задачи");
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
	{NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE,		NULL},
	{NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE,		&message_handler_sess_type_new_issue},

	{NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE,		NULL}
};

/* TODO: replace this by real redmine projects array (used by dev purposes) */
static nxs_string_t tmp_projects[] =
{
	nxs_string("Project-s1"),
	nxs_string("Project-s2"),
	nxs_string("Project-s3"),
	nxs_string("Project-s4"),
	nxs_string("Project-s5"),

	nxs_string("")
};
static nxs_string_t tmp_priority[] =
{
	nxs_string("Низкий"),
	nxs_string("Нормальный"),
	nxs_string("Высокий"),

	nxs_string("")
};

/* Module global functions */

// clang-format on

/* TODO: sample function (junk) */
nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_runtime(nxs_chat_srv_m_queue_com_types_t type, nxs_string_t *data)
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

	if(update.message.reply_to_message != NULL) {

		/* reply to message */
	}
	else {

		/* empty reply */

		if(update.callback_query._is_used == NXS_YES) {

			/* callback request */

			if(sess_id == 0) {

				/* user has no sessions */
			}
			else {

				callback_handler_exec(&update, sess_ctx, users_ctx, sess_id, user_id);
			}
		}
		else {

			message_handler_exec(&update, sess_ctx, users_ctx, sess_id, user_id);
		}
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
                                                size_t                         user_id)
{
	size_t                        i;
	nxs_chat_srv_m_db_sess_type_t type;
	callback_t                    callback;

	callback_deserialize(&callback, &update->callback_query.data);

	type = nxs_chat_srv_u_db_sess_get_type(sess_ctx, sess_id);

	for(i = 0; callback_handlers[i].type != NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE; i++) {

		if(callback_handlers[i].type == type) {

			if(callback_handlers[i].handler != NULL) {

				return callback_handlers[i].handler(&callback, update, sess_ctx, users_ctx, sess_id, user_id);
			}
			else {

				return NXS_CHAT_SRV_E_OK;
			}
		}
	}

	if(callback_handlers[i].handler != NULL) {

		return callback_handlers[i].handler(&callback, update, sess_ctx, users_ctx, sess_id, user_id);
	}

	return NXS_CHAT_SRV_E_OK;
}

static nxs_chat_srv_err_t message_handler_exec(nxs_chat_srv_m_tlgrm_update_t *update,
                                               nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                               nxs_chat_srv_u_db_users_t *    users_ctx,
                                               size_t                         sess_id,
                                               size_t                         user_id)
{
	size_t                         i;
	nxs_chat_srv_m_db_sess_type_t  type;
	nxs_chat_srv_m_tlgrm_message_t response_message;
	nxs_bool_t                     response_status;
	nxs_buf_t                      response_buf;

	nxs_chat_srv_c_tlgrm_message_init(&response_message);
	nxs_buf_init(&response_buf, 1);

	if(sess_id == 0) {

		/* user has no sessions, creating new one and starting dialog */

		nxs_chat_srv_u_db_sess_start(sess_ctx,
		                             &sess_id,
		                             update->message.from.id,
		                             0,
		                             0,
		                             NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE,
		                             NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE);

		nxs_chat_srv_u_db_sess_t_set_message(sess_ctx, sess_id, &update->message.text);

		comment_begin(sess_ctx, sess_id, update->message.chat.id, 0, update, &response_buf);

		nxs_chat_srv_c_tlgrm_message_result_pull_json(&response_message, &response_status, &response_buf);

		nxs_chat_srv_u_db_sess_set_ids(sess_ctx, sess_id, response_message.chat.id, response_message.message_id);

		return NXS_CHAT_SRV_E_OK;
	}

	type = nxs_chat_srv_u_db_sess_get_type(sess_ctx, sess_id);

	for(i = 0; message_handlers[i].type != NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE; i++) {

		if(message_handlers[i].type == type) {

			if(message_handlers[i].handler != NULL) {

				return message_handlers[i].handler(update, sess_ctx, users_ctx, sess_id, user_id);
			}
			else {

				return NXS_CHAT_SRV_E_OK;
			}
		}
	}

	if(message_handlers[i].handler != NULL) {

		return message_handlers[i].handler(update, sess_ctx, users_ctx, sess_id, user_id);
	}

	nxs_buf_free(&response_buf);

	nxs_chat_srv_c_tlgrm_message_free(&response_message);

	return NXS_CHAT_SRV_E_OK;
}

static nxs_chat_srv_err_t callback_handler_sess_type_message(callback_t *                   callback,
                                                             nxs_chat_srv_m_tlgrm_update_t *update,
                                                             nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                             nxs_chat_srv_u_db_users_t *    users_ctx,
                                                             size_t                         sess_id,
                                                             size_t                         user_id)
{
	size_t chat_id, message_id;

	if(sess_id != callback->sess_id) {

		/* wrong callback sess_id */
	}
	else {

		chat_id    = nxs_chat_srv_u_db_sess_get_chat_id(sess_ctx, sess_id);
		message_id = nxs_chat_srv_u_db_sess_get_message_id(sess_ctx, sess_id);

		switch(callback->type) {

			case CALLBACK_TYPE_NEW_ISSUE:

				/* new issue creation process */

				nxs_chat_srv_u_db_sess_t_conv_to_new_issue(
				        sess_ctx, sess_id, 0, &tmp_projects[0], 0, &tmp_priority[1], NULL, NULL);

				comment_new_issue(sess_ctx, sess_id, chat_id, message_id, update, NULL);

				break;

			case CALLBACK_TYPE_TO_ISSUE:

				/* new issue creation process */

				comment_to_issue(sess_ctx, sess_id, chat_id, 0, update, NULL, callback->object_id);

				nxs_chat_srv_u_db_sess_destroy(sess_ctx, sess_id);

				break;

			default:

				break;
		}
	}

	return NXS_CHAT_SRV_E_OK;
}

static nxs_chat_srv_err_t callback_handler_sess_type_new_issue(callback_t *                   callback,
                                                               nxs_chat_srv_m_tlgrm_update_t *update,
                                                               nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                               nxs_chat_srv_u_db_users_t *    users_ctx,
                                                               size_t                         sess_id,
                                                               size_t                         user_id)
{
	nxs_string_t callback_str, description, subject, project_name, priority_name, message;
	size_t       priority_id, project_id, chat_id, message_id;

	nxs_string_init(&callback_str);
	nxs_string_init(&description);
	nxs_string_init(&subject);
	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&message);

	if(sess_id != callback->sess_id) {

		/* wrong callback sess_id */
	}
	else {

		chat_id    = nxs_chat_srv_u_db_sess_get_chat_id(sess_ctx, sess_id);
		message_id = nxs_chat_srv_u_db_sess_get_message_id(sess_ctx, sess_id);

		switch(callback->type) {

			case CALLBACK_TYPE_CREATE_ISSUE:

				/*nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_create_issue(
				        sess_ctx, sess_id);*/

				nxs_chat_srv_u_db_sess_set_wait_for(sess_ctx, sess_id, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_SUBJECT);

				comment_enter_subject(sess_ctx, sess_id, chat_id, 0, update, NULL);

				break;

			case CALLBACK_TYPE_SELECT_PROJECT:

				comment_select_project(sess_ctx, sess_id, chat_id, message_id, update, NULL);

				break;

			case CALLBACK_TYPE_SELECTED_PROJECT:

				nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
				                                       sess_id,
				                                       &project_id,
				                                       &project_name,
				                                       &priority_id,
				                                       &priority_name,
				                                       &subject,
				                                       &description);

				nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx,
				                                       sess_id,
				                                       callback->sess_id,
				                                       &tmp_projects[callback->object_id],
				                                       priority_id,
				                                       &priority_name,
				                                       &subject,
				                                       &description);

				comment_new_issue(sess_ctx, sess_id, chat_id, message_id, update, NULL);

				break;

			case CALLBACK_TYPE_SELECT_PRIORITY:

				comment_select_priority(sess_ctx, sess_id, chat_id, message_id, update, NULL);

				break;

			case CALLBACK_TYPE_SELECTED_PRIORITY:

				nxs_chat_srv_u_db_sess_t_get_new_issue(sess_ctx,
				                                       sess_id,
				                                       &project_id,
				                                       &project_name,
				                                       &priority_id,
				                                       &priority_name,
				                                       &subject,
				                                       &description);

				nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx,
				                                       sess_id,
				                                       project_id,
				                                       &project_name,
				                                       callback->object_id,
				                                       &tmp_priority[callback->object_id],
				                                       &subject,
				                                       &description);

				comment_new_issue(sess_ctx, sess_id, chat_id, message_id, update, NULL);

				break;

			case CALLBACK_TYPE_CHANGE_DESCRIPTION:

				nxs_chat_srv_u_db_sess_set_wait_for(
				        sess_ctx, sess_id, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_DESCRIPTION);

				comment_enter_description(sess_ctx, sess_id, chat_id, 0, update, NULL);

				break;

			case CALLBACK_TYPE_BACK:

				/*nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_create_issue(
				        sess_ctx, sess_id);*/

				nxs_chat_srv_u_db_sess_t_conv_to_message(sess_ctx, sess_id, NULL);

				comment_begin(sess_ctx, sess_id, chat_id, message_id, update, NULL);

				break;

			default:

				break;
		}
	}

	nxs_string_free(&callback_str);
	nxs_string_free(&description);
	nxs_string_free(&subject);
	nxs_string_free(&project_name);
	nxs_string_free(&priority_name);
	nxs_string_free(&message);

	return NXS_CHAT_SRV_E_OK;
}

static nxs_chat_srv_err_t message_handler_sess_type_new_issue(nxs_chat_srv_m_tlgrm_update_t *update,
                                                              nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                              nxs_chat_srv_u_db_users_t *    users_ctx,
                                                              size_t                         sess_id,
                                                              size_t                         user_id)
{
	nxs_string_t callback_str, description, subject, project_name, priority_name, message;
	size_t       priority_id, project_id, chat_id, message_id;

	nxs_string_init(&callback_str);
	nxs_string_init(&description);
	nxs_string_init(&subject);
	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&message);

	chat_id    = nxs_chat_srv_u_db_sess_get_chat_id(sess_ctx, sess_id);
	message_id = nxs_chat_srv_u_db_sess_get_message_id(sess_ctx, sess_id);

	switch(nxs_chat_srv_u_db_sess_get_wait_for(sess_ctx, sess_id)) {

		case NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_SUBJECT:

			/* set subject process */

			nxs_chat_srv_u_db_sess_t_get_new_issue(
			        sess_ctx, sess_id, &project_id, &project_name, &priority_id, &priority_name, &subject, &description);

			nxs_chat_srv_u_db_sess_t_set_new_issue(sess_ctx,
			                                       sess_id,
			                                       project_id,
			                                       &project_name,
			                                       priority_id,
			                                       &priority_name,
			                                       &update->message.text,
			                                       &description);

			nxs_chat_srv_u_db_sess_set_wait_for(sess_ctx, sess_id, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE);

			comment_new_issue(sess_ctx, sess_id, chat_id, message_id, update, NULL);

			comment_issue_created(sess_ctx, sess_id, chat_id, 0, update, NULL);

			nxs_chat_srv_u_db_sess_destroy(sess_ctx, sess_id);

			break;

		case NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_DESCRIPTION:

			/* set description process */

			nxs_chat_srv_u_db_sess_t_get_new_issue(
			        sess_ctx, sess_id, &project_id, &project_name, &priority_id, &priority_name, &subject, &description);

			nxs_chat_srv_u_db_sess_t_set_new_issue(
			        sess_ctx, sess_id, project_id, &project_name, priority_id, &priority_name, &subject, &update->message.text);

			nxs_chat_srv_u_db_sess_set_wait_for(sess_ctx, sess_id, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE);

			comment_new_issue(sess_ctx, sess_id, chat_id, message_id, update, NULL);

			break;

		default:

			break;
	}

	nxs_string_free(&callback_str);
	nxs_string_free(&description);
	nxs_string_free(&subject);
	nxs_string_free(&project_name);
	nxs_string_free(&priority_name);
	nxs_string_free(&message);

	return NXS_CHAT_SRV_E_OK;
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
	nxs_string_t                            callback_str, description, subject, project_name, priority_name, message;
	size_t                                  priority_id, project_id;
	nxs_buf_t *                             b;
	nxs_chat_srv_err_t                      rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);
	nxs_string_init(&description);
	nxs_string_init(&subject);
	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&message);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

	if(message_id == 0) {

		/* create new comment */
	}
	else {

		/* update existing comment */

		nxs_chat_srv_u_db_sess_t_get_new_issue(
		        sess_ctx, sess_id, &project_id, &project_name, &priority_id, &priority_name, &subject, &description);

		nxs_string_printf_dyn(&message,
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
	nxs_string_free(&message);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_free(tlgrm_editmessagetext_ctx);

	return rc;
}

static nxs_chat_srv_err_t comment_select_project(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                 size_t                         sess_id,
                                                 size_t                         chat_id,
                                                 size_t                         message_id,
                                                 nxs_chat_srv_m_tlgrm_update_t *update,
                                                 nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx;
	nxs_string_t                            callback_str, description, subject, project_name, priority_name, message;
	size_t                                  priority_id, project_id;
	nxs_buf_t *                             b;
	nxs_chat_srv_err_t                      rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);
	nxs_string_init(&description);
	nxs_string_init(&subject);
	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&message);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

	if(message_id == 0) {

		/* create new comment */
	}
	else {

		/* update existing comment */

		nxs_chat_srv_u_db_sess_t_get_new_issue(
		        sess_ctx, sess_id, &project_id, &project_name, &priority_id, &priority_name, &subject, &description);

		nxs_string_printf_dyn(&message,
		                      MESSAGE_ISSUE,
		                      &project_name,
		                      &priority_name,
		                      nxs_string_len(&subject) > 0 ? &subject : &_s_msg_empty_subject,
		                      &description);

		tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

		nxs_chat_srv_u_tlgrm_editmessagetext_add(
		        tlgrm_editmessagetext_ctx, chat_id, message_id, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

		button_create_callback_inline_editmessagetext(
		        tlgrm_editmessagetext_ctx, sess_id, CALLBACK_TYPE_SELECTED_PROJECT, 0, 0, 0, nxs_string_str(&tmp_projects[0]));

		button_create_callback_inline_editmessagetext(
		        tlgrm_editmessagetext_ctx, sess_id, CALLBACK_TYPE_SELECTED_PROJECT, 1, 1, 0, nxs_string_str(&tmp_projects[1]));

		button_create_callback_inline_editmessagetext(
		        tlgrm_editmessagetext_ctx, sess_id, CALLBACK_TYPE_SELECTED_PROJECT, 2, 2, 0, nxs_string_str(&tmp_projects[2]));

		button_create_callback_inline_editmessagetext(
		        tlgrm_editmessagetext_ctx, sess_id, CALLBACK_TYPE_SELECTED_PROJECT, 3, 3, 0, nxs_string_str(&tmp_projects[3]));

		button_create_callback_inline_editmessagetext(
		        tlgrm_editmessagetext_ctx, sess_id, CALLBACK_TYPE_SELECTED_PROJECT, 4, 4, 0, nxs_string_str(&tmp_projects[4]));

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
	nxs_string_t                            callback_str, description, subject, project_name, priority_name, message;
	size_t                                  priority_id, project_id;
	nxs_buf_t *                             b;
	nxs_chat_srv_err_t                      rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);
	nxs_string_init(&description);
	nxs_string_init(&subject);
	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&message);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

	if(message_id == 0) {

		/* create new comment */
	}
	else {

		/* update existing comment */

		nxs_chat_srv_u_db_sess_t_get_new_issue(
		        sess_ctx, sess_id, &project_id, &project_name, &priority_id, &priority_name, &subject, &description);

		nxs_string_printf_dyn(&message,
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

	nxs_string_printf_dyn(&msg, (char *)nxs_string_str(&_s_msg_added_to_issue), issue_id);

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
	nxs_string_vprintf_dyn(&b, (const char *)caption, va_caption);
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
	nxs_string_vprintf_dyn(&b, (const char *)caption, va_caption);
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

	nxs_string_printf_dyn(callback_str, "{\"s_id\":%zu,\"t\":%d,\"o_id\":%zu}", callback.sess_id, callback.type, callback.object_id);

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
