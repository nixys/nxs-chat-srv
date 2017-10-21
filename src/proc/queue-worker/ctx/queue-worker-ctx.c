#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

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

void nxs_chat_srv_p_queue_worker_ctx_init(nxs_chat_srv_p_queue_worker_ctx_t *p_ctx)
{

	p_ctx->ra_queue_ctx = nxs_chat_srv_u_ra_queue_init();
}

void nxs_chat_srv_p_queue_worker_ctx_free(nxs_chat_srv_p_queue_worker_ctx_t *p_ctx)
{

	p_ctx->ra_queue_ctx = nxs_chat_srv_u_ra_queue_free(p_ctx->ra_queue_ctx);
}

nxs_chat_srv_u_ra_queue_t *nxs_chat_srv_p_queue_worker_ctx_get_ra_queue(nxs_chat_srv_p_queue_worker_ctx_t *p_ctx)
{

	if(p_ctx == NULL) {

		return NULL;
	}

	return p_ctx->ra_queue_ctx;
}

/* Module internal (static) functions */
