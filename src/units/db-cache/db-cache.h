#ifndef _INCLUDE_NXS_CHAT_SRV_U_DB_CACHE_H
#define _INCLUDE_NXS_CHAT_SRV_U_DB_CACHE_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_db_cache_s;

/* Prototypes */

nxs_chat_srv_u_db_cache_t	*nxs_chat_srv_u_db_cache_init				(void);
nxs_chat_srv_u_db_cache_t	*nxs_chat_srv_u_db_cache_free				(nxs_chat_srv_u_db_cache_t *u_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_u_db_cache_pull				(nxs_chat_srv_u_db_cache_t *u_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_u_db_cache_update_projects			(nxs_chat_srv_u_db_cache_t *u_ctx);
nxs_chat_srv_err_t		nxs_chat_srv_u_db_cache_update_users			(nxs_chat_srv_u_db_cache_t *u_ctx);
nxs_chat_srv_err_t		nxs_chat_srv_u_db_cache_update_issue_priorities		(nxs_chat_srv_u_db_cache_t *u_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_u_db_cache_projects_get			(nxs_chat_srv_u_db_cache_t *u_ctx, nxs_array_t *projects);

nxs_chat_srv_err_t		nxs_chat_srv_u_db_cache_user_get			(nxs_chat_srv_u_db_cache_t *u_ctx, nxs_string_t *tlgrm_username, nxs_chat_srv_m_user_ctx_t *user_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_u_db_cache_prio_get			(nxs_chat_srv_u_db_cache_t *u_ctx, nxs_array_t *priorities);
nxs_chat_srv_err_t		nxs_chat_srv_u_db_cache_prio_get_default		(nxs_chat_srv_u_db_cache_t *u_ctx, nxs_chat_srv_m_db_cache_issue_priority_t *issue_priority);
nxs_chat_srv_err_t		nxs_chat_srv_u_db_cache_prio_get_by_id			(nxs_chat_srv_u_db_cache_t *u_ctx, size_t prio_id, nxs_chat_srv_m_db_cache_issue_priority_t *issue_priority);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_DB_CACHE_H */
