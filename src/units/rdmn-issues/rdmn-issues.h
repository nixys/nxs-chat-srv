#ifndef _INCLUDE_NXS_CHAT_SRV_U_RDMN_ISSUES_H
#define _INCLUDE_NXS_CHAT_SRV_U_RDMN_ISSUES_H

// clang-format off

/* Structs declarations */



/* Prototypes */

nxs_chat_srv_err_t		nxs_chat_srv_u_rdmn_issues_create		(size_t project_id, size_t priority_id, nxs_string_t *subject, nxs_string_t *description, nxs_bool_t is_private, size_t *new_issue_id, nxs_array_t *uploads, nxs_string_t *api_key);
nxs_chat_srv_err_t		nxs_chat_srv_u_rdmn_issues_add_note		(size_t issue_id, size_t assigned_to_id, nxs_string_t *note, nxs_bool_t private_notes, size_t status_id, nxs_array_t *uploads, nxs_array_t *custom_fields, nxs_string_t *api_key);
nxs_chat_srv_err_t		nxs_chat_srv_u_rdmn_issues_get_query		(nxs_array_t *issues, size_t issue_query_id, size_t offset, size_t limit, nxs_string_t *api_key, size_t *total_count);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_RDMN_ISSUES_H */
