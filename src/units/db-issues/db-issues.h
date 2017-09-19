#ifndef _INCLUDE_NXS_CHAT_SRV_U_DB_ISSUES_H
#define _INCLUDE_NXS_CHAT_SRV_U_DB_ISSUES_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_db_issues_s;

/* Prototypes */

nxs_chat_srv_u_db_issues_t	*nxs_chat_srv_u_db_issues_init		(void);
nxs_chat_srv_u_db_issues_t	*nxs_chat_srv_u_db_issues_free		(nxs_chat_srv_u_db_issues_t *u_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_u_db_issues_get		(nxs_chat_srv_u_db_issues_t *u_ctx, size_t tlgrm_chat_id, size_t tlgrm_message_id, size_t *rdmn_issue_id);
nxs_chat_srv_err_t		nxs_chat_srv_u_db_issues_set		(nxs_chat_srv_u_db_issues_t *u_ctx, size_t tlgrm_chat_id, size_t tlgrm_message_id, size_t rdmn_issue_id);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_DB_ISSUES_H */
