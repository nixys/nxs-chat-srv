#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/cache-worker/cache-worker.h>
#include <proc/cache-worker/ctx/cache-worker-ctx.h>
#include <proc/cache-worker/cache-worker-subproc.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_err_t nxs_chat_srv_p_cache_worker_process(nxs_chat_srv_p_cache_worker_ctx_t *p_ctx);
static void nxs_chat_srv_p_cache_worker_sighandler_term(int sig, void *data);
static void nxs_chat_srv_p_cache_worker_sighandler_usr1(int sig, void *data);
static nxs_bool_t nxs_chat_srv_p_cache_worker_check_lock();

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_cache_worker_runtime(void)
{
	nxs_chat_srv_p_cache_worker_ctx_t p_ctx;
	nxs_chat_srv_err_t                rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_chat_srv_p_cache_worker_ctx_init(&p_ctx);

	nxs_proc_signal_set(
	        &process, SIGTERM, NXS_PROCESS_SA_FLAG_EMPTY, &nxs_chat_srv_p_cache_worker_sighandler_term, &p_ctx, NXS_PROCESS_FORCE_NO);
	nxs_proc_signal_set(
	        &process, SIGUSR1, NXS_PROCESS_SA_FLAG_EMPTY, &nxs_chat_srv_p_cache_worker_sighandler_usr1, &p_ctx, NXS_PROCESS_FORCE_NO);

	nxs_proc_signal_set(&process, SIGINT, NXS_PROCESS_SA_FLAG_EMPTY, NXS_SIG_IGN, NULL, NXS_PROCESS_FORCE_NO);

	while(1) {

		nxs_proc_signal_block(&process, SIGTERM, SIGUSR1, NXS_PROCESS_SIG_END_ARGS);

		if(nxs_chat_srv_p_cache_worker_process(&p_ctx) != NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		nxs_proc_signal_unblock(&process, SIGUSR1, SIGTERM, NXS_PROCESS_SIG_END_ARGS);

		sleep(1);
	}

error:

	nxs_chat_srv_p_cache_worker_ctx_free(&p_ctx);

	return rc;
}

/* Module internal (static) functions */

static nxs_chat_srv_err_t nxs_chat_srv_p_cache_worker_process(nxs_chat_srv_p_cache_worker_ctx_t *p_ctx)
{
	nxs_chat_srv_u_db_cache_t *db_cache_ctx;
	nxs_chat_srv_u_presale_t * presale_ctx;
	nxs_chat_srv_u_db_queue_t *queue_ctx;
	nxs_string_t *             data;
	size_t                     i, tlgrm_userid;

	if(nxs_chat_srv_p_cache_worker_check_lock() == NXS_YES) {

		return NXS_CHAT_SRV_E_OK;
	}

	db_cache_ctx = nxs_chat_srv_u_db_cache_init();
	presale_ctx  = nxs_chat_srv_u_presale_init();
	queue_ctx    = nxs_chat_srv_u_db_queue_init();

	/* check and update projects cache */
	if(time(NULL) - p_ctx->projects_updated_at > nxs_chat_srv_cfg.cache.projects_ttl) {

		if(nxs_chat_srv_u_db_cache_update_projects(db_cache_ctx) != NXS_CHAT_SRV_E_OK) {

			nxs_log_write_warn(&process, "[%s]: projects cache update error", nxs_proc_get_name(&process));
		}
		else {

			nxs_log_write_debug(&process, "[%s]: projects cache successfully updated", nxs_proc_get_name(&process));
		}

		p_ctx->projects_updated_at = time(NULL);
	}

	/* check and update users cache */
	if(time(NULL) - p_ctx->users_updated_at > nxs_chat_srv_cfg.cache.users_ttl) {

		if(nxs_chat_srv_u_db_cache_update_users(db_cache_ctx) != NXS_CHAT_SRV_E_OK) {

			nxs_log_write_warn(&process, "[%s]: users cache update error", nxs_proc_get_name(&process));
		}
		else {

			nxs_log_write_debug(&process, "[%s]: users cache successfully updated", nxs_proc_get_name(&process));

			switch(nxs_chat_srv_u_presale_pull(presale_ctx)) {

				case NXS_CHAT_SRV_E_OK:

					nxs_log_write_debug(
					        &process, "[%s]: users cache pull waitings: success", nxs_proc_get_name(&process));

					/* move all existing data from db_waitings to db_queue */
					for(i = 0; (data = nxs_chat_srv_u_presale_w_get(presale_ctx, i, &tlgrm_userid)) != NULL; i++) {

						if(nxs_chat_srv_u_db_queue_add(
						           queue_ctx, tlgrm_userid, NXS_CHAT_SRV_M_TLGRM_UPDATE_TYPE_NONE, data) ==
						   NXS_CHAT_SRV_E_OK) {

							nxs_log_write_debug(&process,
							                    "[%s]: users cache queue add success",
							                    nxs_proc_get_name(&process));

							if(nxs_chat_srv_u_presale_w_del(presale_ctx, tlgrm_userid) == NXS_CHAT_SRV_E_OK) {

								nxs_log_write_debug(&process,
								                    "[%s]: users cache waitings delete success",
								                    nxs_proc_get_name(&process));
							}
							else {

								nxs_log_write_warn(&process,
								                   "[%s]: users cache waitings delete error",
								                   nxs_proc_get_name(&process));
							}
						}
						else {

							nxs_log_write_warn(
							        &process, "[%s]: users cache queue add error", nxs_proc_get_name(&process));
						}
					}

					/* TODO: Add here functionality to notice registered users */

					break;

				case NXS_CHAT_SRV_E_EXIST:

					nxs_log_write_debug(
					        &process, "[%s]: users cache pull waitings: no values", nxs_proc_get_name(&process));

					break;

				default:

					nxs_log_write_debug(
					        &process, "[%s]: users cache pull waitings: error", nxs_proc_get_name(&process));

					break;
			}
		}

		p_ctx->users_updated_at = time(NULL);
	}

	/* check and update issue priorities cache */
	if(time(NULL) - p_ctx->issue_priorities_updated_at > nxs_chat_srv_cfg.cache.issue_priorities_ttl) {

		if(nxs_chat_srv_u_db_cache_update_issue_priorities(db_cache_ctx) != NXS_CHAT_SRV_E_OK) {

			nxs_log_write_warn(&process, "[%s]: issue priorities cache update error", nxs_proc_get_name(&process));
		}
		else {

			nxs_log_write_debug(&process, "[%s]: issue priorities cache successfully updated", nxs_proc_get_name(&process));
		}

		p_ctx->issue_priorities_updated_at = time(NULL);
	}

	db_cache_ctx = nxs_chat_srv_u_db_cache_free(db_cache_ctx);
	presale_ctx  = nxs_chat_srv_u_presale_free(presale_ctx);
	queue_ctx    = nxs_chat_srv_u_db_queue_free(queue_ctx);

	return NXS_CHAT_SRV_E_OK;
}

static void nxs_chat_srv_p_cache_worker_sighandler_term(int sig, void *data)
{
	nxs_chat_srv_p_cache_worker_ctx_t *p_ctx = data;

	nxs_log_write_debug(&process, "[%s]: got TERM, terminating process", nxs_proc_get_name(&process));

	nxs_chat_srv_p_cache_worker_ctx_free(p_ctx);

	exit(EXIT_SUCCESS);
}

static void nxs_chat_srv_p_cache_worker_sighandler_usr1(int sig, void *data)
{

	nxs_log_write_debug(&process, "[%s]: got SIGUSR1, going to reopen log-file", nxs_proc_get_name(&process));

	nxs_log_reopen(&process);
}

/*
 * Return:
 * * NXS_YES	- locked cache
 * * NXS_NO	- unlocked cache
 */
static nxs_bool_t nxs_chat_srv_p_cache_worker_check_lock()
{
	nxs_fs_stat_t lock_file_stat;

	if(nxs_fs_lstat(&nxs_chat_srv_cfg.cache.lock_file, &lock_file_stat) < 0) {

		return NXS_NO;
	}
	else {

		return NXS_YES;
	}
}
