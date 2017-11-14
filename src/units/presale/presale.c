#ifndef _INCLUDE_NXS_CHAT_SRV_U_PRESALE_C
#define _INCLUDE_NXS_CHAT_SRV_U_PRESALE_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/presale/presale-typedefs.h>
#include <units/presale/presale.h>
#include <units/presale/presale-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_u_presale_s
{
	nxs_chat_srv_d_db_waitings_t	*db_waitings_ctx;
	nxs_chat_srv_d_db_presale_t	*db_presale_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_u_presale_t *nxs_chat_srv_u_presale_init(void)
{
	nxs_chat_srv_u_presale_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_presale_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_presale_t));

	u_ctx->db_waitings_ctx = nxs_chat_srv_d_db_waitings_init();
	u_ctx->db_presale_ctx  = nxs_chat_srv_d_db_presale_init();

	return u_ctx;
}

nxs_chat_srv_u_presale_t *nxs_chat_srv_u_presale_free(nxs_chat_srv_u_presale_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	u_ctx->db_waitings_ctx = nxs_chat_srv_d_db_waitings_free(u_ctx->db_waitings_ctx);
	u_ctx->db_presale_ctx  = nxs_chat_srv_d_db_presale_free(u_ctx->db_presale_ctx);

	return (nxs_chat_srv_u_presale_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_presale_pull(nxs_chat_srv_u_presale_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_chat_srv_d_db_presale_pull(u_ctx->db_presale_ctx) == NXS_CHAT_SRV_E_ERR) {

		return NXS_CHAT_SRV_E_ERR;
	}

	if(nxs_chat_srv_d_db_waitings_pull(u_ctx->db_waitings_ctx) == NXS_CHAT_SRV_E_ERR) {

		return NXS_CHAT_SRV_E_ERR;
	}

	return NXS_CHAT_SRV_E_OK;
}

nxs_chat_srv_err_t nxs_chat_srv_u_presale_w_set(nxs_chat_srv_u_presale_t *u_ctx, size_t tlgrm_userid, nxs_string_t *update)
{

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	return nxs_chat_srv_d_db_waitings_set(u_ctx->db_waitings_ctx, tlgrm_userid, update);
}

nxs_string_t *nxs_chat_srv_u_presale_w_get(nxs_chat_srv_u_presale_t *u_ctx, size_t i, size_t *tlgrm_userid)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	return nxs_chat_srv_d_db_waitings_get(u_ctx->db_waitings_ctx, i, tlgrm_userid);
}

nxs_chat_srv_err_t nxs_chat_srv_u_presale_w_del(nxs_chat_srv_u_presale_t *u_ctx, size_t tlgrm_userid)
{

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	return nxs_chat_srv_d_db_waitings_del(u_ctx->db_waitings_ctx, tlgrm_userid);
}

nxs_chat_srv_err_t nxs_chat_srv_u_presale_p_get(nxs_chat_srv_u_presale_t *u_ctx, size_t i, size_t *tlgrm_userid, size_t *rdmn_issue_id)
{

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	return nxs_chat_srv_d_db_presale_get(u_ctx->db_presale_ctx, i, tlgrm_userid, rdmn_issue_id);
}

nxs_chat_srv_err_t nxs_chat_srv_u_presale_p_get_by_user_id(nxs_chat_srv_u_presale_t *u_ctx, size_t tlgrm_userid, size_t *rdmn_issue_id)
{

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	return nxs_chat_srv_d_db_presale_get_by_user_id(u_ctx->db_presale_ctx, tlgrm_userid, rdmn_issue_id);
}

nxs_chat_srv_err_t nxs_chat_srv_u_presale_p_get_by_issue_id(nxs_chat_srv_u_presale_t *u_ctx, size_t rdmn_issue_id, size_t *tlgrm_userid)
{

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	return nxs_chat_srv_d_db_presale_get_by_issue_id(u_ctx->db_presale_ctx, rdmn_issue_id, tlgrm_userid);
}

nxs_chat_srv_err_t nxs_chat_srv_u_presale_p_set(nxs_chat_srv_u_presale_t *u_ctx, size_t tlgrm_userid, size_t rdmn_issue_id)
{

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	return nxs_chat_srv_d_db_presale_set(u_ctx->db_presale_ctx, tlgrm_userid, rdmn_issue_id);
}

/* Module internal (static) functions */
