#ifndef _INCLUDE_NXS_CHAT_SRV_U_DB_ISSUES_C
#define _INCLUDE_NXS_CHAT_SRV_U_DB_ISSUES_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/db-issues/db-issues-typedefs.h>
#include <units/db-issues/db-issues.h>
#include <units/db-issues/db-issues-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_u_db_issues_s
{
	nxs_chat_srv_d_db_issues_t		*db_issues_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_u_db_issues_t *nxs_chat_srv_u_db_issues_init(void)
{
	nxs_chat_srv_u_db_issues_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_db_issues_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_db_issues_t));

	u_ctx->db_issues_ctx = nxs_chat_srv_d_db_issues_init();

	return u_ctx;
}

nxs_chat_srv_u_db_issues_t *nxs_chat_srv_u_db_issues_free(nxs_chat_srv_u_db_issues_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	u_ctx->db_issues_ctx = nxs_chat_srv_d_db_issues_free(u_ctx->db_issues_ctx);

	return (nxs_chat_srv_u_db_issues_t *)nxs_free(u_ctx);
}

/*
 * Get Redmine issue ID by Telegram chat ID and message ID
 */
nxs_chat_srv_err_t nxs_chat_srv_u_db_issues_get(nxs_chat_srv_u_db_issues_t *u_ctx,
                                                size_t                      tlgrm_chat_id,
                                                size_t                      tlgrm_message_id,
                                                size_t *                    rdmn_issue_id)
{
	nxs_chat_srv_err_t rc;

	if(rdmn_issue_id == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	switch((rc = nxs_chat_srv_d_db_issues_get(u_ctx->db_issues_ctx, tlgrm_chat_id, tlgrm_message_id, rdmn_issue_id))) {

		case NXS_CHAT_SRV_E_OK:

			break;

		case NXS_CHAT_SRV_E_EXIST:

			nxs_log_write_debug(
			        &process,
			        "[%s]: can't get rdmn issue id from DB: record does not exist (tlgrm chat id: %zu, tlgrm message id: %zu)",
			        nxs_proc_get_name(&process),
			        tlgrm_chat_id,
			        tlgrm_message_id);

			*rdmn_issue_id = 0;

			rc = NXS_CHAT_SRV_E_OK;

			break;

		default:

			nxs_log_write_error(&process,
			                    "[%s]: can't get rdmn issue id from DB (tlgrm chat id: %zu, tlgrm message id: %zu)",
			                    nxs_proc_get_name(&process),
			                    tlgrm_chat_id,
			                    tlgrm_message_id);

			*rdmn_issue_id = 0;

			break;
	}

	return rc;
}

nxs_chat_srv_err_t
        nxs_chat_srv_u_db_issues_set(nxs_chat_srv_u_db_issues_t *u_ctx, size_t tlgrm_chat_id, size_t tlgrm_message_id, size_t rdmn_issue_id)
{
	nxs_chat_srv_err_t rc;

	if((rc = nxs_chat_srv_d_db_issues_put(u_ctx->db_issues_ctx, tlgrm_chat_id, tlgrm_message_id, rdmn_issue_id)) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(
		        &process,
		        "[%s]: can't save tlgrm user id into DB (tlgrm chat id: %zu, tlgrm message id: %zu, rdmn issue id: %zu)",
		        nxs_proc_get_name(&process),
		        tlgrm_chat_id,
		        tlgrm_message_id,
		        rdmn_issue_id);
	}

	return rc;
}

/* Module internal (static) functions */
