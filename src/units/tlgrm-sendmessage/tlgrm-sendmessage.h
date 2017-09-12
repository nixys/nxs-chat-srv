#ifndef _INCLUDE_NXS_CHAT_SRV_U_TLGRM_SENDMESSAGE_H
#define _INCLUDE_NXS_CHAT_SRV_U_TLGRM_SENDMESSAGE_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_tlgrm_sendmessage_s;

/* Prototypes */

nxs_chat_srv_u_tlgrm_sendmessage_t		*nxs_chat_srv_u_tlgrm_sendmessage_init				(void);
nxs_chat_srv_u_tlgrm_sendmessage_t		*nxs_chat_srv_u_tlgrm_sendmessage_free				(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx);

nxs_chat_srv_err_t				nxs_chat_srv_u_tlgrm_sendmessage_push				(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx);

nxs_buf_t					*nxs_chat_srv_u_tlgrm_sendmessage_get_response_buf		(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx);

void						nxs_chat_srv_u_tlgrm_sendmessage_add				(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx, size_t chat_id, nxs_string_t *text, nxs_chat_srv_m_tlgrm_parse_mode_t parse_mode);

nxs_chat_srv_err_t				nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_add		(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx, size_t pos_y, size_t pos_x, nxs_string_t *text, nxs_string_t *url, nxs_string_t *callback_data);
nxs_chat_srv_err_t				nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add	(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx, size_t pos_y, size_t pos_x, nxs_chat_srv_m_tlgrm_bttn_callback_type_t type, size_t object_id, const u_char *text, ...);
nxs_chat_srv_err_t				nxs_chat_srv_u_tlgrm_sendmessage_force_reply_set		(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_TLGRM_SENDMESSAGE_H */
