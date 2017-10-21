#ifndef _INCLUDE_NXS_CHAT_SRV_D_RA_QUEUE_H
#define _INCLUDE_NXS_CHAT_SRV_D_RA_QUEUE_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_d_ra_queue_s;

/* Prototypes */

nxs_chat_srv_d_ra_queue_t	*nxs_chat_srv_d_ra_queue_init		(void);
nxs_chat_srv_d_ra_queue_t	*nxs_chat_srv_d_ra_queue_free		(nxs_chat_srv_d_ra_queue_t *d_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_d_ra_queue_add		(nxs_chat_srv_d_ra_queue_t *d_ctx, nxs_string_t *value);
nxs_chat_srv_err_t		nxs_chat_srv_d_ra_queue_get		(nxs_chat_srv_d_ra_queue_t *d_ctx, nxs_string_t *value);

#endif /* _INCLUDE_NXS_CHAT_SRV_D_RA_QUEUE_H */
