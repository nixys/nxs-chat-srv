#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/rest-api/tlgrm/tlgrm.h>
#include <proc/rest-api/tlgrm/ctx/tlgrm-ctx.h>
#include <proc/rest-api/tlgrm/tlgrm-subproc.h>

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

static nxs_string_t _s_ra_header_ct_key			= nxs_string("Content-Type");
static nxs_string_t _s_ra_header_ct_val			= nxs_string("application/json; charset=utf-8");

static nxs_string_t _s_ra_arg_key_token			= nxs_string("token");

/* Module global functions */

// clang-format on

void nxs_chat_srv_p_rest_api_tlgrm_handler_post(nxs_rest_api_ctx_t *rest_api_ctx, nxs_rest_api_request_t *req, void *custom_ctx)
{
	nxs_chat_srv_u_ra_queue_t *ra_queue_ctx = custom_ctx;
	nxs_string_t *             auth_token;
	nxs_buf_t *                bdy_in;

	nxs_rest_api_header_add(req, &_s_ra_header_ct_key, &_s_ra_header_ct_val);

	/* Request processing */

	bdy_in = nxs_rest_api_get_req_body(req);

	if((auth_token = nxs_rest_api_get_req_args_find(req, &_s_ra_arg_key_token)) == NULL) {

		nxs_rest_api_page_std(req, NXS_REST_API_FORMAT_ERR_JSON, NXS_HTTP_CODE_403_FORBIDDEN, (u_char *)"");

		return;
	}

	if(nxs_string_cmp(&nxs_chat_srv_cfg.tlgrm.auth_token, 0, auth_token, 0) == NXS_NO) {

		nxs_rest_api_page_std(req, NXS_REST_API_FORMAT_ERR_JSON, NXS_HTTP_CODE_403_FORBIDDEN, (u_char *)"");

		return;
	}

	nxs_log_write_debug(&process,
	                    "[%s]: received message from telegram (auth token: \"%r\" (correct), post body: \"%R\")",
	                    nxs_proc_get_name(&process),
	                    auth_token,
	                    bdy_in,
	                    0);

	if(nxs_chat_srv_u_ra_queue_add(ra_queue_ctx, NXS_CHAT_SRV_M_RA_QUEUE_TYPE_TLGRM_UPDATE, (nxs_string_t *)bdy_in) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_rest_api_page_std(req, NXS_REST_API_FORMAT_ERR_JSON, NXS_HTTP_CODE_500_INTERNAL_SERVER_ERROR, (u_char *)"");

		return;
	}

	nxs_rest_api_page_std(req, NXS_REST_API_FORMAT_ERR_JSON, NXS_HTTP_CODE_200_OK, (u_char *)"");
}

/* Module internal (static) functions */
