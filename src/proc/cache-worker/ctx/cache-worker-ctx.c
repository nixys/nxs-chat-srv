#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <proc/cache-worker/ctx/cache-worker-ctx.h>

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

void nxs_chat_srv_p_cache_worker_ctx_init(nxs_chat_srv_p_cache_worker_ctx_t *p_ctx)
{

	p_ctx->projects_updated_at         = 0;
	p_ctx->users_updated_at            = 0;
	p_ctx->issue_priorities_updated_at = 0;
}

void nxs_chat_srv_p_cache_worker_ctx_free(nxs_chat_srv_p_cache_worker_ctx_t *p_ctx)
{
}

/* Module internal (static) functions */
