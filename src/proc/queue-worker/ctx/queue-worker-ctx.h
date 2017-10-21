#ifndef _INCLUDE_NXS_CHAT_SRV_P_QUEUE_WORKER_CTX_H
#define _INCLUDE_NXS_CHAT_SRV_P_QUEUE_WORKER_CTX_H

// clang-format off

typedef struct
{
	nxs_chat_srv_u_ra_queue_t	*ra_queue_ctx;		/* Rest API queue context for communications with queue-workers */
} nxs_chat_srv_p_queue_worker_ctx_t;

void				nxs_chat_srv_p_queue_worker_ctx_init		(nxs_chat_srv_p_queue_worker_ctx_t *p_ctx);
void				nxs_chat_srv_p_queue_worker_ctx_free		(nxs_chat_srv_p_queue_worker_ctx_t *p_ctx);

nxs_chat_srv_u_ra_queue_t	*nxs_chat_srv_p_queue_worker_ctx_get_ra_queue	(nxs_chat_srv_p_queue_worker_ctx_t *p_ctx);

#endif /* _INCLUDE_NXS_CHAT_SRV_P_QUEUE_WORKER_CTX_H */
