#ifndef _INCLUDE_NXS_CHAT_SRV_U_RDMN_LAST_ISSUE_H
#define _INCLUDE_NXS_CHAT_SRV_U_RDMN_LAST_ISSUE_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_rdmn_last_issue_s;

/* Prototypes */

nxs_chat_srv_u_rdmn_last_issue_t		*nxs_chat_srv_u_rdmn_last_issue_init			(void);
nxs_chat_srv_u_rdmn_last_issue_t		*nxs_chat_srv_u_rdmn_last_issue_free			(nxs_chat_srv_u_rdmn_last_issue_t *u_ctx);

nxs_chat_srv_err_t				nxs_chat_srv_u_rdmn_last_issue_pull			(nxs_chat_srv_u_rdmn_last_issue_t *u_ctx, size_t rdmn_userid);

nxs_chat_srv_err_t				nxs_chat_srv_u_rdmn_last_issue_get			(nxs_chat_srv_u_rdmn_last_issue_t *u_ctx, size_t *issue_id, nxs_string_t *issue_subject);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_RDMN_LAST_ISSUE_H */
