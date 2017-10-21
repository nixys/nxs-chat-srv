#ifndef _INCLUDE_NXS_CHAT_SRV_U_RA_QUEUE_H
#define _INCLUDE_NXS_CHAT_SRV_U_RA_QUEUE_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_ra_queue_s;

/* Prototypes */

nxs_chat_srv_u_ra_queue_t	*nxs_chat_srv_u_ra_queue_init		(void);
nxs_chat_srv_u_ra_queue_t	*nxs_chat_srv_u_ra_queue_free		(nxs_chat_srv_u_ra_queue_t *u_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_u_ra_queue_add		(nxs_chat_srv_u_ra_queue_t *u_ctx, nxs_chat_srv_m_ra_queue_type_t type, nxs_string_t *data);
nxs_chat_srv_err_t		nxs_chat_srv_u_ra_queue_get		(nxs_chat_srv_u_ra_queue_t *u_ctx, nxs_chat_srv_m_ra_queue_type_t *type, nxs_string_t *data);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_RA_QUEUE_H */
