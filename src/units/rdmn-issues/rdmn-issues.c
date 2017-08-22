#ifndef _INCLUDE_NXS_CHAT_SRV_U_RDMN_ISSUES_C
#define _INCLUDE_NXS_CHAT_SRV_U_RDMN_ISSUES_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/rdmn-issues/rdmn-issues-typedefs.h>
#include <units/rdmn-issues/rdmn-issues.h>
#include <units/rdmn-issues/rdmn-issues-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_u_rdmn_issues_s
{
	size_t			issue_id;
	nxs_string_t		notes;
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_u_rdmn_issues_t *nxs_chat_srv_u_rdmn_issues_init(void)
{
	nxs_chat_srv_u_rdmn_issues_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_rdmn_issues_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_rdmn_issues_t));

	u_ctx->issue_id = 0;

	nxs_string_init(&u_ctx->notes);

	return u_ctx;
}

nxs_chat_srv_u_rdmn_issues_t *nxs_chat_srv_u_rdmn_issues_free(nxs_chat_srv_u_rdmn_issues_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	u_ctx->issue_id = 0;

	nxs_string_free(&u_ctx->notes);

	return (nxs_chat_srv_u_rdmn_issues_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t
        nxs_chat_srv_u_rdmn_issues_add_note(nxs_chat_srv_u_rdmn_issues_t *u_ctx, size_t issue_id, nxs_string_t *note, nxs_string_t *api_key)
{
	nxs_buf_t          out_buf;
	nxs_chat_srv_err_t rc;
	nxs_http_code_t    http_code;

	nxs_buf_init(&out_buf, 1);

	rc = nxs_chat_srv_d_rdmn_issues_add_comment(issue_id, note, api_key, &out_buf, &http_code);

	nxs_buf_free(&out_buf);

	return rc;
}

/* Module internal (static) functions */
