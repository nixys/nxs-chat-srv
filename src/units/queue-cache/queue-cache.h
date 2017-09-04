#ifndef _INCLUDE_NXS_CHAT_SRV_U_QUEUE_CACHE_H
#define _INCLUDE_NXS_CHAT_SRV_U_QUEUE_CACHE_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_queue_cache_s;

/* Prototypes */

nxs_chat_srv_u_queue_cache_t	*nxs_chat_srv_u_queue_cache_init			(void);
nxs_chat_srv_u_queue_cache_t	*nxs_chat_srv_u_queue_cache_free			(nxs_chat_srv_u_queue_cache_t *u_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_u_queue_cache_get_user_projects		(nxs_chat_srv_u_queue_cache_t *u_ctx, size_t user_id, nxs_array_t *projects, nxs_string_t *project_name_regex, size_t offset, size_t limit, size_t *count);
nxs_chat_srv_err_t		nxs_chat_srv_u_queue_cache_get_user_project_first	(nxs_chat_srv_u_queue_cache_t *u_ctx, size_t user_id, size_t *project_id, nxs_string_t *project_name);
nxs_chat_srv_err_t		nxs_chat_srv_u_queue_cache_get_user_project_by_id	(nxs_chat_srv_u_queue_cache_t *u_ctx, size_t user_id, size_t project_id, nxs_string_t *project_name);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_QUEUE_CACHE_H */
