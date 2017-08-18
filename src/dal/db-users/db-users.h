#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_USERS_H
#define _INCLUDE_NXS_CHAT_SRV_D_DB_USERS_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_d_db_users_s;

/* Prototypes */

nxs_chat_srv_d_db_users_t	*nxs_chat_srv_d_db_users_init		(void);
nxs_chat_srv_d_db_users_t	*nxs_chat_srv_d_db_users_free		(nxs_chat_srv_d_db_users_t *d_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_d_db_users_select		(nxs_chat_srv_d_db_users_t *d_ctx, nxs_mysql_t *mysql_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_d_db_users_get		(nxs_chat_srv_d_db_users_t *d_ctx, size_t i, nxs_chat_srv_m_db_users_t *user);

#endif /* _INCLUDE_NXS_CHAT_SRV_D_DB_USERS_H */
