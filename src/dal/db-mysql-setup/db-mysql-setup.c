#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_MYSQL_SETUP_C
#define _INCLUDE_NXS_CHAT_SRV_D_DB_MYSQL_SETUP_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/db-mysql-setup/db-mysql-setup-typedefs.h>
#include <dal/db-mysql-setup/db-mysql-setup.h>
#include <dal/db-mysql-setup/db-mysql-setup-subdals.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_d_db_mysql_setup_s
{
	nxs_mysql_t		mysql_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_d_db_mysql_setup_t *nxs_chat_srv_d_db_mysql_setup_init(void)
{
	nxs_chat_srv_d_db_mysql_setup_t *d_ctx;

	d_ctx = (nxs_chat_srv_d_db_mysql_setup_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_d_db_mysql_setup_t));

	/* Connect to MySQL */

	if(nxs_chat_srv_c_mysql_connect(&d_ctx->mysql_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_warn(&process,
		                   "[%s]: db setup error, can't connect to MySQL: \"%s\"",
		                   nxs_proc_get_name(&process),
		                   d_ctx->mysql_ctx.err_str);

		return nxs_chat_srv_d_db_mysql_setup_free(d_ctx);
	}

	return d_ctx;
}

nxs_chat_srv_d_db_mysql_setup_t *nxs_chat_srv_d_db_mysql_setup_free(nxs_chat_srv_d_db_mysql_setup_t *d_ctx)
{

	if(d_ctx == NULL) {

		return NULL;
	}

	nxs_chat_srv_c_mysql_disconnect(&d_ctx->mysql_ctx);

	return (nxs_chat_srv_d_db_mysql_setup_t *)nxs_free(d_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_mysql_setup_create_table_ids(nxs_chat_srv_d_db_mysql_setup_t *d_ctx, nxs_string_t *err_str)
{
	nxs_mysql_res_t    res;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL || err_str == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_clear(err_str);

	nxs_mysql_res_init(&res);

	if(nxs_mysql_query(&d_ctx->mysql_ctx,
	                   NXS_MYSQL_QUERY_TYPE_UPDATE,
	                   &res,
	                   "CREATE TABLE "
	                   "	`ids` ("
	                   "		`rdmn_userid` int(11) NOT NULL, "
	                   "		`tlgrm_userid` int(11) NOT NULL "
	                   "		) "
	                   "ENGINE=InnoDB "
	                   "DEFAULT CHARSET=utf8") != NXS_MYSQL_E_OK) {

		nxs_log_write_warn(
		        &process, "[%s]: MySQL create table `ids` error: %s", nxs_proc_get_name(&process), d_ctx->mysql_ctx.err_str);

		nxs_string_char_cpy(err_str, 0, d_ctx->mysql_ctx.err_str);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_log_write_debug(&process, "[%s]: MySQL create table `ids`: success", nxs_proc_get_name(&process));

error:

	nxs_mysql_res_free(&res);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_mysql_setup_create_table_issues(nxs_chat_srv_d_db_mysql_setup_t *d_ctx, nxs_string_t *err_str)
{
	nxs_mysql_res_t    res;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL || err_str == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_clear(err_str);

	nxs_mysql_res_init(&res);

	if(nxs_mysql_query(&d_ctx->mysql_ctx,
	                   NXS_MYSQL_QUERY_TYPE_UPDATE,
	                   &res,
	                   "CREATE TABLE "
	                   "	`issues` ("
	                   "		`tlgrm_chat_id` int(11) NOT NULL, "
	                   "		`tlgrm_message_id` int(11) NOT NULL, "
	                   "		`rdmn_issue_id` int(11) NOT NULL "
	                   "		) "
	                   "ENGINE=InnoDB "
	                   "DEFAULT CHARSET=utf8") != NXS_MYSQL_E_OK) {

		nxs_log_write_warn(
		        &process, "[%s]: MySQL create table `issues` error: %s", nxs_proc_get_name(&process), d_ctx->mysql_ctx.err_str);

		nxs_string_char_cpy(err_str, 0, d_ctx->mysql_ctx.err_str);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_log_write_debug(&process, "[%s]: MySQL create table `issues`: success", nxs_proc_get_name(&process));

error:

	nxs_mysql_res_free(&res);

	return rc;
}

/* Module internal (static) functions */
