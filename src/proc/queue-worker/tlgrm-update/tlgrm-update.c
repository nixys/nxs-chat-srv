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

#define NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_D_MESSAGE_ISSUE	"*Создание новой задачи*\n\n" \
									"*Проект:* Ak-opt%r\n" \
									"*Приоритет:* Нормальный%r\n" \
									"*Тема:* %r\n" \
									"*Описание:* %r\n---------\n" \
									"_Ниже Вы можете изменить параметры новой задачи_"

/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef enum
{
	NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_CALLBACK_TYPE_NONE,
	NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_CALLBACK_TYPE_NEW_ISSUE,		/* create new issue */
	NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_CALLBACK_TYPE_TO_ISSUE,		/* add comment to existing issue */
	NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_CALLBACK_TYPE_SET_SUBJECT,		/* set subject for new issue */
	NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_CALLBACK_TYPE_CREATE_ISSUE,		/* create new issue */
} nxs_chat_srv_p_queue_worker_tlgrm_update_callback_type_t;

typedef struct
{
	size_t								sess_id;
	nxs_chat_srv_p_queue_worker_tlgrm_update_callback_type_t	type;
	size_t								issue_id;
} nxs_chat_srv_p_queue_worker_tlgrm_update_callback_t;

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_bool_t nxs_chat_srv_p_queue_worker_tlgrm_update_check_user(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                                      nxs_chat_srv_u_db_users_t *    users_ctx,
                                                                      nxs_chat_srv_m_tlgrm_update_t *update,
                                                                      size_t *                       sess_id,
                                                                      size_t *                       user_id);

static nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_start(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                                                size_t                         sess_id,
                                                                                nxs_chat_srv_m_tlgrm_update_t *update);
static nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_new_issue(nxs_chat_srv_u_db_sess_t *sess_ctx,
                                                                                    size_t                    sess_id,
                                                                                    size_t                    chat_id,
                                                                                    size_t                    message_id);
static nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_enter_subject(nxs_chat_srv_u_db_sess_t *sess_ctx,
                                                                                        size_t                    sess_id,
                                                                                        size_t                    chat_id,
                                                                                        size_t                    message_id);
static nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_set_subject(nxs_chat_srv_u_db_sess_t *sess_ctx,
                                                                                      size_t                    sess_id,
                                                                                      size_t                    chat_id,
                                                                                      nxs_string_t *            new_subject);
static nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_create_issue(nxs_chat_srv_u_db_sess_t *sess_ctx, size_t sess_id);

static void nxs_chat_srv_p_queue_worker_tlgrm_update_callback_serialize_new_issue(size_t sess_id, nxs_string_t *callback_str);
static void
        nxs_chat_srv_p_queue_worker_tlgrm_update_callback_serialize_to_issue(size_t sess_id, size_t issue_id, nxs_string_t *callback_str);
static void nxs_chat_srv_p_queue_worker_tlgrm_update_callback_serialize_set_subject(size_t sess_id, nxs_string_t *callback_str);
static void nxs_chat_srv_p_queue_worker_tlgrm_update_callback_serialize_create_issue(size_t sess_id, nxs_string_t *callback_str);
static nxs_chat_srv_err_t
        nxs_chat_srv_p_queue_worker_tlgrm_update_callback_deserialize(nxs_chat_srv_p_queue_worker_tlgrm_update_callback_t *callback,
                                                                      nxs_string_t *                                       callback_str);

// clang-format off

/* Module initializations */

//static u_char		_s_no_entry_sign[]		= {0xF0, 0x9F, 0x9A, 0xAB, 0x0};
static u_char		_s_no_entry_sign[]		= {0xE2, 0x9C, 0x85, 0x0};

static nxs_string_t	_s_msg_specify_subject		= nxs_string("Пожалуйста, напишите ниже тему новой задачи");
//static nxs_string_t	_s_msg_subject_specified	= nxs_string("Спасибо! Тема задана, теперь Вы можете создать задачу");
static nxs_string_t	_s_msg_issue_created		= nxs_string("Новая задача [#47983](https://task.nixys.ru/issues/47983) создана, в ближайшее время наши сотрудники займутся ей. Благодарим за обращение!");
static nxs_string_t	_s_msg_empty_subject		= nxs_string("_Будет задана при создании задачи_");

static nxs_string_t	_s_par_sess_id			= nxs_string("sess_id");
static nxs_string_t	_s_par_type			= nxs_string("type");
static nxs_string_t	_s_par_issue_id			= nxs_string("issue_id");

/* Module global functions */

// clang-format on

/* TODO: sample function (junk) */
nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_runtime(nxs_chat_srv_m_queue_com_types_t type, nxs_string_t *data)
{
	nxs_string_t                                        base64_decoded;
	nxs_chat_srv_m_tlgrm_update_t                       update;
	nxs_chat_srv_u_db_users_t *                         users_ctx;
	nxs_chat_srv_u_db_sess_t *                          sess_ctx;
	nxs_chat_srv_err_t                                  rc;
	size_t                                              sess_id, user_id;
	nxs_chat_srv_p_queue_worker_tlgrm_update_callback_t callback;

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

	if(nxs_chat_srv_p_queue_worker_tlgrm_update_check_user(sess_ctx, users_ctx, &update, &sess_id, &user_id) == NXS_NO) {

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

				nxs_chat_srv_p_queue_worker_tlgrm_update_callback_deserialize(&callback, &update.callback_query.data);

				switch(nxs_chat_srv_u_db_sess_get_type(sess_ctx, sess_id)) {

					case NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE:

						if(sess_id != callback.sess_id) {

							/* wrong callback sess_id */
						}
						else {

							switch(callback.type) {

								case NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_CALLBACK_TYPE_NEW_ISSUE:

									/* new issue creation process */

									nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_new_issue(
									        sess_ctx,
									        sess_id,
									        update.callback_query.message.chat.id,
									        update.callback_query.message.message_id);

									break;

								default:

									break;
							}
						}

						break;

					case NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE:

						if(sess_id != callback.sess_id) {

							/* wrong callback sess_id */
						}
						else {

							switch(callback.type) {

								case NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_CALLBACK_TYPE_SET_SUBJECT:

									/* new issue creation process */

									nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_enter_subject(
									        sess_ctx,
									        sess_id,
									        update.callback_query.message.chat.id,
									        update.callback_query.message.message_id);

									break;

								case NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_CALLBACK_TYPE_CREATE_ISSUE:

									/*nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_create_issue(
									        sess_ctx, sess_id);*/

									nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_enter_subject(
									        sess_ctx,
									        sess_id,
									        update.callback_query.message.chat.id,
									        update.callback_query.message.message_id);

									break;

								default:

									break;
							}
						}

						break;

					default:

						break;
				}
			}
		}
		else {

			if(sess_id == 0) {

				/* user has no sessions, creating new one and starting dialog */

				nxs_chat_srv_u_db_sess_start(sess_ctx,
				                             &sess_id,
				                             update.message.from.id,
				                             0,
				                             0,
				                             NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE,
				                             NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE);

				nxs_chat_srv_u_db_sess_t_set_message(sess_ctx, sess_id, &update.message.text);

				nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_start(sess_ctx, sess_id, &update);
			}
			else {

				switch(nxs_chat_srv_u_db_sess_get_type(sess_ctx, sess_id)) {

					case NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE:

						switch(nxs_chat_srv_u_db_sess_get_wait_for(sess_ctx, sess_id)) {

							case NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_SUBJECT:

								/* set subject process */

								nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_set_subject(
								        sess_ctx, sess_id, update.message.chat.id, &update.message.text);

								break;

							default:

								break;
						}

						break;

					default:

						break;
				}
			}
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

static nxs_bool_t nxs_chat_srv_p_queue_worker_tlgrm_update_check_user(nxs_chat_srv_u_db_sess_t *     sess_ctx,
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

static nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_start(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                                                size_t                         sess_id,
                                                                                nxs_chat_srv_m_tlgrm_update_t *update)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_string_t                        callback_str;
	nxs_buf_t *                         response_buf;
	nxs_chat_srv_m_tlgrm_message_t      response_message;
	nxs_bool_t                          response_status;
	nxs_chat_srv_err_t                  rc;

	nxs_string_t posted = nxs_string(
	        "Пожалуйста, укажите как поступить с Вашим комментарием: добавить в последнюю активную задачу, добавить в другую "
	        "существующую или создать новую?\n---------\nПоследняя задача по которой Вы оставляли свой комментарий:\n\n#44938 - "
	        "*Установка и настройка SpamAssassin*");

	nxs_string_t button10 = nxs_string("Добавить в последнюю активную");
	nxs_string_t button11 = nxs_string("Создать новую задачу");
	nxs_string_t button12 = nxs_string(
	        "Добавить в #46470 - Подключение услуг к серверу 148.251.68.145 в соответствии с выбранным тарифным планом Nixys "
	        "Standard "
	        "Plus");
	nxs_string_t button13 = nxs_string("Добавить в #46653 - Monitoring Alert: Spamassassin is not running on Seolinks-s1");

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);

	nxs_chat_srv_c_tlgrm_message_init(&response_message);

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();

	nxs_chat_srv_u_tlgrm_sendmessage_add(
	        tlgrm_sendmessage_ctx, update->message.chat.id, &posted, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

	nxs_chat_srv_p_queue_worker_tlgrm_update_callback_serialize_new_issue(sess_id, &callback_str);
	nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_add(tlgrm_sendmessage_ctx, 0, 0, &button10, NULL, &callback_str);
	nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_add(tlgrm_sendmessage_ctx, 0, 1, &button11, NULL, &callback_str);

	nxs_chat_srv_p_queue_worker_tlgrm_update_callback_serialize_to_issue(sess_id, 46470, &callback_str);
	nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_add(tlgrm_sendmessage_ctx, 1, 0, &button12, NULL, &callback_str);

	nxs_chat_srv_p_queue_worker_tlgrm_update_callback_serialize_to_issue(sess_id, 46653, &callback_str);
	nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_add(tlgrm_sendmessage_ctx, 2, 0, &button13, NULL, &callback_str);

	// nxs_chat_srv_u_tlgrm_sendmessage_force_reply_set(tlgrm_sendmessage_ctx, 0);

	if(nxs_chat_srv_u_tlgrm_sendmessage_push(tlgrm_sendmessage_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	response_buf = nxs_chat_srv_u_tlgrm_sendmessage_get_response_buf(tlgrm_sendmessage_ctx);

	nxs_chat_srv_c_tlgrm_message_result_pull_json(&response_message, &response_status, response_buf);

	nxs_chat_srv_u_db_sess_set_ids(sess_ctx, sess_id, response_message.chat.id, response_message.message_id);

error:

	nxs_chat_srv_c_tlgrm_message_free(&response_message);

	nxs_string_free(&callback_str);

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);

	return rc;
}

static nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_new_issue(nxs_chat_srv_u_db_sess_t *sess_ctx,
                                                                                    size_t                    sess_id,
                                                                                    size_t                    chat_id,
                                                                                    size_t                    message_id)
{
	nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx;
	nxs_string_t                            callback_str, description, subject, project_name, priority_name, message;
	size_t                                  priority_id, project_id;
	nxs_chat_srv_err_t                      rc;

	nxs_string_t button1 = nxs_string("Проект");
	nxs_string_t button2 = nxs_string("Приоритет");
	//	nxs_string_t button3 = nxs_string("Изменить тему");
	nxs_string_t button4 = nxs_string("Описание");
	nxs_string_t button5; //= nxs_string("Создать задачу");
	nxs_string_t button6 = nxs_string("<< Вернуться назад");

	nxs_string_init(&button5);
	nxs_string_printf_dyn(&button5, "Создать задачу %s", _s_no_entry_sign);

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);
	nxs_string_init(&description);
	nxs_string_init(&subject);
	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&message);

	// nxs_chat_srv_u_db_sess_t_get_message(sess_ctx, sess_id, &description);

	nxs_chat_srv_u_db_sess_t_conv_to_issue(sess_ctx, sess_id, 0, NULL, 0, NULL, NULL, NULL);

	nxs_chat_srv_u_db_sess_t_get_new_issue(
	        sess_ctx, sess_id, &project_id, &project_name, &priority_id, &priority_name, &subject, &description);

	nxs_string_printf_dyn(&message,
	                      NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_D_MESSAGE_ISSUE,
	                      &project_name,
	                      &priority_name,
	                      nxs_string_len(&subject) > 0 ? &subject : &_s_msg_empty_subject,
	                      &description);

	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

	nxs_chat_srv_u_tlgrm_editmessagetext_add(
	        tlgrm_editmessagetext_ctx, chat_id, message_id, &message, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

	nxs_chat_srv_p_queue_worker_tlgrm_update_callback_serialize_new_issue(sess_id, &callback_str);
	nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_add(tlgrm_editmessagetext_ctx, 0, 0, &button1, NULL, &callback_str);
	nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_add(tlgrm_editmessagetext_ctx, 0, 1, &button2, NULL, &callback_str);
	nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_add(tlgrm_editmessagetext_ctx, 0, 2, &button4, NULL, &callback_str);

	nxs_chat_srv_p_queue_worker_tlgrm_update_callback_serialize_set_subject(sess_id, &callback_str);
	//	nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_add(tlgrm_editmessagetext_ctx, m_id, 1, 0, &button3, NULL,
	//&callback_str);

	nxs_chat_srv_p_queue_worker_tlgrm_update_callback_serialize_create_issue(sess_id, &callback_str);
	nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_add(tlgrm_editmessagetext_ctx, 1, 0, &button5, NULL, &callback_str);
	nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_add(tlgrm_editmessagetext_ctx, 2, 0, &button6, NULL, &callback_str);

	if(nxs_chat_srv_u_tlgrm_editmessagetext_push(tlgrm_editmessagetext_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
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

static nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_enter_subject(nxs_chat_srv_u_db_sess_t *sess_ctx,
                                                                                        size_t                    sess_id,
                                                                                        size_t                    chat_id,
                                                                                        size_t                    message_id)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_string_t                        callback_str;
	nxs_chat_srv_err_t                  rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();

	nxs_chat_srv_u_db_sess_set_wait_for(sess_ctx, sess_id, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_SUBJECT);

	nxs_chat_srv_u_tlgrm_sendmessage_add(
	        tlgrm_sendmessage_ctx, chat_id, &_s_msg_specify_subject, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

	if(nxs_chat_srv_u_tlgrm_sendmessage_push(tlgrm_sendmessage_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_string_free(&callback_str);

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);

	return rc;
}

static nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_set_subject(nxs_chat_srv_u_db_sess_t *sess_ctx,
                                                                                      size_t                    sess_id,
                                                                                      size_t                    chat_id,
                                                                                      nxs_string_t *            new_subject)
{
	//	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_string_t       callback_str, description, subject, project_name, priority_name, message;
	size_t             priority_id, project_id;
	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);
	nxs_string_init(&description);
	nxs_string_init(&subject);
	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&message);

	//	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();

	nxs_chat_srv_u_db_sess_t_get_new_issue(
	        sess_ctx, sess_id, &project_id, &project_name, &priority_id, &priority_name, &subject, &description);

	nxs_chat_srv_u_db_sess_t_set_new_issue(
	        sess_ctx, sess_id, project_id, &project_name, priority_id, &priority_name, new_subject, &description);

	nxs_chat_srv_u_db_sess_set_wait_for(sess_ctx, sess_id, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE);

	/*	nxs_chat_srv_u_tlgrm_sendmessage_add(
	                tlgrm_sendmessage_ctx, chat_id, &_s_msg_subject_specified, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

	        if(nxs_chat_srv_u_tlgrm_sendmessage_push(tlgrm_sendmessage_ctx) != NXS_CHAT_SRV_E_OK) {

	                nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	        }*/

	nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_new_issue(sess_ctx,
	                                                          sess_id,
	                                                          nxs_chat_srv_u_db_sess_get_chat_id(sess_ctx, sess_id),
	                                                          nxs_chat_srv_u_db_sess_get_message_id(sess_ctx, sess_id));

	nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_create_issue(sess_ctx, sess_id);

	// error:

	nxs_string_free(&callback_str);
	nxs_string_free(&description);
	nxs_string_free(&subject);
	nxs_string_free(&project_name);
	nxs_string_free(&priority_name);
	nxs_string_free(&message);

	//	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);

	return rc;
}

static nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_dialog_create_issue(nxs_chat_srv_u_db_sess_t *sess_ctx, size_t sess_id)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_string_t                        callback_str;
	nxs_chat_srv_err_t                  rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();

	nxs_chat_srv_u_db_sess_set_wait_for(sess_ctx, sess_id, NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_SUBJECT);

	nxs_chat_srv_u_tlgrm_sendmessage_add(tlgrm_sendmessage_ctx,
	                                     nxs_chat_srv_u_db_sess_get_chat_id(sess_ctx, sess_id),
	                                     &_s_msg_issue_created,
	                                     NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

	if(nxs_chat_srv_u_tlgrm_sendmessage_push(tlgrm_sendmessage_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_chat_srv_u_db_sess_destroy(sess_ctx, sess_id);

error:

	nxs_string_free(&callback_str);

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);

	return rc;
}

static void nxs_chat_srv_p_queue_worker_tlgrm_update_callback_serialize_new_issue(size_t sess_id, nxs_string_t *callback_str)
{

	if(callback_str == NULL) {

		return;
	}

	nxs_string_printf_dyn(
	        callback_str, "{\"sess_id\":%zu,\"type\":%d}", sess_id, NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_CALLBACK_TYPE_NEW_ISSUE);

	nxs_string_escape(callback_str, NULL, NXS_STRING_ESCAPE_TYPE_JSON);
}

static void
        nxs_chat_srv_p_queue_worker_tlgrm_update_callback_serialize_to_issue(size_t sess_id, size_t issue_id, nxs_string_t *callback_str)
{

	if(callback_str == NULL) {

		return;
	}

	nxs_string_printf_dyn(callback_str,
	                      "{\"sess_id\":%zu,\"type\":%d,\"issue\":%zu}",
	                      sess_id,
	                      NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_CALLBACK_TYPE_TO_ISSUE,
	                      issue_id);

	nxs_string_escape(callback_str, NULL, NXS_STRING_ESCAPE_TYPE_JSON);
}

static void nxs_chat_srv_p_queue_worker_tlgrm_update_callback_serialize_set_subject(size_t sess_id, nxs_string_t *callback_str)
{

	if(callback_str == NULL) {

		return;
	}

	nxs_string_printf_dyn(
	        callback_str, "{\"sess_id\":%zu,\"type\":%d}", sess_id, NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_CALLBACK_TYPE_SET_SUBJECT);

	nxs_string_escape(callback_str, NULL, NXS_STRING_ESCAPE_TYPE_JSON);
}

static void nxs_chat_srv_p_queue_worker_tlgrm_update_callback_serialize_create_issue(size_t sess_id, nxs_string_t *callback_str)
{

	if(callback_str == NULL) {

		return;
	}

	nxs_string_printf_dyn(callback_str,
	                      "{\"sess_id\":%zu,\"type\":%d}",
	                      sess_id,
	                      NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_CALLBACK_TYPE_CREATE_ISSUE);

	nxs_string_escape(callback_str, NULL, NXS_STRING_ESCAPE_TYPE_JSON);
}

static nxs_chat_srv_err_t
        nxs_chat_srv_p_queue_worker_tlgrm_update_callback_deserialize(nxs_chat_srv_p_queue_worker_tlgrm_update_callback_t *callback,
                                                                      nxs_string_t *                                       callback_str)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	callback->sess_id  = 0;
	callback->type     = NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_CALLBACK_TYPE_NONE;
	callback->issue_id = 0;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_sess_id,	&callback->sess_id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_type,		&callback->type,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_issue_id,	&callback->issue_id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,	0,	0,	NXS_NO,		NULL);

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
