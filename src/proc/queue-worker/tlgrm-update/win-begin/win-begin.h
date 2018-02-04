#ifndef _INCLUDE_NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_WIN_BEGIN_H
#define _INCLUDE_NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_WIN_BEGIN_H

// clang-format off

nxs_chat_srv_err_t		nxs_chat_srv_p_queue_worker_tlgrm_update_win_begin	(nxs_chat_srv_m_user_ctx_t *user_ctx, size_t chat_id, size_t message_id, nxs_string_t *user_api_key, nxs_bool_t private_notes, size_t files_count, nxs_buf_t *response_buf);

#endif /* _INCLUDE_NXS_CHAT_SRV_P_QUEUE_WORKER_TLGRM_UPDATE_WIN_BEGIN_H */
