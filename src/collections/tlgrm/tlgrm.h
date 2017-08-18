#ifndef _INCLUDE_NXS_CHAT_SRV_C_TLGRM_H
#define _INCLUDE_NXS_CHAT_SRV_C_TLGRM_H

// clang-format off

/* Prototypes */

void				nxs_chat_srv_c_tlgrm_update_init	(nxs_chat_srv_m_tlgrm_update_t *update);
void				nxs_chat_srv_c_tlgrm_update_free	(nxs_chat_srv_m_tlgrm_update_t *update);
void				nxs_chat_srv_c_tlgrm_message_init	(nxs_chat_srv_m_tlgrm_message_t *message);
void				nxs_chat_srv_c_tlgrm_message_free	(nxs_chat_srv_m_tlgrm_message_t *message);

void				nxs_chat_srv_c_tlgrm_chat_init		(nxs_chat_srv_m_tlgrm_chat_t *chat);
void				nxs_chat_srv_c_tlgrm_chat_free		(nxs_chat_srv_m_tlgrm_chat_t *chat);

#endif /* _INCLUDE_NXS_CHAT_SRV_C_TLGRM_H */
