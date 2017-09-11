#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_SESS_H
#define _INCLUDE_NXS_CHAT_SRV_D_DB_SESS_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_d_db_sess_s;

/* Prototypes */

nxs_chat_srv_d_db_sess_t	*nxs_chat_srv_d_db_sess_init		(void);
nxs_chat_srv_d_db_sess_t	*nxs_chat_srv_d_db_sess_free		(nxs_chat_srv_d_db_sess_t *d_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_d_db_sess_get		(nxs_chat_srv_d_db_sess_t *d_ctx, size_t tlgrm_userid, nxs_string_t *value);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_sess_put		(nxs_chat_srv_d_db_sess_t *d_ctx, size_t tlgrm_userid, nxs_string_t *value);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_sess_del		(nxs_chat_srv_d_db_sess_t *d_ctx, size_t tlgrm_userid);

#endif /* _INCLUDE_NXS_CHAT_SRV_D_DB_SESS_H */
