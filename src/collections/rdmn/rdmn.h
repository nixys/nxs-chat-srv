#ifndef _INCLUDE_NXS_CHAT_SRV_C_RDMN_H
#define _INCLUDE_NXS_CHAT_SRV_C_RDMN_H

// clang-format off

/* Prototypes */

void				nxs_chat_srv_c_rdmn_access_init			(nxs_chat_srv_m_rdmn_access_t *access);
void				nxs_chat_srv_c_rdmn_access_free			(nxs_chat_srv_m_rdmn_access_t *access);

void				nxs_chat_srv_c_rdmn_permissions_init		(nxs_chat_srv_m_rdmn_permissions_t *permissions);
void				nxs_chat_srv_c_rdmn_permissions_free		(nxs_chat_srv_m_rdmn_permissions_t *permissions);

void				nxs_chat_srv_c_rdmn_role_init			(nxs_chat_srv_m_rdmn_role_t *role);
void				nxs_chat_srv_c_rdmn_role_free			(nxs_chat_srv_m_rdmn_role_t *role);

void				nxs_chat_srv_c_rdmn_member_init			(nxs_chat_srv_m_rdmn_member_t *member);
void				nxs_chat_srv_c_rdmn_member_free			(nxs_chat_srv_m_rdmn_member_t *member);

void				nxs_chat_srv_c_rdmn_project_init		(nxs_chat_srv_m_rdmn_project_t *project);
void				nxs_chat_srv_c_rdmn_project_free		(nxs_chat_srv_m_rdmn_project_t *project);

void				nxs_chat_srv_c_rdmn_tracker_init		(nxs_chat_srv_m_rdmn_tracker_t *tracker);
void				nxs_chat_srv_c_rdmn_tracker_free		(nxs_chat_srv_m_rdmn_tracker_t *tracker);

void				nxs_chat_srv_c_rdmn_status_init			(nxs_chat_srv_m_rdmn_status_t *status);
void				nxs_chat_srv_c_rdmn_status_free			(nxs_chat_srv_m_rdmn_status_t *status);

void				nxs_chat_srv_c_rdmn_priority_init		(nxs_chat_srv_m_rdmn_priority_t *priority);
void				nxs_chat_srv_c_rdmn_priority_free		(nxs_chat_srv_m_rdmn_priority_t *priority);

void				nxs_chat_srv_c_rdmn_user_init			(nxs_chat_srv_m_rdmn_user_t *user);
void				nxs_chat_srv_c_rdmn_user_free			(nxs_chat_srv_m_rdmn_user_t *user);

void				nxs_chat_srv_c_rdmn_detail_init			(nxs_chat_srv_m_rdmn_detail_t *detail);
void				nxs_chat_srv_c_rdmn_detail_free			(nxs_chat_srv_m_rdmn_detail_t *detail);

void				nxs_chat_srv_c_rdmn_journal_init		(nxs_chat_srv_m_rdmn_journal_t *journal);
void				nxs_chat_srv_c_rdmn_journal_free		(nxs_chat_srv_m_rdmn_journal_t *journal);

void				nxs_chat_srv_c_rdmn_issue_init			(nxs_chat_srv_m_rdmn_issue_t *issue);
void				nxs_chat_srv_c_rdmn_issue_free			(nxs_chat_srv_m_rdmn_issue_t *issue);

void				nxs_chat_srv_c_rdmn_data_init			(nxs_chat_srv_m_rdmn_data_t *data);
void				nxs_chat_srv_c_rdmn_data_free			(nxs_chat_srv_m_rdmn_data_t *data);

void				nxs_chat_srv_c_rdmn_update_init			(nxs_chat_srv_m_rdmn_update_t *update);
void				nxs_chat_srv_c_rdmn_update_free			(nxs_chat_srv_m_rdmn_update_t *update);
nxs_chat_srv_err_t		nxs_chat_srv_c_rdmn_update_pull_json		(nxs_chat_srv_m_rdmn_update_t *update, nxs_buf_t *json_buf);

void				nxs_chat_srv_c_rdmn_attachment_init		(nxs_chat_srv_m_rdmn_attachment_t *attachment);
void				nxs_chat_srv_c_rdmn_attachment_free		(nxs_chat_srv_m_rdmn_attachment_t *attachment);
nxs_chat_srv_err_t		nxs_chat_srv_c_rdmn_attachment_pull_json	(nxs_chat_srv_m_rdmn_attachment_t *attachment, nxs_buf_t *json_buf);

#endif /* _INCLUDE_NXS_CHAT_SRV_C_RDMN_H */
