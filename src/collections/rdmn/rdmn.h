#ifndef _INCLUDE_NXS_CHAT_SRV_C_RDMN_H
#define _INCLUDE_NXS_CHAT_SRV_C_RDMN_H

// clang-format off

/* Prototypes */

void					nxs_chat_srv_c_rdmn					(void);

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

void				nxs_chat_srv_c_rdmn_journal_init		(nxs_chat_srv_m_rdmn_journal_t *journal);
void				nxs_chat_srv_c_rdmn_journal_free		(nxs_chat_srv_m_rdmn_journal_t *journal);

void				nxs_chat_srv_c_rdmn_issue_init			(nxs_chat_srv_m_rdmn_issue_t *issue);
void				nxs_chat_srv_c_rdmn_issue_free			(nxs_chat_srv_m_rdmn_issue_t *issue);

void				nxs_chat_srv_c_rdmn_data_init			(nxs_chat_srv_m_rdmn_data_t *data);
void				nxs_chat_srv_c_rdmn_data_free			(nxs_chat_srv_m_rdmn_data_t *data);

void				nxs_chat_srv_c_rdmn_update_init			(nxs_chat_srv_m_rdmn_update_t *update);
void				nxs_chat_srv_c_rdmn_update_free			(nxs_chat_srv_m_rdmn_update_t *update);
nxs_chat_srv_err_t		nxs_chat_srv_c_rdmn_update_pull_json		(nxs_chat_srv_m_rdmn_update_t *update, nxs_buf_t *json_buf);


#endif /* _INCLUDE_NXS_CHAT_SRV_C_RDMN_H */
