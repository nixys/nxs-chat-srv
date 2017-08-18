#ifndef _INCLUDE_NXS_CHAT_SRV_U_TLGRM_UPDATE_H
#define _INCLUDE_NXS_CHAT_SRV_U_TLGRM_UPDATE_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_tlgrm_update_s;

/* Prototypes */

nxs_chat_srv_u_tlgrm_update_t		*nxs_chat_srv_u_tlgrm_update_init		(void);
nxs_chat_srv_u_tlgrm_update_t		*nxs_chat_srv_u_tlgrm_update_free		(nxs_chat_srv_u_tlgrm_update_t *u_ctx);

nxs_chat_srv_err_t			nxs_chat_srv_u_tlgrm_update_pull_json		(nxs_chat_srv_u_tlgrm_update_t *u_ctx, nxs_buf_t *json_buf);

nxs_chat_srv_m_tlgrm_update_t		*nxs_chat_srv_u_tlgrm_update_get		(nxs_chat_srv_u_tlgrm_update_t *u_ctx);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_TLGRM_UPDATE_H */
