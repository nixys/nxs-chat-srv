#ifndef _INCLUDE_NXS_CHAT_SRV_U_TLGRM_SENDMESSAGE_H
#define _INCLUDE_NXS_CHAT_SRV_U_TLGRM_SENDMESSAGE_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_tlgrm_sendmessage_s;

/* Prototypes */

nxs_chat_srv_u_tlgrm_sendmessage_t		*nxs_chat_srv_u_tlgrm_sendmessage_init				(void);
nxs_chat_srv_u_tlgrm_sendmessage_t		*nxs_chat_srv_u_tlgrm_sendmessage_free				(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx);

nxs_chat_srv_err_t				nxs_chat_srv_u_tlgrm_sendmessage_push				(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx);

size_t						nxs_chat_srv_u_tlgrm_sendmessage_add				(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx, size_t chat_id, nxs_string_t *text, nxs_chat_srv_m_tlgrm_parse_mode_t parse_mode);

nxs_chat_srv_err_t				nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_add		(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx, size_t message_id, size_t pos_y, size_t pos_x, nxs_string_t *text, nxs_string_t *url, nxs_string_t *callback_data);
nxs_chat_srv_err_t				nxs_chat_srv_u_tlgrm_sendmessage_force_reply_set		(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx, size_t message_id);

void nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_print(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx, size_t message_id, nxs_string_t *reply_markup);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_TLGRM_SENDMESSAGE_H */
