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
} nxs_chat_srv_err_t;

/* Project config structures */

typedef struct
{
	nxs_string_t				cfg_path;
} nxs_chat_srv_cfg_ctx_t;

typedef struct
{
	nxs_string_t				path;
	int					level;
} nxs_chat_srv_cfg_log_t;

typedef struct
{
	nxs_string_t				iface;
	uint16_t				port;
} nxs_chat_srv_cfg_bind_t;

typedef struct
{
	nxs_string_t				crt;
	nxs_string_t				key;
} nxs_chat_srv_cfg_ssl_t;

typedef struct
{
	nxs_chat_srv_cfg_log_t			log;
	nxs_chat_srv_cfg_bind_t			bind;
	nxs_chat_srv_cfg_ssl_t			ssl;
} nxs_chat_srv_cfg_t;

/* Project includes */



/* Global functions for project's core module */



#endif /* _INCLUDE_NXS_CHAT_SRV_CORE_H */
