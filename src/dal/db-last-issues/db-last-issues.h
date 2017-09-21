#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_LAST_ISSUES_H
#define _INCLUDE_NXS_CHAT_SRV_D_DB_LAST_ISSUES_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_d_db_last_issues_s;

/* Prototypes */

nxs_chat_srv_d_db_last_issues_t			*nxs_chat_srv_d_db_last_issues_init		(void);
nxs_chat_srv_d_db_last_issues_t			*nxs_chat_srv_d_db_last_issues_free		(nxs_chat_srv_d_db_last_issues_t *d_ctx);

nxs_chat_srv_err_t				nxs_chat_srv_d_db_last_issues_get		(nxs_chat_srv_d_db_last_issues_t *d_ctx, size_t tlgrm_userid, nxs_string_t *value);
nxs_chat_srv_err_t				nxs_chat_srv_d_db_last_issues_put		(nxs_chat_srv_d_db_last_issues_t *d_ctx, size_t tlgrm_userid, nxs_string_t *value);

#endif /* _INCLUDE_NXS_CHAT_SRV_D_DB_LAST_ISSUES_H */
