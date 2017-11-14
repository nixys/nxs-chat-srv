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

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_ctx_init(nxs_chat_srv_p_queue_worker_ctx_t *p_ctx)
{
	nxs_chat_srv_u_rdmn_user_t *user_ctx;
	nxs_chat_srv_err_t          rc;

	rc = NXS_CHAT_SRV_E_OK;

	user_ctx = nxs_chat_srv_u_rdmn_user_init();

	p_ctx->ra_queue_ctx = nxs_chat_srv_u_ra_queue_init();

	if(nxs_chat_srv_u_rdmn_user_pull_current(user_ctx, &nxs_chat_srv_cfg.rdmn.presale_api_key) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: queue worker process context init error: can't pull presale user by api key (rest api key: %r)",
		                    nxs_proc_get_name(&process),
		                    &nxs_chat_srv_cfg.rdmn.presale_api_key);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	p_ctx->rdmn_update_ctx.presale_rdmn_user_id = nxs_chat_srv_u_rdmn_user_get_id(user_ctx);

error:

	user_ctx = nxs_chat_srv_u_rdmn_user_free(user_ctx);

	return rc;
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
