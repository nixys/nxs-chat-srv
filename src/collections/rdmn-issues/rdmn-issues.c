#ifndef _INCLUDE_NXS_CHAT_SRV_C_RDMN_ISSUES_C
#define _INCLUDE_NXS_CHAT_SRV_C_RDMN_ISSUES_C
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



/* Module global functions */

// clang-format on

void nxs_chat_srv_c_rdmn_issues_init(nxs_array_t *issues)
{

	if(issues == NULL) {

		return;
	}

	nxs_array_init2(issues, nxs_chat_srv_m_rdmn_issues_query_t);
}

void nxs_chat_srv_c_rdmn_issues_free(nxs_array_t *issues)
{
	nxs_chat_srv_m_rdmn_issues_query_t *iss;
	size_t                              i;

	if(issues == NULL) {

		return;
	}

	for(i = 0; i < nxs_array_count(issues); i++) {

		iss = nxs_array_get(issues, i);

		iss->id = 0;

		nxs_string_free(&iss->subject);
	}

	nxs_array_free(issues);
}

/* Module internal (static) functions */
