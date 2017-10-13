#ifndef _INCLUDE_NXS_CHAT_SRV_C_RDMN_ISSUES_H
#define _INCLUDE_NXS_CHAT_SRV_C_RDMN_ISSUES_H

// clang-format off

/* Prototypes */

void				nxs_chat_srv_c_rdmn_issues_init		(nxs_array_t *issues);
void				nxs_chat_srv_c_rdmn_issues_free		(nxs_array_t *issues);

void				nxs_chat_srv_c_rdmn_issues_cf_init	(nxs_array_t *custom_fields);
void				nxs_chat_srv_c_rdmn_issues_cf_free	(nxs_array_t *custom_fields);
void				nxs_chat_srv_c_rdmn_issues_cf_add	(nxs_array_t *custom_fields, size_t id, nxs_string_t *value);

void				nxs_chat_srv_c_rdmn_issues_uploads_init	(nxs_array_t *uploads);
void				nxs_chat_srv_c_rdmn_issues_uploads_free	(nxs_array_t *uploads);
void				nxs_chat_srv_c_rdmn_issues_uploads_add	(nxs_array_t *uploads, nxs_string_t *file_path, nxs_string_t *file_name);

#endif /* _INCLUDE_NXS_CHAT_SRV_C_RDMN_ISSUES_H */
