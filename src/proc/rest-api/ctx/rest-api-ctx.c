#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <proc/rest-api/ctx/rest-api-ctx.h>

/* Definitions */


/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_rest_api_ctx_init(nxs_chat_srv_p_rest_api_ctx_t *p_ctx)
{
	nxs_rest_api_err_t ra_rc;

	if((ra_rc = nxs_rest_api_init(&process,
	                              &p_ctx->ra_conn,
	                              &nxs_chat_srv_cfg.bind.iface,
	                              nxs_chat_srv_cfg.bind.port,
	                              NXS_REST_API_FORMAT_ERR_JSON,
	                              &nxs_chat_srv_cfg.bind.ssl.crt,
	                              &nxs_chat_srv_cfg.bind.ssl.key)) != NXS_REST_API_E_OK) {

		nxs_log_write_error(&process, "[%s]: rest api init error (error code: %d)", nxs_proc_get_name(&process), ra_rc);

		return NXS_CHAT_SRV_E_ERR;
	}

	nxs_rest_api_set_max_header_size(&p_ctx->ra_conn, NXS_CHAT_SRV_REST_API_MAX_HEADER_SIZE);

	return NXS_CHAT_SRV_E_OK;
}

nxs_chat_srv_err_t nxs_chat_srv_p_rest_api_ctx_free(nxs_chat_srv_p_rest_api_ctx_t *p_ctx)
{

	nxs_rest_api_free(&p_ctx->ra_conn);

	return NXS_CHAT_SRV_E_OK;
}

nxs_rest_api_ctx_t *nxs_chat_srv_p_rest_api_ctx_get_ra_conn(nxs_chat_srv_p_rest_api_ctx_t *p_ctx)
{

	if(p_ctx == NULL) {

		return NULL;
	}

	return &p_ctx->ra_conn;
}

/* Module internal (static) functions */
