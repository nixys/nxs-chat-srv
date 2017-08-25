#ifndef _INCLUDE_NXS_CHAT_SRV_C_TLGRM_H
#define _INCLUDE_NXS_CHAT_SRV_C_TLGRM_H

// clang-format off

/* Prototypes */

void					nxs_chat_srv_c_tlgrm_update_init		(nxs_chat_srv_m_tlgrm_update_t *update);
void					nxs_chat_srv_c_tlgrm_update_free		(nxs_chat_srv_m_tlgrm_update_t *update);
nxs_chat_srv_err_t			nxs_chat_srv_c_tlgrm_update_pull_json		(nxs_chat_srv_m_tlgrm_update_t *update, nxs_buf_t *json_buf);

void					nxs_chat_srv_c_tlgrm_callback_query_init	(nxs_chat_srv_m_tlgrm_callback_query_t *callback_query);
void					nxs_chat_srv_c_tlgrm_callback_query_free	(nxs_chat_srv_m_tlgrm_callback_query_t *callback_query);

void					nxs_chat_srv_c_tlgrm_message_init		(nxs_chat_srv_m_tlgrm_message_t *message);
void					nxs_chat_srv_c_tlgrm_message_free		(nxs_chat_srv_m_tlgrm_message_t *message);
nxs_chat_srv_err_t			nxs_chat_srv_c_tlgrm_message_result_pull_json	(nxs_chat_srv_m_tlgrm_message_t *message, nxs_bool_t *status, nxs_buf_t *json_buf);
nxs_chat_srv_m_tlgrm_message_t		*nxs_chat_srv_c_tlgrm_message_reply_alloc	(void);
nxs_chat_srv_m_tlgrm_message_t		*nxs_chat_srv_c_tlgrm_message_reply_destroy	(nxs_chat_srv_m_tlgrm_message_t *message);

void					nxs_chat_srv_c_tlgrm_chat_init			(nxs_chat_srv_m_tlgrm_chat_t *chat);
void					nxs_chat_srv_c_tlgrm_chat_free			(nxs_chat_srv_m_tlgrm_chat_t *chat);
nxs_chat_srv_m_tlgrm_chat_type_t	nxs_chat_srv_c_tlgrm_chat_type_map		(nxs_string_t *type_name);
nxs_string_t 				*nxs_chat_srv_c_tlgrm_parse_mode_map		(nxs_chat_srv_m_tlgrm_parse_mode_t mode);

void					nxs_chat_srv_c_tlgrm_user_init			(nxs_chat_srv_m_tlgrm_user_t *user);
void					nxs_chat_srv_c_tlgrm_user_free			(nxs_chat_srv_m_tlgrm_user_t *user);

void					nxs_chat_srv_c_tlgrm_inl_keyboard_init		(nxs_chat_srv_m_tlgrm_inl_keyboard_t *inl_keyboard);
void					nxs_chat_srv_c_tlgrm_inl_keyboard_free		(nxs_chat_srv_m_tlgrm_inl_keyboard_t *inl_keyboard);
nxs_chat_srv_err_t			nxs_chat_srv_c_tlgrm_inl_keyboard_add_button	(nxs_chat_srv_m_tlgrm_inl_keyboard_t *inl_keyboard, size_t pos_y, size_t pos_x, nxs_string_t *text, nxs_string_t *url, nxs_string_t *callback_data);
void					nxs_chat_srv_c_tlgrm_inl_keyboard_serialize	(nxs_chat_srv_m_tlgrm_inl_keyboard_t *inl_keyboard, nxs_string_t *out_str);

void					nxs_chat_srv_c_tlgrm_inl_keybutton_init		(nxs_chat_srv_m_tlgrm_inl_keybutton_t *inl_keybutton);
void					nxs_chat_srv_c_tlgrm_inl_keybutton_free		(nxs_chat_srv_m_tlgrm_inl_keybutton_t *inl_keybutton);

void					nxs_chat_srv_c_tlgrm_force_reply_init		(nxs_chat_srv_m_tlgrm_force_reply_t *force_reply);
void					nxs_chat_srv_c_tlgrm_force_reply_free		(nxs_chat_srv_m_tlgrm_force_reply_t *force_reply);
void					nxs_chat_srv_c_tlgrm_force_reply_set		(nxs_chat_srv_m_tlgrm_force_reply_t *force_reply);
void					nxs_chat_srv_c_tlgrm_force_reply_serialize	(nxs_chat_srv_m_tlgrm_force_reply_t *force_reply, nxs_string_t *out_str);

#endif /* _INCLUDE_NXS_CHAT_SRV_C_TLGRM_H */
