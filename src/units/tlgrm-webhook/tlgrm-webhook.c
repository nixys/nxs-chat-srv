#ifndef _INCLUDE_NXS_CHAT_SRV_U_TLGRM_WEBHOOK_C
#define _INCLUDE_NXS_CHAT_SRV_U_TLGRM_WEBHOOK_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/tlgrm-webhook/tlgrm-webhook-typedefs.h>
#include <units/tlgrm-webhook/tlgrm-webhook.h>
#include <units/tlgrm-webhook/tlgrm-webhook-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_u_tlgrm_webhook_s
{
	nxs_buf_t		response_buf;
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_u_tlgrm_webhook_t *nxs_chat_srv_u_tlgrm_webhook_init(void)
{
	nxs_chat_srv_u_tlgrm_webhook_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_tlgrm_webhook_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_tlgrm_webhook_t));

	nxs_buf_init2(&u_ctx->response_buf);

	return u_ctx;
}

nxs_chat_srv_u_tlgrm_webhook_t *nxs_chat_srv_u_tlgrm_webhook_free(nxs_chat_srv_u_tlgrm_webhook_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	nxs_buf_free(&u_ctx->response_buf);

	return (nxs_chat_srv_u_tlgrm_webhook_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_webhook_get(nxs_chat_srv_u_tlgrm_webhook_t *u_ctx, nxs_string_t *url)
{
	nxs_chat_srv_m_tlgrm_webhookinfo_t webhookinfo;
	nxs_chat_srv_err_t                 rc;
	nxs_string_t                       message;
	nxs_bool_t                         response_status;

	if(u_ctx == NULL || url == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	nxs_string_clear(url);

	nxs_string_init_empty(&message);

	nxs_chat_srv_c_tlgrm_webhookinfo_init(&webhookinfo);

	if(nxs_chat_srv_d_tlgrm_request(NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_GET_WEBHOOKINFO, &message, NULL, &u_ctx->response_buf) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process, "[%s]: can't get webhook info from tlgrm", nxs_proc_get_name(&process));

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_c_tlgrm_webhookinfo_result_pull_json(&webhookinfo, &response_status, &u_ctx->response_buf) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_string_clone(url, &webhookinfo.url);

error:

	nxs_string_free(&message);

	nxs_chat_srv_c_tlgrm_webhookinfo_free(&webhookinfo);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_webhook_set(nxs_chat_srv_u_tlgrm_webhook_t *u_ctx, nxs_bool_t *status, nxs_string_t *description)
{
	nxs_chat_srv_m_tlgrm_webhookset_t webhookset;
	nxs_chat_srv_err_t                rc;
	nxs_string_t                      message;

	if(u_ctx == NULL || status == NULL || description == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init_empty(&message);

	nxs_string_printf(
	        &message, "{\"url\":\"%r/tlgrm?token=%r\"}", &nxs_chat_srv_cfg.tlgrm.webhook_host, &nxs_chat_srv_cfg.tlgrm.auth_token);

	nxs_chat_srv_c_tlgrm_webhookset_init(&webhookset);

	switch(nxs_chat_srv_d_tlgrm_request(NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SET_WEBHOOK, &message, NULL, &u_ctx->response_buf)) {

		case NXS_CHAT_SRV_E_OK:
		case NXS_CHAT_SRV_E_WARN:

			break;

		default:

			nxs_log_write_error(&process, "[%s]: can't set webhook to tlgrm", nxs_proc_get_name(&process));

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_c_tlgrm_webhookset_result_pull_json(&webhookset, &u_ctx->response_buf) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(webhookset.ok == NXS_YES) {

		nxs_log_write_warn(&process,
		                   "[%s]: set webhook to tlgrm: success (result: %s, error code: %zu, description: \"%s\")",
		                   nxs_proc_get_name(&process),
		                   webhookset.result == NXS_YES ? "true" : "false",
		                   webhookset.error_code,
		                   nxs_string_str(&webhookset.description));
	}
	else {

		nxs_log_write_warn(&process,
		                   "[%s]: set webhook to tlgrm: fail (result: %s, error code: %zu, description: \"%s\")",
		                   nxs_proc_get_name(&process),
		                   webhookset.result == NXS_YES ? "true" : "false",
		                   webhookset.error_code,
		                   nxs_string_str(&webhookset.description));
	}

	*status = webhookset.ok;

	nxs_string_clone(description, &webhookset.description);

error:

	nxs_string_free(&message);

	nxs_chat_srv_c_tlgrm_webhookset_free(&webhookset);

	return rc;
}

/* Module internal (static) functions */
