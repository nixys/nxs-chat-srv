#ifndef _INCLUDE_NXS_CHAT_SRV_CORE_H
#define _INCLUDE_NXS_CHAT_SRV_CORE_H

// clang-format off

/* Project version */
#define NXS_CHAT_SRV_VERSION				"0.0.1"

/* Project basic global definitions */

#define	NXS_CHAT_SRV_CONF_PATH_DEFAULT			"./nxs-chat-srv.conf"

#define NXS_CHAT_SRV_REST_API_MAX_HEADER_SIZE		1048576				/* 1 MB */

/* Project global typedefs */



/* Project global structures */

typedef enum
{
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_NONE,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_MESSAGE,
	NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_EDIT_MESSAGE_TEXT
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
	nxs_string_t				api_key;
	nxs_string_t				auth_token;
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
	nxs_chat_srv_cfg_proc_t			proc;
	nxs_chat_srv_cfg_log_t			log;
	nxs_chat_srv_cfg_bind_t			bind;
	nxs_chat_srv_cfg_tlgrm_t		tlgrm;
	nxs_chat_srv_cfg_mysql_t		mysql;
	nxs_chat_srv_cfg_rdmn_t			rdmn;
	nxs_chat_srv_cfg_queue_worker_t		queue_worker;
} nxs_chat_srv_cfg_t;

/* Project includes */



/* Global functions for project's core module */



#endif /* _INCLUDE_NXS_CHAT_SRV_CORE_H */
