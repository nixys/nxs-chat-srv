#ifndef _INCLUDE_NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_WIN_NEW_ISSUE_H
#define _INCLUDE_NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_WIN_NEW_ISSUE_H

// clang-format off

nxs_chat_srv_err_t		nxs_chat_srv_p_queue_worker_tlgrm_update_win_new_issue		(nxs_chat_srv_u_db_sess_t *sess_ctx, size_t chat_id, size_t message_id, nxs_bool_t full_message, nxs_chat_srv_m_tlgrm_update_t *update, nxs_buf_t *response_buf);

#endif /* _INCLUDE_NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_WIN_NEW_ISSUE_H */
