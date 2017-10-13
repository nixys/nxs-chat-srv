#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_ISSUES_C
#define _INCLUDE_NXS_CHAT_SRV_D_DB_ISSUES_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/db-issues/db-issues-typedefs.h>
#include <dal/db-issues/db-issues.h>
#include <dal/db-issues/db-issues-subdals.h>

/* Definitions */

#define NXS_CHAT_SRV_D_DB_ISSUES_REDIS_PREFIX	"nxs-chat-srv:issues"

/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_d_db_issues_s
{
	nxs_mysql_t				mysql_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_d_db_issues_t *nxs_chat_srv_d_db_issues_init(void)
{
	nxs_chat_srv_d_db_issues_t *d_ctx;

	d_ctx = (nxs_chat_srv_d_db_issues_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_d_db_issues_t));

	/* Connect to MySQL */

	if(nxs_chat_srv_c_mysql_connect(&d_ctx->mysql_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: db issues error, can't connect to MySQL: \"%s\"",
		                    nxs_proc_get_name(&process),
		                    d_ctx->mysql_ctx.err_str);

		return nxs_chat_srv_d_db_issues_free(d_ctx);
	}

	return d_ctx;
}

nxs_chat_srv_d_db_issues_t *nxs_chat_srv_d_db_issues_free(nxs_chat_srv_d_db_issues_t *d_ctx)
{

	if(d_ctx == NULL) {

		return NULL;
	}

	nxs_chat_srv_c_mysql_disconnect(&d_ctx->mysql_ctx);

	return (nxs_chat_srv_d_db_issues_t *)nxs_free(d_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_issues_get(nxs_chat_srv_d_db_issues_t *d_ctx,
                                                size_t                      tlgrm_chat_id,
                                                size_t                      tlgrm_message_id,
                                                size_t *                    rdmn_issue_id)
{
	nxs_mysql_res_t    res;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL || rdmn_issue_id == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_mysql_res_init(&res);

	if(nxs_mysql_query(&d_ctx->mysql_ctx,
	                   NXS_MYSQL_QUERY_TYPE_SELECT,
	                   &res,
	                   "SELECT "
	                   "	`rdmn_issue_id` "
	                   "FROM "
	                   "	`issues` "
	                   "WHERE "
	                   "	`tlgrm_chat_id` = %zu "
	                   "	AND `tlgrm_message_id` = %zu",
	                   tlgrm_chat_id,
	                   tlgrm_message_id) != NXS_MYSQL_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: db issues get error, MySQL select error: \"%s\" (tlgrm chat id: %zu, tlgrm message id: %zu)",
		                    nxs_proc_get_name(&process),
		                    d_ctx->mysql_ctx.err_str,
		                    tlgrm_chat_id,
		                    tlgrm_message_id);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_mysql_res_rows(&res) == 0) {

		nxs_log_write_debug(&process,
		                    "[%s]: db issues get: value does not exist (tlgrm chat id: %zu, tlgrm message id: %zu)",
		                    nxs_proc_get_name(&process),
		                    tlgrm_chat_id,
		                    tlgrm_message_id);

		nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);
	}

	*rdmn_issue_id = nxs_string_atoi(nxs_mysql_res_get(&res, 0, 0));

	nxs_log_write_debug(&process,
	                    "[%s]: db issues get: success (tlgrm chat id: %zu, tlgrm message id: %zu)",
	                    nxs_proc_get_name(&process),
	                    tlgrm_chat_id,
	                    tlgrm_message_id);

error:

	nxs_mysql_res_free(&res);

	return rc;
}

nxs_chat_srv_err_t
        nxs_chat_srv_d_db_issues_put(nxs_chat_srv_d_db_issues_t *d_ctx, size_t tlgrm_chat_id, size_t tlgrm_message_id, size_t rdmn_issue_id)
{
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if(nxs_mysql_query(&d_ctx->mysql_ctx,
	                   NXS_MYSQL_QUERY_TYPE_INSERT,
	                   NULL,
	                   "INSERT INTO"
	                   "	`issues` (`tlgrm_chat_id`, `tlgrm_message_id`, `rdmn_issue_id`) "
	                   "VALUES (%zu, %zu, %zu)",
	                   tlgrm_chat_id,
	                   tlgrm_message_id,
	                   rdmn_issue_id) != NXS_MYSQL_E_OK) {

		nxs_log_write_error(
		        &process,
		        "[%s]: db issues put error, MySQL insert error: %s (tlgrm chat id: %zu, tlgrm message id: %zu, rdmn issue id: %zu)",
		        nxs_proc_get_name(&process),
		        d_ctx->mysql_ctx.err_str,
		        tlgrm_chat_id,
		        tlgrm_message_id,
		        rdmn_issue_id);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_log_write_debug(&process,
	                    "[%s]: db issues put: success (tlgrm chat id: %zu, tlgrm message id: %zu, rdmn issue id: %zu)",
	                    nxs_proc_get_name(&process),
	                    tlgrm_chat_id,
	                    tlgrm_message_id,
	                    rdmn_issue_id);

error:

	return rc;
}

/* Module internal (static) functions */
