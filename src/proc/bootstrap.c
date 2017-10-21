#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-proc.h>

#include <proc/bootstrap-subproc.h>

/* Definitions */


/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */

typedef struct
{
	nxs_array_t		queue_worker_pids;	/* type: pid_t */
	pid_t			cache_worker_pid;
	pid_t			rest_api_pid;
} nxs_chat_srv_p_bootstrap_ctx_t;

/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_err_t nxs_chat_srv_p_bootstrap_write_pidfile(void);

static void nxs_chat_srv_p_bootstrap_sighandler_term(int sig, void *data);
static void nxs_chat_srv_p_bootstrap_sighandler_int(int sig, void *data);
static void nxs_chat_srv_p_bootstrap_sighandler_child(int sig, void *data);
static void nxs_chat_srv_p_bootstrap_sighandler_usr1(int sig, void *data);

static void nxs_chat_srv_p_bootstrap_term(nxs_chat_srv_p_bootstrap_ctx_t *p_ctx);
static void nxs_chat_srv_p_bootstrap_cleanup(nxs_chat_srv_p_bootstrap_ctx_t *p_ctx);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_bootstrap(void)
{
	nxs_chat_srv_err_t             rc;
	nxs_chat_srv_p_bootstrap_ctx_t p_ctx;
	size_t                         i;
	pid_t *                        pid;

	if(nxs_chat_srv_cfg.proc.daemonize == NXS_YES) {

		nxs_proc_daemonize(&process, NXS_PROCESS_CHLD_SIG_CLEAR, NULL, NXS_PROCESS_NO_UID, NXS_PROCESS_NO_UMASK, NULL);
	}

	if(nxs_chat_srv_p_bootstrap_write_pidfile() != NXS_CHAT_SRV_E_OK) {

		return NXS_CHAT_SRV_E_ERR;
	}

	nxs_array_init(&p_ctx.queue_worker_pids, nxs_chat_srv_cfg.ra_queue.queue_workers, sizeof(pid_t), 1);

	p_ctx.cache_worker_pid = 0;
	p_ctx.rest_api_pid     = 0;

	nxs_proc_signal_set(
	        &process, SIGTERM, NXS_PROCESS_SA_FLAG_EMPTY, &nxs_chat_srv_p_bootstrap_sighandler_term, &p_ctx, NXS_PROCESS_FORCE_NO);
	nxs_proc_signal_set(
	        &process, SIGINT, NXS_PROCESS_SA_FLAG_EMPTY, &nxs_chat_srv_p_bootstrap_sighandler_int, &p_ctx, NXS_PROCESS_FORCE_NO);
	nxs_proc_signal_set(
	        &process, SIGCHLD, NXS_PROCESS_SA_FLAG_EMPTY, &nxs_chat_srv_p_bootstrap_sighandler_child, &p_ctx, NXS_PROCESS_FORCE_NO);
	nxs_proc_signal_set(
	        &process, SIGUSR1, NXS_PROCESS_SA_FLAG_EMPTY, &nxs_chat_srv_p_bootstrap_sighandler_usr1, &p_ctx, NXS_PROCESS_FORCE_NO);

	nxs_proc_signal_block(&process, SIGINT, SIGTERM, SIGCHLD, SIGUSR1, NXS_PROCESS_SIG_END_ARGS);

	/*
	 * Starting queue worker
	 */

	for(i = 0; i < nxs_chat_srv_cfg.ra_queue.queue_workers; i++) {

		pid = nxs_array_add(&p_ctx.queue_worker_pids);

		if((*pid = nxs_proc_fork(&process, NXS_PROCESS_CHLD_SIG_CLEAR, (u_char *)"queue-worker")) == NXS_PROCESS_FORK_ERR) {

			nxs_chat_srv_p_bootstrap_term(&p_ctx);

			return NXS_CHAT_SRV_E_ERR;
		}
		else {

			if(*pid == NXS_PROCESS_FORK_CHILD) {

				nxs_chat_srv_p_bootstrap_cleanup(&p_ctx);

				return nxs_chat_srv_p_queue_worker_runtime();
			}

			nxs_log_write_debug(
			        &process, "[%s]: started queue worker process (index: %zu, pid: %d)", nxs_proc_get_name(&process), i, *pid);
		}
	}

	/*
	 * Starting cache worker
	 */

	if((p_ctx.cache_worker_pid = nxs_proc_fork(&process, NXS_PROCESS_CHLD_SIG_CLEAR, (u_char *)"cache-worker")) ==
	   NXS_PROCESS_FORK_ERR) {

		nxs_chat_srv_p_bootstrap_term(&p_ctx);

		return NXS_CHAT_SRV_E_ERR;
	}
	else {

		if(p_ctx.cache_worker_pid == NXS_PROCESS_FORK_CHILD) {

			nxs_chat_srv_p_bootstrap_cleanup(&p_ctx);

			return nxs_chat_srv_p_cache_worker_runtime();
		}

		nxs_log_write_debug(
		        &process, "[%s]: started cache worker process (pid: %d)", nxs_proc_get_name(&process), p_ctx.cache_worker_pid);
	}

	/*
	 * Starting rest-api
	 */

	if((p_ctx.rest_api_pid = nxs_proc_fork(&process, NXS_PROCESS_CHLD_SIG_CLEAR, (u_char *)"rest-api")) == NXS_PROCESS_FORK_ERR) {

		nxs_chat_srv_p_bootstrap_term(&p_ctx);

		return NXS_CHAT_SRV_E_ERR;
	}
	else {

		if(p_ctx.rest_api_pid == NXS_PROCESS_FORK_CHILD) {

			nxs_chat_srv_p_bootstrap_cleanup(&p_ctx);

			return nxs_chat_srv_p_rest_api_runtime();
		}

		nxs_log_write_debug(&process, "[%s]: started rest api process (pid: %d)", nxs_proc_get_name(&process), p_ctx.rest_api_pid);
	}

	nxs_proc_signal_unblock(&process, SIGUSR1, SIGCHLD, SIGTERM, SIGINT, NXS_PROCESS_SIG_END_ARGS);

	while(1) {

		sleep(1);
	}

	return rc;
}

/* Module internal (static) functions */

static nxs_chat_srv_err_t nxs_chat_srv_p_bootstrap_write_pidfile(void)
{
	nxs_string_t       pid_str;
	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&pid_str);

	/*
	 * Check existing pid-file
	 */

	switch(nxs_fs_read_file_to_str(&nxs_chat_srv_cfg.proc.pid_file, &pid_str)) {

		case NXS_FS_E_FILE_OPEN:

			/* File does not exist */

			break;

		case NXS_FS_E_FILE_BUF:

			nxs_log_write_error(&process,
			                    "[%s]: can't write pid file: NULL pointer (pid file: %r)",
			                    nxs_proc_get_name(&process),
			                    &nxs_chat_srv_cfg.proc.pid_file);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);

		case NXS_FS_E_FILE_READ:

			nxs_log_write_error(&process,
			                    "[%s]: can't write pid file: existing pid file read error (pid file: %r, error: %s)",
			                    nxs_proc_get_name(&process),
			                    &nxs_chat_srv_cfg.proc.pid_file,
			                    strerror(errno));

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);

		case NXS_FS_E_FILE_CLOSE:

			nxs_log_write_error(&process,
			                    "[%s]: can't write pid file: existing pid file close error (pid file: %r, error: %s)",
			                    nxs_proc_get_name(&process),
			                    &nxs_chat_srv_cfg.proc.pid_file,
			                    strerror(errno));

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);

		default:

			nxs_log_write_error(&process,
			                    "[%s]: can't write pid file: pid file already exist, check process with pid %r (pid file: %r)",
			                    nxs_proc_get_name(&process),
			                    &nxs_chat_srv_cfg.proc.pid_file,
			                    &pid_str);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	/*
	 * Write new pid-file
	 */

	nxs_string_printf(&pid_str, "%d", nxs_proc_get_pid(&process));

	switch(nxs_fs_write_file(&nxs_chat_srv_cfg.proc.pid_file, (nxs_buf_t *)&pid_str, 0640)) {

		case NXS_FS_E_FILE_OPEN:

			nxs_log_write_error(&process,
			                    "[%s]: can't write pid file: can't open file for write (pid file: %r, error: %s)",
			                    nxs_proc_get_name(&process),
			                    &nxs_chat_srv_cfg.proc.pid_file,
			                    strerror(errno));

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);

		case NXS_FS_E_FILE_WRITE:

			nxs_log_write_error(&process,
			                    "[%s]: can't write pid file: write error (pid file: %r, error: %s)",
			                    &nxs_chat_srv_cfg.proc.pid_file,
			                    strerror(errno));

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);

		case NXS_FS_E_FILE_CLOSE:

			nxs_log_write_error(&process,
			                    "c[%s]: an't write pid file: close error (pid file: %r, error: %s)",
			                    nxs_proc_get_name(&process),
			                    &nxs_chat_srv_cfg.proc.pid_file,
			                    strerror(errno));

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_log_write_debug(&process,
	                    "[%s]: pid file successfully created (pid file: %r)",
	                    nxs_proc_get_name(&process),
	                    &nxs_chat_srv_cfg.proc.pid_file);

error:

	nxs_string_free(&pid_str);

	return rc;
}

static void nxs_chat_srv_p_bootstrap_sighandler_term(int sig, void *data)
{
	nxs_chat_srv_p_bootstrap_ctx_t *p_ctx = data;

	nxs_log_write_debug(&process, "[%s]: got signal TERM, terminating daemon", nxs_proc_get_name(&process));

	nxs_chat_srv_p_bootstrap_term(p_ctx);

	exit(EXIT_SUCCESS);
}

static void nxs_chat_srv_p_bootstrap_sighandler_int(int sig, void *data)
{
	nxs_chat_srv_p_bootstrap_ctx_t *p_ctx = data;

	nxs_log_write_debug(&process, "[%s]: got signal INT, terminating daemon", nxs_proc_get_name(&process));

	nxs_chat_srv_p_bootstrap_term(p_ctx);

	exit(EXIT_SUCCESS);
}

static void nxs_chat_srv_p_bootstrap_sighandler_child(int sig, void *data)
{
	nxs_chat_srv_p_bootstrap_ctx_t *p_ctx = data;
	nxs_bool_t                      f;
	size_t                          i;
	pid_t                           cpid, *pid;
	int                             chld_status;

	nxs_log_write_debug(&process, "[%s]: got signal CHLD, terminating daemon", nxs_proc_get_name(&process));

	while((cpid = waitpid(-1, &chld_status, WNOHANG)) > 0) {

		for(f = NXS_NO, i = 0; i < nxs_array_count(&p_ctx->queue_worker_pids); i++) {

			pid = nxs_array_get(&p_ctx->queue_worker_pids, i);

			if(cpid == *pid) {

				nxs_log_write_error(
				        &process,
				        "[%s]: queue worker was finished unexpectedly, program will terminate (index: %zu, pid: "
				        "%d, status: %d)",
				        nxs_proc_get_name(&process),
				        i,
				        cpid,
				        WEXITSTATUS(chld_status));

				*pid = 0;

				f = NXS_YES;

				break;
			}
		}

		if(f == NXS_YES) {

			continue;
		}

		if(cpid == p_ctx->rest_api_pid) {

			nxs_log_write_error(&process,
			                    "[%s]: rest api was finished unexpectedly, program will terminate (pid: %d, status: %d)",
			                    nxs_proc_get_name(&process),
			                    cpid,
			                    WEXITSTATUS(chld_status));

			p_ctx->rest_api_pid = 0;

			continue;
		}

		if(cpid == p_ctx->cache_worker_pid) {

			nxs_log_write_error(&process,
			                    "[%s]: cache worker was finished unexpectedly, program will terminate (pid: "
			                    "%d, status: %d)",
			                    nxs_proc_get_name(&process),
			                    cpid,
			                    WEXITSTATUS(chld_status));

			p_ctx->cache_worker_pid = 0;

			continue;
		}

		nxs_log_write_warn(&process,
		                   "[%s]: unknown process was finished (pid: %d, status: %d)",
		                   nxs_proc_get_name(&process),
		                   cpid,
		                   WEXITSTATUS(chld_status));
	}

	switch(cpid) {

		case 0:
			nxs_log_write_debug(&process, "[%s]: no more childs processes finished", nxs_proc_get_name(&process));

			break;

		case -1:

			nxs_log_write_error(&process, "[%s]: waitpid error: %s", nxs_proc_get_name(&process), strerror(errno));

			break;
	}

	nxs_chat_srv_p_bootstrap_term(p_ctx);

	exit(EXIT_FAILURE);
}

static void nxs_chat_srv_p_bootstrap_sighandler_usr1(int sig, void *data)
{
	nxs_chat_srv_p_bootstrap_ctx_t *p_ctx = data;
	size_t                          i;
	pid_t *                         pid;

	nxs_log_write_info(&process, "[%s]: got SIGUSR1, going to reopen log-file", nxs_proc_get_name(&process));

	for(i = 0; i < nxs_array_count(&p_ctx->queue_worker_pids); i++) {

		pid = nxs_array_get(&p_ctx->queue_worker_pids, i);

		nxs_log_write_debug(
		        &process, "[%s]: sending SIGUSR1 to queue worker (index: %zu, pid: %d)", nxs_proc_get_name(&process), i, *pid);
		nxs_proc_kill(*pid, SIGUSR1);
	}

	nxs_log_write_debug(
	        &process, "[%s]: sending SIGUSR1 to cache worker (pid: %d)", nxs_proc_get_name(&process), p_ctx->cache_worker_pid);
	nxs_proc_kill(p_ctx->cache_worker_pid, SIGUSR1);

	nxs_log_write_debug(
	        &process, "[%s]: sending SIGUSR1 to rest-api process (pid: %d)", nxs_proc_get_name(&process), p_ctx->rest_api_pid);
	nxs_proc_kill(p_ctx->rest_api_pid, SIGUSR1);

	nxs_log_reopen(&process);
}

static void nxs_chat_srv_p_bootstrap_term(nxs_chat_srv_p_bootstrap_ctx_t *p_ctx)
{
	size_t i;
	pid_t *pid;

	int chld_status;

	nxs_proc_signal_block(&process, SIGINT, SIGTERM, SIGCHLD, SIGUSR1, NXS_PROCESS_SIG_END_ARGS);

	if(p_ctx->rest_api_pid > 0) {

		nxs_proc_term_pid(p_ctx->rest_api_pid, nxs_chat_srv_cfg.proc.rest_api_term_timeout, &chld_status);
	}

	for(i = 0; i < nxs_array_count(&p_ctx->queue_worker_pids); i++) {

		pid = nxs_array_get(&p_ctx->queue_worker_pids, i);

		if(*pid > 0) {

			nxs_proc_term_pid(*pid, 1000, &chld_status);
		}
	}

	if(p_ctx->cache_worker_pid > 0) {

		nxs_proc_term_pid(p_ctx->cache_worker_pid, 1000, &chld_status);
	}

	if(nxs_fs_unlink(&nxs_chat_srv_cfg.proc.pid_file) == -1) {

		nxs_log_write_warn(&process,
		                   "[%s]: can't delete pid file: %s (pidfile: \"%r\")",
		                   nxs_proc_get_name(&process),
		                   strerror(errno),
		                   &nxs_chat_srv_cfg.proc.pid_file);
	}

	nxs_log_write_info(&process, "[%s]: shutdown process", nxs_proc_get_name(&process));
}

static void nxs_chat_srv_p_bootstrap_cleanup(nxs_chat_srv_p_bootstrap_ctx_t *p_ctx)
{

	p_ctx->cache_worker_pid = 0;
	p_ctx->rest_api_pid     = 0;

	nxs_array_free(&p_ctx->queue_worker_pids);
}
