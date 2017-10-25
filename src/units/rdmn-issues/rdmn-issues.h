#ifndef _INCLUDE_NXS_CHAT_SRV_U_RDMN_ISSUES_H
#define _INCLUDE_NXS_CHAT_SRV_U_RDMN_ISSUES_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_rdmn_issues_s;

/* Prototypes */

nxs_chat_srv_u_rdmn_issues_t	*nxs_chat_srv_u_rdmn_issues_init		(void);
nxs_chat_srv_u_rdmn_issues_t	*nxs_chat_srv_u_rdmn_issues_free		(nxs_chat_srv_u_rdmn_issues_t *u_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_u_rdmn_issues_create		(nxs_chat_srv_u_rdmn_issues_t *u_ctx, size_t project_id, size_t priority_id, nxs_string_t *subject, nxs_string_t *description, nxs_bool_t is_private, size_t *new_issue_id, nxs_array_t *uploads, nxs_string_t *api_key);
nxs_chat_srv_err_t		nxs_chat_srv_u_rdmn_issues_add_note		(nxs_chat_srv_u_rdmn_issues_t *u_ctx, size_t issue_id, size_t assigned_to_id, nxs_string_t *note, nxs_bool_t private_notes, size_t status_id, nxs_array_t *uploads, nxs_string_t *api_key);
nxs_chat_srv_err_t		nxs_chat_srv_u_rdmn_issues_get_query		(nxs_chat_srv_u_rdmn_issues_t *u_ctx, size_t issue_query_id, size_t offset, size_t limit, nxs_string_t *api_key, size_t *total_count);

nxs_string_t			*nxs_chat_srv_u_rdmn_issues_get_shorts		(nxs_chat_srv_u_rdmn_issues_t *u_ctx, size_t i, size_t *issue_id);
void				nxs_chat_srv_u_rdmn_issues_cf_add		(nxs_chat_srv_u_rdmn_issues_t *u_ctx, size_t id, nxs_string_t *value);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_RDMN_ISSUES_H */
