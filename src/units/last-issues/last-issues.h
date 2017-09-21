#ifndef _INCLUDE_NXS_CHAT_SRV_U_LAST_ISSUES_H
#define _INCLUDE_NXS_CHAT_SRV_U_LAST_ISSUES_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_last_issues_s;

/* Prototypes */

nxs_chat_srv_u_last_issues_t		*nxs_chat_srv_u_last_issues_init		(void);
nxs_chat_srv_u_last_issues_t		*nxs_chat_srv_u_last_issues_free		(nxs_chat_srv_u_last_issues_t *u_ctx);

nxs_chat_srv_err_t			nxs_chat_srv_u_last_issues_get			(nxs_chat_srv_u_last_issues_t *u_ctx, size_t tlgrm_userid, size_t rdmn_userid, nxs_string_t *user_api_key, size_t *rdmn_last_issue_id, nxs_string_t *issue_subject);
nxs_chat_srv_err_t			nxs_chat_srv_u_last_issues_set			(nxs_chat_srv_u_last_issues_t *u_ctx, size_t tlgrm_userid, size_t rdmn_last_issue_id);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_LAST_ISSUES_H */
