#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/setup/setup.h>
#include <proc/setup/ctx/setup-ctx.h>
#include <proc/setup/setup-subproc.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_err_t nxs_chat_srv_p_setup_set_webhook(void);
static nxs_chat_srv_err_t nxs_chat_srv_p_setup_create_mysql_tables(void);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_setup_runtime(void)
{

	if((nxs_chat_srv_cfg.init_mode & NXS_CHAT_SRV_INIT_MODE_WEBHOOK_SET) == NXS_CHAT_SRV_INIT_MODE_WEBHOOK_SET) {

		if(nxs_chat_srv_p_setup_set_webhook() != NXS_CHAT_SRV_E_OK) {

			return NXS_CHAT_SRV_E_ERR;
		}
	}

	if((nxs_chat_srv_cfg.init_mode & NXS_CHAT_SRV_INIT_MODE_MYSQL_CREATE_TABLES) == NXS_CHAT_SRV_INIT_MODE_MYSQL_CREATE_TABLES) {

		if(nxs_chat_srv_p_setup_create_mysql_tables() != NXS_CHAT_SRV_E_OK) {

			return NXS_CHAT_SRV_E_ERR;
		}
	}

	return NXS_CHAT_SRV_E_OK;
}

/* Module internal (static) functions */

static nxs_chat_srv_err_t nxs_chat_srv_p_setup_set_webhook(void)
{

	nxs_chat_srv_u_tlgrm_webhook_t *webhook_ctx;
	nxs_chat_srv_err_t              rc;
	nxs_bool_t                      status;
	nxs_string_t                    description;

	rc = NXS_CHAT_SRV_E_OK;

	webhook_ctx = nxs_chat_srv_u_tlgrm_webhook_init();

	nxs_string_init_empty(&description);

	if(nxs_chat_srv_u_tlgrm_webhook_set(webhook_ctx, &status, &description) == NXS_CHAT_SRV_E_ERR) {

		nxs_log_write_console(&process, "set telegram webhook error (see log file for details)");

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(status == NXS_YES) {

		nxs_log_write_console(&process, "set telegram webhook success: %r", &description);
	}
	else {

		nxs_log_write_console(&process, "set telegram webhook fail: %r", &description);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_string_free(&description);

	webhook_ctx = nxs_chat_srv_u_tlgrm_webhook_free(webhook_ctx);

	return rc;
}

static nxs_chat_srv_err_t nxs_chat_srv_p_setup_create_mysql_tables(void)
{
	nxs_chat_srv_u_db_mysql_setup_t *mysql_setup_ctx;
	nxs_chat_srv_err_t               rc;
	nxs_string_t                     err_str;

	mysql_setup_ctx = nxs_chat_srv_u_db_mysql_setup_init();

	nxs_string_init_empty(&err_str);

	if((rc = nxs_chat_srv_u_db_mysql_setup_create_tables(mysql_setup_ctx, &err_str)) == NXS_CHAT_SRV_E_OK) {

		nxs_log_write_console(&process, "create MySQL tables success");
	}
	else {

		nxs_log_write_console(&process, "create MySQL tables fail: %r", &err_str);
	}

	nxs_string_free(&err_str);

	mysql_setup_ctx = nxs_chat_srv_u_db_mysql_setup_free(mysql_setup_ctx);

	return rc;
}
