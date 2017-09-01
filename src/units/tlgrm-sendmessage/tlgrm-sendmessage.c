#ifndef _INCLUDE_NXS_CHAT_SRV_U_TLGRM_SENDMESSAGE_C
#define _INCLUDE_NXS_CHAT_SRV_U_TLGRM_SENDMESSAGE_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/tlgrm-sendmessage/tlgrm-sendmessage-typedefs.h>
#include <units/tlgrm-sendmessage/tlgrm-sendmessage.h>
#include <units/tlgrm-sendmessage/tlgrm-sendmessage-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	nxs_chat_srv_m_tlgrm_reply_markup_type_t		type;
	nxs_chat_srv_m_tlgrm_inl_keyboard_t			ikms;
	nxs_chat_srv_m_tlgrm_force_reply_t			fr;
} nxs_chat_srv_u_tlgrm_sendmessage_reply_markup_t;

/* see: https://core.telegram.org/bots/api#sendmessage */
struct nxs_chat_srv_u_tlgrm_sendmessage_s
{
	size_t							chat_id;
	nxs_string_t						text;
	nxs_chat_srv_m_tlgrm_parse_mode_t			parse_mode;
	nxs_chat_srv_u_tlgrm_sendmessage_reply_markup_t		reply_markup;

	nxs_buf_t						response_buf;
};

/* Module internal (static) functions prototypes */

// clang-format on

static void nxs_chat_srv_u_tlgrm_sendmessage_rmarkaup_serialize(nxs_chat_srv_u_tlgrm_sendmessage_reply_markup_t *rmarkaup,
                                                                nxs_string_t *                                   out_str);
static void nxs_chat_srv_u_tlgrm_sendmessage_rmarkaup_init(nxs_chat_srv_u_tlgrm_sendmessage_reply_markup_t *rmarkaup);
static void nxs_chat_srv_u_tlgrm_sendmessage_rmarkaup_free(nxs_chat_srv_u_tlgrm_sendmessage_reply_markup_t *rmarkaup);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_u_tlgrm_sendmessage_t *nxs_chat_srv_u_tlgrm_sendmessage_init(void)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_tlgrm_sendmessage_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_tlgrm_sendmessage_t));

	u_ctx->chat_id    = 0;
	u_ctx->parse_mode = NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_NONE;

	nxs_string_init_empty(&u_ctx->text);

	nxs_chat_srv_u_tlgrm_sendmessage_rmarkaup_init(&u_ctx->reply_markup);

	nxs_buf_init(&u_ctx->response_buf, 1);

	return u_ctx;
}

nxs_chat_srv_u_tlgrm_sendmessage_t *nxs_chat_srv_u_tlgrm_sendmessage_free(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	u_ctx->chat_id    = 0;
	u_ctx->parse_mode = NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_NONE;

	nxs_string_free(&u_ctx->text);

	nxs_chat_srv_u_tlgrm_sendmessage_rmarkaup_free(&u_ctx->reply_markup);

	nxs_buf_free(&u_ctx->response_buf);

	return (nxs_chat_srv_u_tlgrm_sendmessage_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_sendmessage_push(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx)
{
	nxs_chat_srv_err_t rc;
	nxs_string_t       message, reply_markup_str;
	nxs_http_code_t    http_code;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&message);
	nxs_string_init(&reply_markup_str);

	nxs_chat_srv_u_tlgrm_sendmessage_rmarkaup_serialize(&u_ctx->reply_markup, &reply_markup_str);

	nxs_string_printf(&message,
	                  "{\"chat_id\":%zu,\"parse_mode\":\"%r\",\"text\":\"%r\"%r}",
	                  u_ctx->chat_id,
	                  nxs_chat_srv_c_tlgrm_parse_mode_map(u_ctx->parse_mode),
	                  &u_ctx->text,
	                  &reply_markup_str);

	if(nxs_chat_srv_d_tlgrm_request(NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_MESSAGE, &message, &u_ctx->response_buf, &http_code) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(http_code != NXS_HTTP_CODE_200_OK) {

		nxs_log_write_warn(
		        &process, "[%s]: wrong telegram send message http code (http code: %d)", nxs_proc_get_name(&process), http_code);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_string_free(&message);
	nxs_string_free(&reply_markup_str);

	return rc;
}

nxs_buf_t *nxs_chat_srv_u_tlgrm_sendmessage_get_response_buf(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	return &u_ctx->response_buf;
}

void nxs_chat_srv_u_tlgrm_sendmessage_add(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx,
                                          size_t                              chat_id,
                                          nxs_string_t *                      text,
                                          nxs_chat_srv_m_tlgrm_parse_mode_t   parse_mode)
{

	if(u_ctx == NULL) {

		return;
	}

	u_ctx->chat_id    = chat_id;
	u_ctx->parse_mode = parse_mode;

	nxs_string_clone(&u_ctx->text, text);
}

nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_add(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx,
                                                                         size_t                              pos_y,
                                                                         size_t                              pos_x,
                                                                         nxs_string_t *                      text,
                                                                         nxs_string_t *                      url,
                                                                         nxs_string_t *                      callback_data)
{
	if(u_ctx == NULL || text == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(u_ctx->reply_markup.type != NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_NONE &&
	   u_ctx->reply_markup.type != NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_IKM) {

		return NXS_CHAT_SRV_E_TYPE;
	}

	if(nxs_chat_srv_c_tlgrm_inl_keyboard_add_button(&u_ctx->reply_markup.ikms, pos_y, pos_x, text, url, callback_data) !=
	   NXS_CHAT_SRV_E_OK) {

		return NXS_CHAT_SRV_E_ERR;
	}

	u_ctx->reply_markup.type = NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_IKM;

	return NXS_CHAT_SRV_E_OK;
}

nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_sendmessage_force_reply_set(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx)
{
	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(u_ctx->reply_markup.type != NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_NONE &&
	   u_ctx->reply_markup.type != NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_FR) {

		return NXS_CHAT_SRV_E_TYPE;
	}

	u_ctx->reply_markup.type = NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_FR;

	nxs_chat_srv_c_tlgrm_force_reply_set(&u_ctx->reply_markup.fr);

	return NXS_CHAT_SRV_E_OK;
}

/* Module internal (static) functions */

static void nxs_chat_srv_u_tlgrm_sendmessage_rmarkaup_serialize(nxs_chat_srv_u_tlgrm_sendmessage_reply_markup_t *rmarkaup,
                                                                nxs_string_t *                                   out_str)
{

	if(rmarkaup == NULL || out_str == NULL) {

		return;
	}

	nxs_string_clear(out_str);

	switch(rmarkaup->type) {

		case NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_IKM:

			nxs_chat_srv_c_tlgrm_inl_keyboard_serialize(&rmarkaup->ikms, out_str);

			break;

		case NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_FR:

			nxs_chat_srv_c_tlgrm_force_reply_serialize(&rmarkaup->fr, out_str);

			break;

		default:

			break;
	}
}

static void nxs_chat_srv_u_tlgrm_sendmessage_rmarkaup_init(nxs_chat_srv_u_tlgrm_sendmessage_reply_markup_t *rmarkaup)
{

	if(rmarkaup == NULL) {

		return;
	}

	rmarkaup->type = NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_NONE;

	nxs_chat_srv_c_tlgrm_inl_keyboard_init(&rmarkaup->ikms);
	nxs_chat_srv_c_tlgrm_force_reply_init(&rmarkaup->fr);
}

static void nxs_chat_srv_u_tlgrm_sendmessage_rmarkaup_free(nxs_chat_srv_u_tlgrm_sendmessage_reply_markup_t *rmarkaup)
{

	if(rmarkaup == NULL) {

		return;
	}

	rmarkaup->type = NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_NONE;

	nxs_chat_srv_c_tlgrm_inl_keyboard_free(&rmarkaup->ikms);
	nxs_chat_srv_c_tlgrm_force_reply_free(&rmarkaup->fr);
}
