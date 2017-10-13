#ifndef _INCLUDE_NXS_CHAT_SRV_CORE_H
#define _INCLUDE_NXS_CHAT_SRV_CORE_H

// clang-format off

#include <hiredis/hiredis.h>

/* Project version */
#define NXS_CHAT_SRV_VERSION						"0.0.6"

/* Project basic global definitions */

#define	NXS_CHAT_SRV_CONF_PATH_DEFAULT					"/etc/nxs-chat-srv/nxs-chat-srv.conf"

#define NXS_CHAT_SRV_REST_API_MAX_HEADER_SIZE				1048576		/* 1 MB */

#define NXS_CHAT_SRV_TLGRM_MAX_MESSAGE_SIZE				3584		/* 3.5 K */

#define NXS_CHAT_SRV_RDMN_QUERY_LIMIT					100

#define NXS_CHAT_SRV_TLGRM_PROJECTS_LIMIT				5
#define NXS_CHAT_SRV_TLGRM_ISSUES_LIMIT					5

#define NXS_CHAT_SRV_UTF8_PRIVATE_MESSAGE				0xF0, 0x9F, 0x94, 0x95, 0x0
#define NXS_CHAT_SRV_UTF8_ENTRY_SIGN					0xE2, 0x9C, 0x85, 0x0
#define NXS_CHAT_SRV_UTF8_EXCLAMATION					0xE2, 0x9D, 0x97, 0x0

#define NXS_CHAT_SRV_TLGRM_MESSAGE_CMD_DESC_HELP			"справка по использованию бота"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_CMD_DESC_DIALOGDESTROY		"завершить текущий диалог"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_CMD_DESC_ISSUECREATE			"создать новую задачу"

#define NXS_CHAT_SRV_TLGRM_MESSAGE_HELP					"...Справка по использованию бота...\n\nОписание команд:\n\n%r\n"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_DUMMY				"На текущий момент данная команда не реализована"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_HELLO				"%r, добро пожаловать в чат поддержки Nixys!\nТут будет краткая инструкция по использованию ..."
#define NXS_CHAT_SRV_TLGRM_MESSAGE_ISSUE_FULL				"<b>Создание новой задачи</b>\n\n" \
									"<b>Проект:</b> %r\n" \
									"<b>Приоритет:</b> %r\n" \
									"<b>Тема:</b> %r\n" \
									"%r" \
									"<b>Описание:</b> %r\n---------\n" \
									"<i>Ниже Вы можете изменить параметры новой задачи</i>"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_ISSUE_SHORT				"<b>Создание новой задачи</b>\n\n" \
									"<b>Проект:</b> %r\n" \
									"<b>Приоритет:</b> %r\n" \
									"<b>Тема:</b> %r\n" \
									"%r" \
									"<b>Описание:</b> %r\n"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_BEGIN				"Как поступить с Вашим комментарием? Добавить в последнюю задачу, выбрать задачу из списка или создать новую?%r\n---------\nПоследняя задача: <a href=\"%r/issues/%zu\">[%r - #%zu] %r</a>"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_SPECIFY_SUBJECT			"Напишите название задачи?"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_EMPTY_SUBJECT			"<i>Будет задана при создании задачи</i>"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_ERROR				"%s Произошла внутрення ошибка сервера, мы уже получили уведомление об этом и работаем над решением проблемы. Пожалуйста, <a href=\"%r\">воспользуйтесь web-версией системы задач</a> или повторите свой запрос позже."
#define NXS_CHAT_SRV_TLGRM_MESSAGE_WRONG_ACTION				"Не верное действие. Вам необходимо либо следовать указаниям мастера, либо Вы можете завершить текущий диалог"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_WRONG_REPLY				"К сожалению, не удалось определить номер задачи в которую необходимо отправить Ваше сообщение"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_SESSION_DESTROYED			"Предыдущий диалог завершён"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_DESCRIPTION_CHANGED			"Описание изменено"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_SPECIFY_DESCRIPTION			"Напишите новое описание задачи? Все новые файлы будут добавлены к уже существующим"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_ISSUE_CREATED			"Задача <a href=\"%r/issues/%zu\">[%r - #%zu] %r</a> создана, в ближайшее время наши сотрудники займутся ей. Благодарим за обращение!"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_ADDED_TO_ISSUE			"Ваш комментарий отправлен в задачу <a href=\"%r/issues/%zu\">#%zu</a>"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_SELECT_ISSUE				"Пожалуйста, выберите задачу в которую необходимо отправить Ваш комментарий"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_ISSUE_PRIVACY			"<b>Приватная задача %s</b>\n"
#define NXS_CHAT_SRV_TLGRM_MESSAGE_ADD_NOTE_EXT				"Для добавления комментария необходимо выбрать одно из следующих действий"

#define NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_CREATED				"Создана новая задача <a href=\"%r/issues/%zu\">[%r - #%zu] %r</a>\n\n%r<b>Автор:</b> %r\n<b>Статус:</b> %r\n<b>Приоритет:</b> %r\n<b>Назначена:</b> %r\n\n<b>Описание</b>:\n---\n"
#define NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_UPDATED				"%s<a href=\"%r/issues/%zu\">[%r - #%zu] %r</a>\n\n%r<b>%r писал(а):</b>\n---\n"
#define NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_UPDATED_NO_MESSAGE		"%s<a href=\"%r/issues/%zu\">[%r - #%zu] %r</a>\n\n%r<b>%r</b>"
#define NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_TOO_BIG				"<i>Сообщение слишком велико, пожалуйста, ознакомтесь с ним в web-интерфейсе системы задач</i>"

#define NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_CREATED_IS_PRIVATE		"<b>Частная:</b> да %s\n"
#define NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_UPDATED_IS_PRIVATE_YES		"<b>Частная:</b> да\n"
#define NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_UPDATED_IS_PRIVATE_NO		"<b>Частная:</b> нет\n"
#define NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_UPDATED_STATUS			"<b>Статус:</b> %r\n"
#define NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_UPDATED_PRIORITY		"<b>Приоритет:</b> %r\n"
#define NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_UPDATED_ASSIGNED_TO		"<b>Назначена:</b> %r\n"

#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_ADD_LAST_ACTIVE		(u_char *)"Добавить в последнюю задачу"
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_NEW_ISSUE			(u_char *)"Создать новую задачу"
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_SELECT_ISSUE			(u_char *)"Выбрать задачу"
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_ADD_ADD_TO_ISSUE		(u_char *)"Добавить в #%zu - %s"
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_PROJECT			(u_char *)"Проект"
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_PRIORITY			(u_char *)"Приоритет"
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_DESCRIPTION			(u_char *)"Описание"
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_CREATE_ISSUE			(u_char *)"Создать задачу %s"
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_BACK				(u_char *)"<< Назад"
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_FORWARD			(u_char *)"Вперёд >>"
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_DESTROY_SESSION		(u_char *)"Завершить текущий диалог"
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_CANCEL			(u_char *)"Отмена"
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_TO_STATUS_IN_PROGRESS		(u_char *)"Перевести в статус \"В работе\""
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_TO_STATUS_NEED_FEEDBACK	(u_char *)"Перевести в статус \"Ожидание клиента\""
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_TO_STATUS_RESOLVED		(u_char *)"Перевести в статус \"Решена\""
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_TO_PRIVATE_NOTE		(u_char *)"Сделать комментарий париватным"
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_IGNORE_STATUS			(u_char *)"Игнорировать ошибки статуса"
#define NXS_CHAT_SRV_TLGRM_BUTTON_CAPTION_TAKE_ISSUE			(u_char *)"Взять задачу в работу"

/* Project global typedefs */



/* Project global structures */

typedef enum
{
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_NONE,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_MESSAGE,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_EDIT_MESSAGE_TEXT,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_ANSWER_CALLBACK_QUERY,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_GET_FILE,
} nxs_chat_srv_tlgrm_request_type_t;

/* Project errors */
typedef enum
{
	NXS_CHAT_SRV_E_OK,
	NXS_CHAT_SRV_E_WARN,
	NXS_CHAT_SRV_E_ERR,
	NXS_CHAT_SRV_E_PTR,
	NXS_CHAT_SRV_E_INIT,
	NXS_CHAT_SRV_E_EXIT,
	NXS_CHAT_SRV_E_EXIST,
	NXS_CHAT_SRV_E_TIMEOUT,
	NXS_CHAT_SRV_E_TYPE,
	NXS_CHAT_SRV_E_ATTR,
} nxs_chat_srv_err_t;

/* Project config structures */

typedef struct
{
	nxs_string_t				cfg_path;
} nxs_chat_srv_cfg_ctx_t;

typedef struct
{
	nxs_string_t				pid_file;
	size_t					queue_worker_term_timeout;
	size_t					rest_api_term_timeout;
	nxs_bool_t				daemonize;
} nxs_chat_srv_cfg_proc_t;

typedef struct
{
	nxs_string_t				path;
	int					level;
} nxs_chat_srv_cfg_log_t;

typedef struct
{
	nxs_string_t				crt;
	nxs_string_t				key;
} nxs_chat_srv_cfg_bind_ssl_t;

typedef struct
{
	nxs_string_t				iface;
	uint16_t				port;
	nxs_chat_srv_cfg_bind_ssl_t		ssl;
} nxs_chat_srv_cfg_bind_t;

typedef struct
{
	nxs_string_t				bot_api_addr;
	nxs_string_t				bot_api_key;
	nxs_string_t				auth_token;
} nxs_chat_srv_cfg_tlgrm_t;

typedef struct
{
	nxs_string_t				name;
	nxs_string_t				user;
	nxs_string_t				pass;
	nxs_string_t				host;
	uint16_t				port;
} nxs_chat_srv_cfg_mysql_t;

typedef struct
{
	nxs_string_t				host;
	uint16_t				port;
} nxs_chat_srv_cfg_redis_t;

typedef struct
{
	nxs_string_t				host;
	nxs_string_t				api_key;
	nxs_string_t				auth_token;
	size_t					issue_list_query_id;
	nxs_bool_t				ssl_verifyhost;
	size_t					cf_watchers;
	size_t					cf_ignore_status;
	size_t					status_in_progress;
	size_t					status_resolved;
	size_t					status_need_feedback;
} nxs_chat_srv_cfg_rdmn_t;

typedef struct
{
	nxs_string_t				sock_path;
	nxs_string_t				sock_user;
	nxs_string_t				sock_group;
	mode_t					sock_mode;
	size_t					sock_max_conn;
} nxs_chat_srv_cfg_queue_worker_t;

typedef struct
{
	size_t					rdmn_cf_telegram;
	time_t					projects_ttl;
	time_t					users_ttl;
	time_t					issue_priorities_ttl;
	nxs_string_t				lock_file;
} nxs_chat_srv_cfg_cache_t;

typedef struct
{
	nxs_string_t				tlgrm_download_tmp_dir;
} nxs_chat_srv_cfg_attachments_t;

typedef struct
{
	nxs_chat_srv_cfg_proc_t			proc;
	nxs_chat_srv_cfg_log_t			log;
	nxs_chat_srv_cfg_bind_t			bind;
	nxs_chat_srv_cfg_tlgrm_t		tlgrm;
	nxs_chat_srv_cfg_mysql_t		mysql;
	nxs_chat_srv_cfg_redis_t		redis;
	nxs_chat_srv_cfg_rdmn_t			rdmn;
	nxs_chat_srv_cfg_queue_worker_t		queue_worker;
	nxs_chat_srv_cfg_cache_t		cache;
	nxs_array_t				dev_accounts;
	nxs_chat_srv_cfg_attachments_t		attachments;
} nxs_chat_srv_cfg_t;

/* Project includes */



/* Global functions for project's core module */



#endif /* _INCLUDE_NXS_CHAT_SRV_CORE_H */
