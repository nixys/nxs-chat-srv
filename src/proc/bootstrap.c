#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-proc.h>

#include <proc/bootstrap-subproc.h>

/* Definitions */


/* Project globals */
extern		nxs_process_t				process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_bootstrap(void)
{
	nxs_chat_srv_err_t	rc;

	/*
	if(nxs_chat_srv_cfg.daemonized == NXS_YES){

		nxs_proc_daemonize(&process, NXS_PROCESS_CHLD_SIG_CLEAR, NULL, NXS_PROCESS_NO_UID, NXS_PROCESS_NO_UMASK, NULL);
	}
	*/

	rc = NXS_CHAT_SRV_E_OK;
	// rc = /* Call subprocess runtime function here */;

	return rc;
}

/* Module internal (static) functions */
