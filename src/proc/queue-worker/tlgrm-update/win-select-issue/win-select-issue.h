#ifndef _INCLUDE_NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_WIN_SELECT_ISSUE_H
#define _INCLUDE_NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_WIN_SELECT_ISSUE_H

// clang-format off

nxs_chat_srv_err_t		nxs_chat_srv_p_queue_worker_tlgrm_update_win_select_issue		(nxs_chat_srv_u_db_sess_t *sess_ctx, size_t chat_id, size_t message_id, nxs_array_t *issues, size_t offset, size_t issues_count, nxs_buf_t *response_buf);

#endif /* _INCLUDE_NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_WIN_SELECT_ISSUE_H */
