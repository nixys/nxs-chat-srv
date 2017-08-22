#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <proc/queue-worker/ctx/queue-worker-ctx.h>

/* Definitions */


/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_ctx_init(nxs_chat_srv_p_queue_worker_ctx_t *p_ctx)
{

	return nxs_chat_srv_c_unix_sock_create(&p_ctx->queue_sock);
}

void nxs_chat_srv_p_queue_worker_ctx_free(nxs_chat_srv_p_queue_worker_ctx_t *p_ctx)
{

	nxs_net_unix_server_unlink(&process, &p_ctx->queue_sock);
}

nxs_net_unix_server_t *nxs_chat_srv_p_queue_worker_ctx_get_sock(nxs_chat_srv_p_queue_worker_ctx_t *p_ctx)
{

	if(p_ctx == NULL) {

		return NULL;
	}

	return &p_ctx->queue_sock;
}

/* Module internal (static) functions */
