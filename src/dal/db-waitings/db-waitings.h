#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_WAITINGS_H
#define _INCLUDE_NXS_CHAT_SRV_D_DB_WAITINGS_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_d_db_waitings_s;

/* Prototypes */

nxs_chat_srv_d_db_waitings_t		*nxs_chat_srv_d_db_waitings_init	(void);
nxs_chat_srv_d_db_waitings_t		*nxs_chat_srv_d_db_waitings_free	(nxs_chat_srv_d_db_waitings_t *d_ctx);

nxs_chat_srv_err_t			nxs_chat_srv_d_db_waitings_pull		(nxs_chat_srv_d_db_waitings_t *d_ctx);
nxs_chat_srv_err_t			nxs_chat_srv_d_db_waitings_set		(nxs_chat_srv_d_db_waitings_t *d_ctx, size_t tlgrm_userid, nxs_string_t *update);
nxs_string_t				*nxs_chat_srv_d_db_waitings_get		(nxs_chat_srv_d_db_waitings_t *d_ctx, size_t i, size_t *tlgrm_userid);
nxs_chat_srv_err_t			nxs_chat_srv_d_db_waitings_del		(nxs_chat_srv_d_db_waitings_t *d_ctx, size_t tlgrm_userid);


#endif /* _INCLUDE_NXS_CHAT_SRV_D_DB_WAITINGS_H */
