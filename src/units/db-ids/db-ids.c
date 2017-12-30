#ifndef _INCLUDE_NXS_CHAT_SRV_U_DB_IDS_C
#define _INCLUDE_NXS_CHAT_SRV_U_DB_IDS_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/db-ids/db-ids-typedefs.h>
#include <units/db-ids/db-ids.h>
#include <units/db-ids/db-ids-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_u_db_ids_s
{
	nxs_chat_srv_d_db_ids_t			*db_ids_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_u_db_ids_t *nxs_chat_srv_u_db_ids_init(void)
{
	nxs_chat_srv_u_db_ids_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_db_ids_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_db_ids_t));

	u_ctx->db_ids_ctx = nxs_chat_srv_d_db_ids_init();

	return u_ctx;
}

nxs_chat_srv_u_db_ids_t *nxs_chat_srv_u_db_ids_free(nxs_chat_srv_u_db_ids_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	u_ctx->db_ids_ctx = nxs_chat_srv_d_db_ids_free(u_ctx->db_ids_ctx);

	return (nxs_chat_srv_u_db_ids_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_ids_get(nxs_chat_srv_u_db_ids_t *u_ctx, size_t rdmn_userid, size_t *tlgrm_userid)
{
	nxs_chat_srv_err_t rc;

	if(u_ctx == NULL || tlgrm_userid == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	switch((rc = nxs_chat_srv_d_db_ids_get(u_ctx->db_ids_ctx, rdmn_userid, tlgrm_userid))) {

		case NXS_CHAT_SRV_E_OK:

			break;

		case NXS_CHAT_SRV_E_EXIST:

			nxs_log_write_debug(&process,
			                    "[%s]: can't get tlgrm user id from DB: record does not exist (rdmn userid: %zu)",
			                    nxs_proc_get_name(&process),
			                    rdmn_userid);

			*tlgrm_userid = 0;

			rc = NXS_CHAT_SRV_E_OK;

			break;

		default:

			nxs_log_write_error(&process,
			                    "[%s]: can't get tlgrm user id from DB (rdmn userid: %zu)",
			                    nxs_proc_get_name(&process),
			                    rdmn_userid);

			*tlgrm_userid = 0;

			break;
	}

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_ids_set_tlgrm(nxs_chat_srv_u_db_ids_t *u_ctx,
                                                   size_t                   tlgrm_userid,
                                                   nxs_string_t *           tlgrm_username,
                                                   size_t                   rdmn_userid)
{
	nxs_chat_srv_err_t rc;

	if(u_ctx == NULL || tlgrm_username == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if(nxs_chat_srv_d_db_ids_transaction_start(u_ctx->db_ids_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: can't save tlgrm ids into DB: transaction start error (tlgrm userid: %zu, tlgrm username: %r, "
		                    "rdmn userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    tlgrm_userid,
		                    tlgrm_username,
		                    rdmn_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_string_len(tlgrm_username) > 0) {

		if(nxs_chat_srv_d_db_ids_cleanup_tlgrm_username(u_ctx->db_ids_ctx, tlgrm_userid, tlgrm_username) != NXS_CHAT_SRV_E_OK) {

			nxs_log_write_error(
			        &process,
			        "[%s]: can't save tlgrm ids into DB: tlgrm username cleanup error (tlgrm userid: %zu, tlgrm username: %r, "
			        "rdmn userid: %zu)",
			        nxs_proc_get_name(&process),
			        tlgrm_userid,
			        tlgrm_username,
			        rdmn_userid);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}

	if(rdmn_userid > 0) {

		if(nxs_chat_srv_d_db_ids_cleanup_rdmn_userid_by_tlgrm_userid(u_ctx->db_ids_ctx, tlgrm_userid, rdmn_userid) !=
		   NXS_CHAT_SRV_E_OK) {

			nxs_log_write_error(
			        &process,
			        "[%s]: can't save tlgrm ids into DB: rdmn userid cleanup error (tlgrm userid: %zu, tlgrm username: %r, "
			        "rdmn userid: %zu)",
			        nxs_proc_get_name(&process),
			        tlgrm_userid,
			        tlgrm_username,
			        rdmn_userid);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}

	if(nxs_chat_srv_d_db_ids_put_tlgrm(u_ctx->db_ids_ctx, tlgrm_userid, tlgrm_username, rdmn_userid) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: can't save tlgrm ids into DB: put tlgrm error (tlgrm userid: %zu, tlgrm username: %r, "
		                    "rdmn userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    tlgrm_userid,
		                    tlgrm_username,
		                    rdmn_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	if(rc == NXS_CHAT_SRV_E_OK) {

		if(nxs_chat_srv_d_db_ids_transaction_commit(u_ctx->db_ids_ctx) != NXS_CHAT_SRV_E_OK) {

			nxs_log_write_error(
			        &process,
			        "[%s]: can't save tlgrm ids into DB: transaction commit error (tlgrm userid: %zu, tlgrm username: %r, "
			        "rdmn userid: %zu)",
			        nxs_proc_get_name(&process),
			        tlgrm_userid,
			        tlgrm_username,
			        rdmn_userid);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}
	else {

		if(nxs_chat_srv_d_db_ids_transaction_rollback(u_ctx->db_ids_ctx) != NXS_CHAT_SRV_E_OK) {

			nxs_log_write_error(
			        &process,
			        "[%s]: can't save tlgrm ids into DB: transaction rollback error (tlgrm userid: %zu, tlgrm username: %r, "
			        "rdmn userid: %zu)",
			        nxs_proc_get_name(&process),
			        tlgrm_userid,
			        tlgrm_username,
			        rdmn_userid);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_ids_set_rdmn(nxs_chat_srv_u_db_ids_t *u_ctx, nxs_string_t *tlgrm_username, size_t rdmn_userid)
{

	nxs_chat_srv_err_t rc;

	if(u_ctx == NULL || tlgrm_username == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_string_len(tlgrm_username) == 0) {

		return NXS_CHAT_SRV_E_OK;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if(nxs_chat_srv_d_db_ids_transaction_start(u_ctx->db_ids_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: can't save rdmn ids into DB: transaction start error (tlgrm username: %r, "
		                    "rdmn userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    tlgrm_username,
		                    rdmn_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_d_db_ids_cleanup_rdmn_userid_by_tlgrm_username(u_ctx->db_ids_ctx, tlgrm_username, rdmn_userid) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: can't save rdmn ids into DB: rdmn userid cleanup error (tlgrm username: %r, "
		                    "rdmn userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    tlgrm_username,
		                    rdmn_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_d_db_ids_put_rdmn(u_ctx->db_ids_ctx, tlgrm_username, rdmn_userid) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: can't save rdmn ids into DB: put rdmn error (tlgrm username: %r, "
		                    "rdmn userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    tlgrm_username,
		                    rdmn_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	if(rc == NXS_CHAT_SRV_E_OK) {

		if(nxs_chat_srv_d_db_ids_transaction_commit(u_ctx->db_ids_ctx) != NXS_CHAT_SRV_E_OK) {

			nxs_log_write_error(&process,
			                    "[%s]: can't save rdmn ids into DB: transaction commit error (tlgrm username: %r, "
			                    "rdmn userid: %zu)",
			                    nxs_proc_get_name(&process),
			                    tlgrm_username,
			                    rdmn_userid);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}
	else {

		if(nxs_chat_srv_d_db_ids_transaction_rollback(u_ctx->db_ids_ctx) != NXS_CHAT_SRV_E_OK) {

			nxs_log_write_error(&process,
			                    "[%s]: can't save rdmn ids into DB: transaction rollback error (tlgrm username: %r, "
			                    "rdmn userid: %zu)",
			                    nxs_proc_get_name(&process),
			                    tlgrm_username,
			                    rdmn_userid);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}

	return rc;
}

/* Module internal (static) functions */
