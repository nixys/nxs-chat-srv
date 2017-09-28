#ifndef _INCLUDE_NXS_CHAT_SRV_C_MYSQL_C
#define _INCLUDE_NXS_CHAT_SRV_C_MYSQL_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */

static nxs_string_t _s_mysql_charset	= nxs_string(NXS_MYSQL_CONN_CHARSET_DEFAULT);

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_c_mysql_connect(nxs_mysql_t *mysql)
{

	if(nxs_mysql_init(mysql,
	                  &nxs_chat_srv_cfg.mysql.host,
	                  &nxs_chat_srv_cfg.mysql.name,
	                  &nxs_chat_srv_cfg.mysql.user,
	                  &nxs_chat_srv_cfg.mysql.pass,
	                  nxs_chat_srv_cfg.mysql.port,
	                  &_s_mysql_charset) != NXS_MYSQL_E_OK) {

		nxs_log_write_error(&process, "[%s]: MySQL init error: \"%s\"", nxs_proc_get_name(&process), (char *)mysql->err_str);

		nxs_mysql_free(mysql);

		return NXS_CHAT_SRV_E_ERR;
	}

	if(nxs_mysql_connect(mysql) != NXS_MYSQL_E_OK) {

		nxs_log_write_error(&process, "[%s]: MySQL connect error: \"%s\"", nxs_proc_get_name(&process), mysql->err_str);

		nxs_mysql_disconnect(mysql);
		nxs_mysql_free(mysql);

		return NXS_CHAT_SRV_E_ERR;
	}

	return NXS_CHAT_SRV_E_OK;
}

nxs_chat_srv_err_t nxs_chat_srv_c_mysql_disconnect(nxs_mysql_t *mysql)
{

	if(nxs_mysql_disconnect(mysql) != NXS_MYSQL_E_OK) {

		nxs_log_write_error(&process, "[%s]: MySQL disconnect error: \"%s\"", nxs_proc_get_name(&process), (char *)mysql->err_str);

		nxs_mysql_free(mysql);

		return NXS_CHAT_SRV_E_ERR;
	}

	nxs_mysql_free(mysql);

	return NXS_CHAT_SRV_E_OK;
}

nxs_chat_srv_err_t nxs_chat_srv_c_mysql_transaction_start(nxs_mysql_t *mysql)
{

	if(nxs_mysql_query(mysql, NXS_MYSQL_QUERY_TYPE_UPDATE, NULL, "START TRANSACTION") != NXS_MYSQL_E_OK) {

		nxs_log_write_error(
		        &process, "[%s]: MySQL start transaction error: \"%s\"", nxs_proc_get_name(&process), (char *)mysql->err_str);

		return NXS_CHAT_SRV_E_ERR;
	}

	return NXS_CHAT_SRV_E_OK;
}

nxs_chat_srv_err_t nxs_chat_srv_c_mysql_transaction_commit(nxs_mysql_t *mysql)
{

	if(nxs_mysql_query(mysql, NXS_MYSQL_QUERY_TYPE_UPDATE, NULL, "COMMIT") != NXS_MYSQL_E_OK) {

		nxs_log_write_error(
		        &process, "[%s]: MySQL transaction commit error: \"%s\"", nxs_proc_get_name(&process), (char *)mysql->err_str);

		return NXS_CHAT_SRV_E_ERR;
	}

	return NXS_CHAT_SRV_E_OK;
}

nxs_chat_srv_err_t nxs_chat_srv_c_mysql_transaction_rollback(nxs_mysql_t *mysql)
{

	if(nxs_mysql_query(mysql, NXS_MYSQL_QUERY_TYPE_UPDATE, NULL, "ROLLBACK") != NXS_MYSQL_E_OK) {

		nxs_log_write_error(
		        &process, "[%s]: MySQL transaction rollback error: \"%s\"", nxs_proc_get_name(&process), (char *)mysql->err_str);

		return NXS_CHAT_SRV_E_ERR;
	}

	return NXS_CHAT_SRV_E_OK;
}

/* Module internal (static) functions */
