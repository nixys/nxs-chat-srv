#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_PRESALE_H
#define _INCLUDE_NXS_CHAT_SRV_D_DB_PRESALE_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_d_db_presale_s;

/* Prototypes */

nxs_chat_srv_d_db_presale_t	*nxs_chat_srv_d_db_presale_init			(void);
nxs_chat_srv_d_db_presale_t	*nxs_chat_srv_d_db_presale_free			(nxs_chat_srv_d_db_presale_t *d_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_d_db_presale_pull			(nxs_chat_srv_d_db_presale_t *d_ctx);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_presale_set			(nxs_chat_srv_d_db_presale_t *d_ctx, size_t tlgrm_userid, size_t rdmn_issue_id);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_presale_get			(nxs_chat_srv_d_db_presale_t *d_ctx, size_t i, size_t *tlgrm_userid, size_t *rdmn_issue_id);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_presale_get_by_user_id	(nxs_chat_srv_d_db_presale_t *d_ctx, size_t tlgrm_userid, size_t *rdmn_issue_id);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_presale_get_by_issue_id	(nxs_chat_srv_d_db_presale_t *d_ctx, size_t rdmn_issue_id, size_t *tlgrm_userid);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_presale_del			(nxs_chat_srv_d_db_presale_t *d_ctx, size_t tlgrm_userid);

#endif /* _INCLUDE_NXS_CHAT_SRV_D_DB_PRESALE_H */
