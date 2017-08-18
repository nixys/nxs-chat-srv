#ifndef _INCLUDE_NXS_CHAT_SRV_U_DB_USERS_H
#define _INCLUDE_NXS_CHAT_SRV_U_DB_USERS_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_db_users_s;

/* Prototypes */

nxs_chat_srv_u_db_users_t	*nxs_chat_srv_u_db_users_init				(void);
nxs_chat_srv_u_db_users_t	*nxs_chat_srv_u_db_users_free				(nxs_chat_srv_u_db_users_t *u_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_u_db_users_pull				(nxs_chat_srv_u_db_users_t *u_ctx);
nxs_chat_srv_err_t		nxs_chat_srv_u_db_users_find_by_tid			(nxs_chat_srv_u_db_users_t *u_ctx, size_t tlgrm_id, nxs_chat_srv_m_db_users_t *user);
nxs_chat_srv_err_t		nxs_chat_srv_u_db_users_find_by_tusername		(nxs_chat_srv_u_db_users_t *u_ctx, nxs_string_t *tlgrm_username, nxs_chat_srv_m_db_users_t *user);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_DB_USERS_H */
