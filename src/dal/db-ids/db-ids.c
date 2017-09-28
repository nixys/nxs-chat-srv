#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_IDS_C
#define _INCLUDE_NXS_CHAT_SRV_D_DB_IDS_C
#endif

// clang-format off

#include <hiredis/hiredis.h>

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

nxs_chat_srv_err_t nxs_chat_srv_d_db_ids_put(nxs_chat_srv_d_db_ids_t *d_ctx, size_t rdmn_userid, size_t tlgrm_userid)
{
	nxs_mysql_res_t    res;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL) {

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
		                    "[%s]: db ids put error, MySQL select error: \"%s\" (rdmn userid: %zu, tlgrm userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    d_ctx->mysql_ctx.err_str,
		                    rdmn_userid,
		                    tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_mysql_res_rows(&res) == 0) {

		if(nxs_mysql_query(&d_ctx->mysql_ctx,
		                   NXS_MYSQL_QUERY_TYPE_INSERT,
		                   NULL,
		                   "INSERT INTO"
		                   "	`ids` (`rdmn_userid`, `tlgrm_userid`) "
		                   "VALUES (%zu, %zu)",
		                   rdmn_userid,
		                   tlgrm_userid) != NXS_MYSQL_E_OK) {

			nxs_log_write_error(&process,
			                    "[%s]: db ids put error, MySQL insert error: %s (rdmn userid: %zu, tlgrm userid: %zu)",
			                    nxs_proc_get_name(&process),
			                    d_ctx->mysql_ctx.err_str,
			                    rdmn_userid,
			                    tlgrm_userid);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}
	else {

		if((size_t)nxs_string_atoi(nxs_mysql_res_get(&res, 0, 0)) != tlgrm_userid) {

			if(nxs_mysql_query(&d_ctx->mysql_ctx,
			                   NXS_MYSQL_QUERY_TYPE_UPDATE,
			                   NULL,
			                   "UPDATE"
			                   "	`ids`"
			                   "SET "
			                   "	`tlgrm_userid` = %zu "
			                   "WHERE "
			                   "	`rdmn_userid` = %zu",
			                   tlgrm_userid,
			                   rdmn_userid) != NXS_MYSQL_E_OK) {

				nxs_log_write_error(&process,
				                    "[%s]: db ids put error, MySQL update error: %s (rdmn userid: %zu, tlgrm userid: %zu)",
				                    nxs_proc_get_name(&process),
				                    d_ctx->mysql_ctx.err_str,
				                    rdmn_userid,
				                    tlgrm_userid);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}
		}
	}

	nxs_log_write_debug(&process,
	                    "[%s]: db ids put: success (rdmn userid: %zu, tlgrm userid: %zu)",
	                    nxs_proc_get_name(&process),
	                    rdmn_userid,
	                    tlgrm_userid);

error:

	nxs_mysql_res_free(&res);

	return rc;
}

/* Module internal (static) functions */
