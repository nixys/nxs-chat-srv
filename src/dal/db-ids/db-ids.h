#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_IDS_H
#define _INCLUDE_NXS_CHAT_SRV_D_DB_IDS_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_d_db_ids_s;

/* Prototypes */

nxs_chat_srv_d_db_ids_t		*nxs_chat_srv_d_db_ids_init			(void);
nxs_chat_srv_d_db_ids_t		*nxs_chat_srv_d_db_ids_free			(nxs_chat_srv_d_db_ids_t *d_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_d_db_ids_get			(nxs_chat_srv_d_db_ids_t *d_ctx, size_t rdmn_userid, nxs_string_t *value);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_ids_put			(nxs_chat_srv_d_db_ids_t *d_ctx, size_t rdmn_userid, nxs_string_t *value);

#endif /* _INCLUDE_NXS_CHAT_SRV_D_DB_IDS_H */
