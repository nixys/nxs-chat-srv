#ifndef _INCLUDE_NXS_CHAT_SRV_D_RDMN_ISSUES_H
#define _INCLUDE_NXS_CHAT_SRV_D_RDMN_ISSUES_H

// clang-format off

/* Structs declarations */



/* Prototypes */

nxs_chat_srv_err_t			nxs_chat_srv_d_rdmn_issues_add_comment		(size_t issue_id, nxs_string_t *note, nxs_string_t *user_api_key, nxs_buf_t *out_buf, nxs_http_code_t *http_code);
nxs_chat_srv_err_t			nxs_chat_srv_d_rdmn_issues_create		(size_t project_id, size_t priority_id, nxs_string_t *subject, nxs_string_t *description, nxs_string_t *user_api_key, nxs_buf_t *out_buf, nxs_http_code_t *http_code);
nxs_chat_srv_err_t			nxs_chat_srv_d_rdmn_issues_get_query		(size_t issue_query_id, nxs_string_t *user_api_key, size_t offset, size_t limit, nxs_buf_t *out_buf, nxs_http_code_t *http_code);
nxs_chat_srv_err_t			nxs_chat_srv_d_rdmn_issues_get_issue		(size_t issue_id, nxs_string_t *user_api_key, nxs_buf_t *out_buf, nxs_http_code_t *http_code);

#endif /* _INCLUDE_NXS_CHAT_SRV_D_RDMN_ISSUES_H */
