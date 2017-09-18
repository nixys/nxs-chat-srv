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
	nxs_string_t       value;
	size_t             id;
	nxs_chat_srv_err_t rc;

	if(tlgrm_userid == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	nxs_string_init(&value);

	switch((rc = nxs_chat_srv_d_db_ids_get(u_ctx->db_ids_ctx, rdmn_userid, &value))) {

		case NXS_CHAT_SRV_E_OK:

			id = nxs_string_atoi(&value);

			break;

		case NXS_CHAT_SRV_E_EXIST:

			nxs_log_write_debug(&process,
			                    "[%s]: can't get tlgrm user id from DB: record does not exist (rdmn userid: %zu)",
			                    nxs_proc_get_name(&process),
			                    rdmn_userid);

			id = 0;

			rc = NXS_CHAT_SRV_E_OK;

			break;

		default:

			nxs_log_write_error(&process,
			                    "[%s]: can't get tlgrm user id from DB (rdmn userid: %zu)",
			                    nxs_proc_get_name(&process),
			                    rdmn_userid);

			id = 0;

			break;
	}

	*tlgrm_userid = id;

	nxs_string_free(&value);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_ids_set(nxs_chat_srv_u_db_ids_t *u_ctx, size_t rdmn_userid, size_t tlgrm_userid)
{
	nxs_string_t       value;
	nxs_chat_srv_err_t rc;

	nxs_string_init(&value);

	nxs_string_printf(&value, "%zu", tlgrm_userid);

	if((rc = nxs_chat_srv_d_db_ids_put(u_ctx->db_ids_ctx, rdmn_userid, &value)) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: can't save tlgrm user id into DB (rdmn userid: %zu, tlgrm userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    rdmn_userid,
		                    tlgrm_userid);
	}

	nxs_string_free(&value);

	return rc;
}

/* Module internal (static) functions */
