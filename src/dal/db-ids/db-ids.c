#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_IDS_C
#define _INCLUDE_NXS_CHAT_SRV_D_DB_IDS_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/db-ids/db-ids-typedefs.h>
#include <dal/db-ids/db-ids.h>
#include <dal/db-ids/db-ids-subdals.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_d_db_ids_s
{
	nxs_mysql_t				mysql_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_d_db_ids_t *nxs_chat_srv_d_db_ids_init(void)
{
	nxs_chat_srv_d_db_ids_t *d_ctx;

	d_ctx = (nxs_chat_srv_d_db_ids_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_d_db_ids_t));

	/* Connect to MySQL */

	if(nxs_chat_srv_c_mysql_connect(&d_ctx->mysql_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: db ids error, can't connect to MySQL: \"%s\"",
		                    nxs_proc_get_name(&process),
		                    d_ctx->mysql_ctx.err_str);

		return nxs_chat_srv_d_db_ids_free(d_ctx);
	}

	return d_ctx;
}

nxs_chat_srv_d_db_ids_t *nxs_chat_srv_d_db_ids_free(nxs_chat_srv_d_db_ids_t *d_ctx)
{

	if(d_ctx == NULL) {

		return NULL;
	}

	nxs_chat_srv_c_mysql_disconnect(&d_ctx->mysql_ctx);

	return (nxs_chat_srv_d_db_ids_t *)nxs_free(d_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_ids_transaction_start(nxs_chat_srv_d_db_ids_t *d_ctx)
{

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	return nxs_chat_srv_c_mysql_transaction_start(&d_ctx->mysql_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_ids_transaction_commit(nxs_chat_srv_d_db_ids_t *d_ctx)
{

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	return nxs_chat_srv_c_mysql_transaction_commit(&d_ctx->mysql_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_ids_transaction_rollback(nxs_chat_srv_d_db_ids_t *d_ctx)
{

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	return nxs_chat_srv_c_mysql_transaction_rollback(&d_ctx->mysql_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_ids_get(nxs_chat_srv_d_db_ids_t *d_ctx, size_t rdmn_userid, size_t *tlgrm_userid)
{
	nxs_mysql_res_t    res;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL || tlgrm_userid == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_mysql_res_init(&res);

	if(nxs_mysql_query(&d_ctx->mysql_ctx,
	                   NXS_MYSQL_QUERY_TYPE_SELECT,
	                   &res,
	                   "SELECT "
	                   "	`tlgrm_userid` "
	                   "FROM "
	                   "	`ids` "
	                   "WHERE "
	                   "	`rdmn_userid` = %zu",
	                   rdmn_userid) != NXS_MYSQL_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: db ids get error, MySQL select error: \"%s\" (rdmn userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    d_ctx->mysql_ctx.err_str,
		                    rdmn_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_mysql_res_rows(&res) == 0) {

		nxs_log_write_debug(
		        &process, "[%s]: db ids get: value does not exist (rdmn userid: %zu)", nxs_proc_get_name(&process), rdmn_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);
	}

	*tlgrm_userid = nxs_string_atoi(nxs_mysql_res_get(&res, 0, 0));

	nxs_log_write_debug(&process, "[%s]: db ids get: success (rdmn userid: %zu)", nxs_proc_get_name(&process), rdmn_userid);

error:

	nxs_mysql_res_free(&res);

	return rc;
}

nxs_chat_srv_err_t
        nxs_chat_srv_d_db_ids_cleanup_tlgrm_username(nxs_chat_srv_d_db_ids_t *d_ctx, size_t tlgrm_userid, nxs_string_t *tlgrm_username)
{
	nxs_chat_srv_err_t rc;
	nxs_string_t       t_uname_esc;

	if(d_ctx == NULL || tlgrm_username == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&t_uname_esc);

	nxs_mysql_escape_string(&d_ctx->mysql_ctx, &t_uname_esc, tlgrm_username);

	if(nxs_mysql_query(&d_ctx->mysql_ctx,
	                   NXS_MYSQL_QUERY_TYPE_UPDATE,
	                   NULL,
	                   "UPDATE "
	                   "	`ids` "
	                   "SET "
	                   "	`tlgrm_username` = '', "
	                   "	`rdmn_userid` = 0 "
	                   "WHERE "
	                   "	`tlgrm_username` = '%r' "
	                   "	AND `tlgrm_userid` <> %zu",
	                   &t_uname_esc,
	                   tlgrm_userid) != NXS_MYSQL_E_OK) {

		nxs_log_write_error(
		        &process,
		        "[%s]: db ids cleanup tlgrm username error, MySQL update error: %s (tlgrm userid: %zu, tlgrm username: %r)",
		        nxs_proc_get_name(&process),
		        d_ctx->mysql_ctx.err_str,
		        tlgrm_userid,
		        tlgrm_username);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_string_free(&t_uname_esc);

	return rc;
}

nxs_chat_srv_err_t
        nxs_chat_srv_d_db_ids_cleanup_rdmn_userid_by_tlgrm_userid(nxs_chat_srv_d_db_ids_t *d_ctx, size_t tlgrm_userid, size_t rdmn_userid)
{
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if(nxs_mysql_query(&d_ctx->mysql_ctx,
	                   NXS_MYSQL_QUERY_TYPE_UPDATE,
	                   NULL,
	                   "UPDATE "
	                   "	`ids` "
	                   "SET "
	                   "	`rdmn_userid` = 0 "
	                   "WHERE "
	                   "	`rdmn_userid` = %zu "
	                   "	AND `tlgrm_userid` <> %zu",
	                   rdmn_userid,
	                   tlgrm_userid) != NXS_MYSQL_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: db ids cleanup rdmn userid by tlgrm userid error, MySQL update error: %s (tlgrm userid: %zu, "
		                    "rdmn userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    d_ctx->mysql_ctx.err_str,
		                    tlgrm_userid,
		                    rdmn_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_ids_cleanup_rdmn_userid_by_tlgrm_username(nxs_chat_srv_d_db_ids_t *d_ctx,
                                                                               nxs_string_t *           tlgrm_username,
                                                                               size_t                   rdmn_userid)
{
	nxs_chat_srv_err_t rc;
	nxs_string_t       t_uname_esc;

	if(d_ctx == NULL || tlgrm_username == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&t_uname_esc);

	nxs_mysql_escape_string(&d_ctx->mysql_ctx, &t_uname_esc, tlgrm_username);

	if(nxs_mysql_query(&d_ctx->mysql_ctx,
	                   NXS_MYSQL_QUERY_TYPE_UPDATE,
	                   NULL,
	                   "UPDATE "
	                   "	`ids` "
	                   "SET "
	                   "	`rdmn_userid` = 0 "
	                   "WHERE "
	                   "	`rdmn_userid` = %zu "
	                   "	AND `tlgrm_username` <> '%r'",
	                   rdmn_userid,
	                   &t_uname_esc) != NXS_MYSQL_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: db ids cleanup rdmn userid by tlgrm username error, MySQL update error: %s (tlgrm username: %r, "
		                    "rdmn userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    d_ctx->mysql_ctx.err_str,
		                    tlgrm_username,
		                    rdmn_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_string_free(&t_uname_esc);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_ids_put_tlgrm(nxs_chat_srv_d_db_ids_t *d_ctx,
                                                   size_t                   tlgrm_userid,
                                                   nxs_string_t *           tlgrm_username,
                                                   size_t                   rdmn_userid)
{
	nxs_chat_srv_err_t rc;
	nxs_string_t       t_uname_esc;
	nxs_mysql_res_t    res;

	if(d_ctx == NULL || tlgrm_username == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_mysql_res_init(&res);

	nxs_string_init(&t_uname_esc);

	nxs_mysql_escape_string(&d_ctx->mysql_ctx, &t_uname_esc, tlgrm_username);

	if(nxs_mysql_query(&d_ctx->mysql_ctx,
	                   NXS_MYSQL_QUERY_TYPE_SELECT,
	                   &res,
	                   "SELECT "
	                   "	`tlgrm_userid` "
	                   "FROM "
	                   "	`ids` "
	                   "WHERE "
	                   "	`tlgrm_userid` = %zu",
	                   tlgrm_userid) != NXS_MYSQL_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: db ids put tlgrm error, MySQL select error: \"%s\" (tlgrm userid: %zu, tlgrm username: %r, rdmn "
		                    "userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    d_ctx->mysql_ctx.err_str,
		                    tlgrm_userid,
		                    tlgrm_username,
		                    rdmn_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_mysql_res_rows(&res) == 0) {

		if(nxs_mysql_query(&d_ctx->mysql_ctx,
		                   NXS_MYSQL_QUERY_TYPE_INSERT,
		                   NULL,
		                   "INSERT INTO "
		                   "	`ids` (`tlgrm_userid`, `tlgrm_username`, `rdmn_userid`) "
		                   "VALUES (%zu, '%r', %zu)",
		                   tlgrm_userid,
		                   &t_uname_esc,
		                   rdmn_userid) != NXS_MYSQL_E_OK) {

			nxs_log_write_error(&process,
			                    "[%s]: db ids put tlgrm error, MySQL insert error: %s (tlgrm userid: %zu, tlgrm username: %r, "
			                    "rdmn userid: %zu)",
			                    nxs_proc_get_name(&process),
			                    d_ctx->mysql_ctx.err_str,
			                    tlgrm_userid,
			                    tlgrm_username,
			                    rdmn_userid);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}
	else {

		if(nxs_mysql_query(&d_ctx->mysql_ctx,
		                   NXS_MYSQL_QUERY_TYPE_UPDATE,
		                   NULL,
		                   "UPDATE "
		                   "	`ids`"
		                   "SET "
		                   "	`tlgrm_username` = '%r', "
		                   "	`rdmn_userid` = %zu "
		                   "WHERE "
		                   "	`tlgrm_userid` = %zu",
		                   &t_uname_esc,
		                   rdmn_userid,
		                   tlgrm_userid) != NXS_MYSQL_E_OK) {

			nxs_log_write_error(&process,
			                    "[%s]: db ids put tlgrm error, MySQL update error: %s (tlgrm userid: %zu, tlgrm "
			                    "username: %r, rdmn userid: %zu)",
			                    nxs_proc_get_name(&process),
			                    d_ctx->mysql_ctx.err_str,
			                    tlgrm_userid,
			                    tlgrm_username,
			                    rdmn_userid);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}

error:

	nxs_mysql_res_free(&res);

	nxs_string_free(&t_uname_esc);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_ids_put_rdmn(nxs_chat_srv_d_db_ids_t *d_ctx, nxs_string_t *tlgrm_username, size_t rdmn_userid)
{
	nxs_chat_srv_err_t rc;
	nxs_string_t       t_uname_esc;

	if(d_ctx == NULL || tlgrm_username == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&t_uname_esc);

	nxs_mysql_escape_string(&d_ctx->mysql_ctx, &t_uname_esc, tlgrm_username);

	if(nxs_mysql_query(&d_ctx->mysql_ctx,
	                   NXS_MYSQL_QUERY_TYPE_UPDATE,
	                   NULL,
	                   "UPDATE "
	                   "	`ids`"
	                   "SET "
	                   "	`rdmn_userid` = %zu "
	                   "WHERE "
	                   "	`tlgrm_username` = '%r'",
	                   rdmn_userid,
	                   &t_uname_esc) != NXS_MYSQL_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: db ids put rdmn error, MySQL update error: %s (tlgrm username: %r, "
		                    "rdmn userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    d_ctx->mysql_ctx.err_str,
		                    tlgrm_username,
		                    rdmn_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_string_free(&t_uname_esc);

	return rc;
}

/* Module internal (static) functions */
