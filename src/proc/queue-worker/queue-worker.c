#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/queue-worker/queue-worker.h>
#include <proc/queue-worker/ctx/queue-worker-ctx.h>
#include <proc/queue-worker/queue-worker-subproc.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_process(nxs_chat_srv_p_queue_worker_ctx_t *p_ctx);

static void nxs_chat_srv_p_queue_worker_sighandler_term(int sig, void *data);
static void nxs_chat_srv_p_queue_worker_sighandler_usr1(int sig, void *data);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_runtime(void)
{
	nxs_chat_srv_p_queue_worker_ctx_t p_ctx;
	nxs_chat_srv_err_t                rc, ec;

	rc = NXS_CHAT_SRV_E_OK;

	if(nxs_chat_srv_p_queue_worker_ctx_init(&p_ctx) != NXS_CHAT_SRV_E_OK) {

		return NXS_CHAT_SRV_E_ERR;
	}

	nxs_proc_signal_set(
	        &process, SIGTERM, NXS_PROCESS_SA_FLAG_EMPTY, &nxs_chat_srv_p_queue_worker_sighandler_term, &p_ctx, NXS_PROCESS_FORCE_NO);
	nxs_proc_signal_set(
	        &process, SIGUSR1, NXS_PROCESS_SA_FLAG_EMPTY, &nxs_chat_srv_p_queue_worker_sighandler_usr1, &p_ctx, NXS_PROCESS_FORCE_NO);

	nxs_proc_signal_set(&process, SIGINT, NXS_PROCESS_SA_FLAG_EMPTY, NXS_SIG_IGN, NULL, NXS_PROCESS_FORCE_NO);

	while(1) {

		nxs_proc_signal_block(&process, SIGTERM, SIGUSR1, NXS_PROCESS_SIG_END_ARGS);

		while((ec = nxs_chat_srv_p_queue_worker_process(&p_ctx)) == NXS_CHAT_SRV_E_OK) {

			/* continue */
		};

		if(ec != NXS_CHAT_SRV_E_TIMEOUT) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		nxs_proc_signal_unblock(&process, SIGUSR1, SIGTERM, NXS_PROCESS_SIG_END_ARGS);
	}

error:

	nxs_chat_srv_p_queue_worker_ctx_free(&p_ctx);

	return rc;
}

/* Module internal (static) functions */

static nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_process(nxs_chat_srv_p_queue_worker_ctx_t *p_ctx)
{
	nxs_string_t       data;
	nxs_chat_srv_err_t rc;

	if(p_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&data);

	switch(nxs_chat_srv_c_unix_sock_recv(nxs_chat_srv_p_queue_worker_ctx_get_sock(p_ctx), &data)) {

		case NXS_CHAT_SRV_E_OK:

			break;

		case NXS_CHAT_SRV_E_TIMEOUT:

			nxs_error(rc, NXS_CHAT_SRV_E_TIMEOUT, error);

		default:

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

/* TODO: request processing */

error:

	nxs_string_free(&data);

	return rc;
}

static void nxs_chat_srv_p_queue_worker_sighandler_term(int sig, void *data)
{
	nxs_chat_srv_p_queue_worker_ctx_t *p_ctx = data;

	nxs_log_write_debug(&process, "[%s]: got TERM, terminating process", nxs_proc_get_name(&process));

	nxs_chat_srv_p_queue_worker_ctx_free(p_ctx);

	exit(EXIT_SUCCESS);
}

static void nxs_chat_srv_p_queue_worker_sighandler_usr1(int sig, void *data)
{

	nxs_log_write_debug(&process, "[%s]: got SIGUSR1, going to reopen log-file", nxs_proc_get_name(&process));

	nxs_log_reopen(&process);
}
