#ifndef _INCLUDE_NXS_CHAT_SRV_U_TLGRM_CHAT_H
#define _INCLUDE_NXS_CHAT_SRV_U_TLGRM_CHAT_H

// clang-format off

/* Structs declarations */



/* Prototypes */

nxs_chat_srv_u_tlgrm_chat_t	*nxs_chat_srv_u_tlgrm_chat_init		(void);
nxs_chat_srv_u_tlgrm_chat_t	*nxs_chat_srv_u_tlgrm_chat_free		(nxs_chat_srv_u_tlgrm_chat_t *u_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_u_tlgrm_chat_pull		(nxs_chat_srv_u_tlgrm_chat_t *u_ctx, size_t chat_id);
nxs_chat_srv_m_tlgrm_chat_t	*nxs_chat_srv_u_tlgrm_chat_get		(nxs_chat_srv_u_tlgrm_chat_t *u_ctx);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_TLGRM_CHAT_H */
