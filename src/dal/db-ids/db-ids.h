#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_IDS_H
#define _INCLUDE_NXS_CHAT_SRV_D_DB_IDS_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_d_db_ids_s;

/* Prototypes */

nxs_chat_srv_d_db_ids_t		*nxs_chat_srv_d_db_ids_init					(void);
nxs_chat_srv_d_db_ids_t		*nxs_chat_srv_d_db_ids_free					(nxs_chat_srv_d_db_ids_t *d_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_d_db_ids_transaction_start				(nxs_chat_srv_d_db_ids_t *d_ctx);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_ids_transaction_commit			(nxs_chat_srv_d_db_ids_t *d_ctx);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_ids_transaction_rollback			(nxs_chat_srv_d_db_ids_t *d_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_d_db_ids_get					(nxs_chat_srv_d_db_ids_t *d_ctx, size_t rdmn_userid, size_t *tlgrm_userid);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_ids_put					(nxs_chat_srv_d_db_ids_t *d_ctx, size_t rdmn_userid, size_t tlgrm_userid);

nxs_chat_srv_err_t		nxs_chat_srv_d_db_ids_cleanup_tlgrm_username			(nxs_chat_srv_d_db_ids_t *d_ctx, size_t tlgrm_userid, nxs_string_t *tlgrm_username);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_ids_cleanup_rdmn_userid_by_tlgrm_userid	(nxs_chat_srv_d_db_ids_t *d_ctx, size_t tlgrm_userid, size_t rdmn_userid);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_ids_cleanup_rdmn_userid_by_tlgrm_username	(nxs_chat_srv_d_db_ids_t *d_ctx, nxs_string_t *tlgrm_username, size_t rdmn_userid);

nxs_chat_srv_err_t		nxs_chat_srv_d_db_ids_put_tlgrm					(nxs_chat_srv_d_db_ids_t *d_ctx, size_t tlgrm_userid, nxs_string_t *tlgrm_username, size_t rdmn_userid);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_ids_put_rdmn					(nxs_chat_srv_d_db_ids_t *d_ctx, nxs_string_t *tlgrm_username, size_t rdmn_userid);

#endif /* _INCLUDE_NXS_CHAT_SRV_D_DB_IDS_H */
