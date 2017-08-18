#ifndef _INCLUDE_NXS_CHAT_SRV_U_TLGRM_SENDMESSAGE_H
#define _INCLUDE_NXS_CHAT_SRV_U_TLGRM_SENDMESSAGE_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_tlgrm_sendmessage_s;

/* Prototypes */

nxs_chat_srv_u_tlgrm_sendmessage_t		*nxs_chat_srv_u_tlgrm_sendmessage_init		(void);
nxs_chat_srv_u_tlgrm_sendmessage_t		*nxs_chat_srv_u_tlgrm_sendmessage_free		(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx);

nxs_chat_srv_err_t				nxs_chat_srv_u_tlgrm_sendmessage_push		(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx);

void						nxs_chat_srv_u_tlgrm_sendmessage_add		(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx, size_t chat_id, nxs_string_t *text);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_TLGRM_SENDMESSAGE_H */
