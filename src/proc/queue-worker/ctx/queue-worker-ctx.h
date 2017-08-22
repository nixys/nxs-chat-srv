#ifndef _INCLUDE_NXS_CHAT_SRV_P_QUEUE_WORKER_CTX_H
#define _INCLUDE_NXS_CHAT_SRV_P_QUEUE_WORKER_CTX_H

// clang-format off

typedef struct
{
	nxs_net_unix_server_t		queue_sock;		/* Unix socket for queue worker tasks get */
} nxs_chat_srv_p_queue_worker_ctx_t;

nxs_chat_srv_err_t		nxs_chat_srv_p_queue_worker_ctx_init		(nxs_chat_srv_p_queue_worker_ctx_t *p_ctx);
void				nxs_chat_srv_p_queue_worker_ctx_free		(nxs_chat_srv_p_queue_worker_ctx_t *p_ctx);

nxs_net_unix_server_t		*nxs_chat_srv_p_queue_worker_ctx_get_sock	(nxs_chat_srv_p_queue_worker_ctx_t *p_ctx);

#endif /* _INCLUDE_NXS_CHAT_SRV_P_QUEUE_WORKER_CTX_H */
