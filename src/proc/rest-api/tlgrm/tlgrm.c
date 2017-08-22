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
	nxs_string_t *auth_token, q_com_out, base64_encoded;
	nxs_buf_t *   bdy_in;

	nxs_string_init(&base64_encoded);
	nxs_string_init(&q_com_out);

	nxs_rest_api_header_add(req, &_s_ra_header_ct_key, &_s_ra_header_ct_val);

	/* Request processing */

	bdy_in = nxs_rest_api_get_req_body(req);

	if((auth_token = nxs_rest_api_get_req_args_find(req, &_s_ra_arg_key_token)) == NULL) {

		nxs_rest_api_page_std(req, NXS_REST_API_FORMAT_ERR_JSON, NXS_HTTP_CODE_403_FORBIDDEN, (u_char *)"");

		goto error;
	}

	if(nxs_string_cmp(&nxs_chat_srv_cfg.tlgrm.auth_token, 0, auth_token, 0) == NXS_STRING_CMP_NE) {

		nxs_rest_api_page_std(req, NXS_REST_API_FORMAT_ERR_JSON, NXS_HTTP_CODE_403_FORBIDDEN, (u_char *)"");

		goto error;
	}

	nxs_log_write_debug(&process,
	                    "[%s]: received message from telegram (auth token: \"%s\" (correct), post body: \"%s\")",
	                    nxs_proc_get_name(&process),
	                    nxs_string_str(auth_token),
	                    nxs_buf_get_subbuf(bdy_in, 0));

	nxs_base64_encode_string(&base64_encoded, (nxs_string_t *)bdy_in);

	nxs_chat_srv_c_queue_com_serialize(&q_com_out, NXS_CHAT_SRV_M_QUEUE_COM_TYPE_TLGRM_UPDATE, &base64_encoded);

	if(nxs_chat_srv_c_unix_sock_send(&q_com_out) != NXS_CHAT_SRV_E_OK) {

		nxs_rest_api_page_std(req, NXS_REST_API_FORMAT_ERR_JSON, NXS_HTTP_CODE_500_INTERNAL_SERVER_ERROR, (u_char *)"");

		goto error;
	}

	nxs_rest_api_page_std(req, NXS_REST_API_FORMAT_ERR_JSON, NXS_HTTP_CODE_200_OK, (u_char *)"");

error:

	nxs_string_free(&base64_encoded);
	nxs_string_free(&q_com_out);
}

/* Module internal (static) functions */
