#ifndef _INCLUDE_NXS_CHAT_SRV_C_QUEUE_COM_H
#define _INCLUDE_NXS_CHAT_SRV_C_QUEUE_COM_H

// clang-format off

/* Prototypes */

void				nxs_chat_srv_c_queue_com_serialize	(nxs_string_t *out_msg, nxs_chat_srv_m_queue_com_types_t type, nxs_string_t *body);
nxs_chat_srv_err_t		nxs_chat_srv_c_queue_com_deserialize	(nxs_buf_t *in_msg, nxs_chat_srv_m_queue_com_types_t *type, nxs_string_t *body);


#endif /* _INCLUDE_NXS_CHAT_SRV_C_QUEUE_COM_H */
