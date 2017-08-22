#ifndef _INCLUDE_NXS_CHAT_SRV_U_RDMN_ISSUES_H
#define _INCLUDE_NXS_CHAT_SRV_U_RDMN_ISSUES_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_rdmn_issues_s;

/* Prototypes */

nxs_chat_srv_u_rdmn_issues_t		*nxs_chat_srv_u_rdmn_issues_init		(void);
nxs_chat_srv_u_rdmn_issues_t		*nxs_chat_srv_u_rdmn_issues_free		(nxs_chat_srv_u_rdmn_issues_t *u_ctx);

nxs_chat_srv_err_t			nxs_chat_srv_u_rdmn_issues_add_note		(nxs_chat_srv_u_rdmn_issues_t *u_ctx, size_t issue_id, nxs_string_t *note, nxs_string_t *api_key);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_RDMN_ISSUES_H */
