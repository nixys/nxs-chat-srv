#ifndef _INCLUDE_NXS_CHAT_SRV_C_TLGRM_H
#define _INCLUDE_NXS_CHAT_SRV_C_TLGRM_H

// clang-format off

/* Prototypes */

void					nxs_chat_srv_c_tlgrm_format_escape_markdown	(nxs_string_t *text_dst, nxs_string_t *text_src);
void					nxs_chat_srv_c_tlgrm_format_escape_html		(nxs_string_t *text_dst, nxs_string_t *text_src);

nxs_chat_srv_err_t			nxs_chat_srv_c_tlgrm_make_message_chunks	(nxs_string_t *message_header, nxs_string_t *message_src, nxs_array_t *chunks);
nxs_chat_srv_err_t			nxs_chat_srv_c_tlgrm_make_message_preview	(nxs_string_t *message_dst, nxs_string_t *message_src);

void					nxs_chat_srv_c_tlgrm_update_init		(nxs_chat_srv_m_tlgrm_update_t *update);
void					nxs_chat_srv_c_tlgrm_update_free		(nxs_chat_srv_m_tlgrm_update_t *update);
nxs_chat_srv_err_t			nxs_chat_srv_c_tlgrm_update_pull_json		(nxs_chat_srv_m_tlgrm_update_t *update, nxs_buf_t *json_buf);
nxs_chat_srv_m_tlgrm_update_type_t	nxs_chat_srv_c_tlgrm_update_get_type		(nxs_chat_srv_m_tlgrm_update_t *update);

void					nxs_chat_srv_c_tlgrm_callback_query_init	(nxs_chat_srv_m_tlgrm_callback_query_t *callback_query);
void					nxs_chat_srv_c_tlgrm_callback_query_free	(nxs_chat_srv_m_tlgrm_callback_query_t *callback_query);

void					nxs_chat_srv_c_tlgrm_message_entity_init	(nxs_chat_srv_m_tlgrm_message_entity_t *entity);
void					nxs_chat_srv_c_tlgrm_message_entity_free	(nxs_chat_srv_m_tlgrm_message_entity_t *entity);

void					nxs_chat_srv_c_tlgrm_photo_size_init		(nxs_chat_srv_m_tlgrm_photo_size_t *photo_size);
void					nxs_chat_srv_c_tlgrm_photo_size_free		(nxs_chat_srv_m_tlgrm_photo_size_t *entity);

void					nxs_chat_srv_c_tlgrm_document_init		(nxs_chat_srv_m_tlgrm_document_t *document);
void					nxs_chat_srv_c_tlgrm_document_free		(nxs_chat_srv_m_tlgrm_document_t *document);

void					nxs_chat_srv_c_tlgrm_sticker_init		(nxs_chat_srv_m_tlgrm_sticker_t *sticker);
void					nxs_chat_srv_c_tlgrm_sticker_free		(nxs_chat_srv_m_tlgrm_sticker_t *sticker);

void					nxs_chat_srv_c_tlgrm_voice_init			(nxs_chat_srv_m_tlgrm_voice_t *voice);
void					nxs_chat_srv_c_tlgrm_voice_free			(nxs_chat_srv_m_tlgrm_voice_t *voice);

void					nxs_chat_srv_c_tlgrm_video_init			(nxs_chat_srv_m_tlgrm_video_t *video);
void					nxs_chat_srv_c_tlgrm_video_free			(nxs_chat_srv_m_tlgrm_video_t *video);

void					nxs_chat_srv_c_tlgrm_message_init		(nxs_chat_srv_m_tlgrm_message_t *message);
void					nxs_chat_srv_c_tlgrm_message_free		(nxs_chat_srv_m_tlgrm_message_t *message);
nxs_chat_srv_err_t			nxs_chat_srv_c_tlgrm_message_result_pull_json	(nxs_chat_srv_m_tlgrm_message_t *message, nxs_bool_t *status, nxs_buf_t *json_buf);
nxs_chat_srv_m_tlgrm_message_t		*nxs_chat_srv_c_tlgrm_message_reply_alloc	(void);
nxs_chat_srv_m_tlgrm_message_t		*nxs_chat_srv_c_tlgrm_message_reply_destroy	(nxs_chat_srv_m_tlgrm_message_t *message);

void					nxs_chat_srv_c_tlgrm_chat_init			(nxs_chat_srv_m_tlgrm_chat_t *chat);
void					nxs_chat_srv_c_tlgrm_chat_free			(nxs_chat_srv_m_tlgrm_chat_t *chat);
nxs_chat_srv_err_t			nxs_chat_srv_c_tlgrm_chat_result_pull_json	(nxs_chat_srv_m_tlgrm_chat_t *chat, nxs_bool_t *status, nxs_buf_t *json_buf);
nxs_chat_srv_m_tlgrm_chat_type_t	nxs_chat_srv_c_tlgrm_chat_type_map		(nxs_string_t *type_name);
nxs_string_t 				*nxs_chat_srv_c_tlgrm_parse_mode_map		(nxs_chat_srv_m_tlgrm_parse_mode_t mode);

void					nxs_chat_srv_c_tlgrm_user_init			(nxs_chat_srv_m_tlgrm_user_t *user);
void					nxs_chat_srv_c_tlgrm_user_free			(nxs_chat_srv_m_tlgrm_user_t *user);

void					nxs_chat_srv_c_tlgrm_file_init			(nxs_chat_srv_m_tlgrm_file_t *file);
void					nxs_chat_srv_c_tlgrm_file_free			(nxs_chat_srv_m_tlgrm_file_t *file);
nxs_chat_srv_err_t			nxs_chat_srv_c_tlgrm_file_result_pull_json	(nxs_chat_srv_m_tlgrm_file_t *file, nxs_bool_t *status, nxs_buf_t *json_buf);

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

void					nxs_chat_srv_c_tlgrm_bttn_callback_serialize	(nxs_chat_srv_m_tlgrm_bttn_callback_t callback, nxs_string_t *callback_str);
nxs_chat_srv_err_t			nxs_chat_srv_c_tlgrm_bttn_callback_deserialize	(nxs_chat_srv_m_tlgrm_bttn_callback_t *callback, nxs_string_t *callback_str);

void					nxs_chat_srv_c_tlgrm_pre_uploads_init		(nxs_array_t *pre_uploads);
void					nxs_chat_srv_c_tlgrm_pre_uploads_free		(nxs_array_t *pre_uploads);
void					nxs_chat_srv_c_tlgrm_pre_uploads_clear		(nxs_array_t *pre_uploads);
nxs_chat_srv_err_t			nxs_chat_srv_c_tlgrm_pre_uploads_add(		nxs_array_t *pre_uploads, nxs_chat_srv_m_tlgrm_update_t *update);
void					nxs_chat_srv_c_tlgrm_pre_uploads_serialize	(nxs_array_t *pre_uploads, nxs_string_t *files);
nxs_cfg_json_state_t			nxs_chat_srv_c_tlgrm_pre_uploads_extract_json	(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el, nxs_array_t *cfg_arr);

#endif /* _INCLUDE_NXS_CHAT_SRV_C_TLGRM_H */
