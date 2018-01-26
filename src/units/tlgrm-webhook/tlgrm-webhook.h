#ifndef _INCLUDE_NXS_CHAT_SRV_U_TLGRM_WEBHOOK_H
#define _INCLUDE_NXS_CHAT_SRV_U_TLGRM_WEBHOOK_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_tlgrm_webhook_s;

/* Prototypes */

nxs_chat_srv_u_tlgrm_webhook_t		*nxs_chat_srv_u_tlgrm_webhook_init		(void);
nxs_chat_srv_u_tlgrm_webhook_t		*nxs_chat_srv_u_tlgrm_webhook_free		(nxs_chat_srv_u_tlgrm_webhook_t *u_ctx);

nxs_chat_srv_err_t			nxs_chat_srv_u_tlgrm_webhook_get		(nxs_chat_srv_u_tlgrm_webhook_t *u_ctx, nxs_string_t *url);
nxs_chat_srv_err_t			nxs_chat_srv_u_tlgrm_webhook_set		(nxs_chat_srv_u_tlgrm_webhook_t *u_ctx, nxs_bool_t set_certificate, nxs_bool_t *status, nxs_string_t *description);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_TLGRM_WEBHOOK_H */
