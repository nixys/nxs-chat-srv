#ifndef _INCLUDE_NXS_CHAT_SRV_U_DB_MYSQL_SETUP_C
#define _INCLUDE_NXS_CHAT_SRV_U_DB_MYSQL_SETUP_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/db-mysql-setup/db-mysql-setup-typedefs.h>
#include <units/db-mysql-setup/db-mysql-setup.h>
#include <units/db-mysql-setup/db-mysql-setup-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_u_db_mysql_setup_s
{
	nxs_chat_srv_d_db_mysql_setup_t		*db_mysql_setup_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_u_db_mysql_setup_t *nxs_chat_srv_u_db_mysql_setup_init(void)
{
	nxs_chat_srv_u_db_mysql_setup_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_db_mysql_setup_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_db_mysql_setup_t));

	u_ctx->db_mysql_setup_ctx = nxs_chat_srv_d_db_mysql_setup_init();

	return u_ctx;
}

nxs_chat_srv_u_db_mysql_setup_t *nxs_chat_srv_u_db_mysql_setup_free(nxs_chat_srv_u_db_mysql_setup_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	u_ctx->db_mysql_setup_ctx = nxs_chat_srv_d_db_mysql_setup_free(u_ctx->db_mysql_setup_ctx);

	return (nxs_chat_srv_u_db_mysql_setup_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_mysql_setup_create_tables(nxs_chat_srv_u_db_mysql_setup_t *u_ctx, nxs_string_t *err_str)
{

	if(u_ctx == NULL || err_str == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_chat_srv_d_db_mysql_setup_create_table_ids(u_ctx->db_mysql_setup_ctx, err_str) != NXS_CHAT_SRV_E_OK) {

		return NXS_CHAT_SRV_E_ERR;
	}

	if(nxs_chat_srv_d_db_mysql_setup_create_table_issues(u_ctx->db_mysql_setup_ctx, err_str) != NXS_CHAT_SRV_E_OK) {

		return NXS_CHAT_SRV_E_ERR;
	}

	return NXS_CHAT_SRV_E_OK;
}

/* Module internal (static) functions */
