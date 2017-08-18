#include <nxs-core/nxs-core.h>

// clang-format off

/* Core module system includes */



/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-conf.h>
#include <nxs-chat-srv-proc.h>

/* Core module defaults */



/* Core module static functions */

// clang-format on

// clang-format off

/* Core module global variables */

nxs_process_t process;

/*
 * Core module config file settings
 */

nxs_chat_srv_cfg_t nxs_chat_srv_cfg;

/*
 *
 * Core module functions
 *
 */

// clang-format on

int main(int argc, char *argv[])
{
	int rc;

	nxs_proc_init(&process, (u_char *)argv[0]);
	nxs_proc_setlog(&process, NXS_LOG_LEVEL_ALL, NXS_LOG_MODE_KEEP_OPEN, NXS_LOG_SYSLOG_DONT_USE, NULL);

	switch(nxs_chat_srv_conf_runtime(argc, argv)) {

		case NXS_CHAT_SRV_E_OK:

			break;

		case NXS_CHAT_SRV_E_EXIT:

			exit(EXIT_SUCCESS);

		case NXS_CHAT_SRV_E_ERR:
		default:

			exit(EXIT_FAILURE);
	}

	nxs_proc_setlog(
	        &process, nxs_chat_srv_cfg.log.level, NXS_LOG_MODE_KEEP_OPEN, NXS_LOG_SYSLOG_DONT_USE, &(nxs_chat_srv_cfg.log.path));

	rc = nxs_chat_srv_p_bootstrap();

	exit(rc);
}
