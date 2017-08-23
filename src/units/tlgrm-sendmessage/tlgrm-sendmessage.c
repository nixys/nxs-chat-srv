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

typedef struct
{
	size_t							chat_id;
	nxs_string_t						text;
	nxs_chat_srv_m_tlgrm_parse_mode_t			parse_mode;
	nxs_chat_srv_u_tlgrm_sendmessage_reply_markup_t		reply_markup;
} nxs_chat_srv_u_tlgrm_sendmessage_el_t;

struct nxs_chat_srv_u_tlgrm_sendmessage_s
{
	nxs_array_t						messages;	/* type: nxs_chat_srv_u_tlgrm_sendmessage_el_t */
};

/* Module internal (static) functions prototypes */

// clang-format on

static void nxs_chat_srv_u_tlgrm_sendmessage_rmarkaup_serialize(nxs_chat_srv_u_tlgrm_sendmessage_reply_markup_t *rmarkaup,
                                                                nxs_string_t *                                   out_str);
static void nxs_chat_srv_u_tlgrm_sendmessage_rmarkaup_free(nxs_chat_srv_u_tlgrm_sendmessage_reply_markup_t *rmarkaup);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_u_tlgrm_sendmessage_t *nxs_chat_srv_u_tlgrm_sendmessage_init(void)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_tlgrm_sendmessage_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_tlgrm_sendmessage_t));

	nxs_array_init2(&u_ctx->messages, nxs_chat_srv_u_tlgrm_sendmessage_el_t);

	return u_ctx;
}

nxs_chat_srv_u_tlgrm_sendmessage_t *nxs_chat_srv_u_tlgrm_sendmessage_free(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx)
{
	nxs_chat_srv_u_tlgrm_sendmessage_el_t *m;
	size_t                                 i;

	if(u_ctx == NULL) {

		return NULL;
	}

	for(i = 0; i < nxs_array_count(&u_ctx->messages); i++) {

		m = nxs_array_get(&u_ctx->messages, i);

		m->chat_id = 0;

		nxs_string_free(&m->text);

		nxs_chat_srv_u_tlgrm_sendmessage_rmarkaup_free(&m->reply_markup);
	}

	nxs_array_free(&u_ctx->messages);

	return (nxs_chat_srv_u_tlgrm_sendmessage_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_sendmessage_push(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx)
{
	nxs_chat_srv_u_tlgrm_sendmessage_el_t *m;
	size_t                                 i;
	nxs_chat_srv_err_t                     rc;
	nxs_string_t                           message, reply_markup_str;
	nxs_http_code_t                        http_code;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&message);
	nxs_string_init(&reply_markup_str);

	for(i = 0; i < nxs_array_count(&u_ctx->messages); i++) {

		m = nxs_array_get(&u_ctx->messages, i);

		nxs_chat_srv_u_tlgrm_sendmessage_rmarkaup_serialize(&m->reply_markup, &reply_markup_str);

		nxs_string_printf_dyn(&message,
		                      "{\"chat_id\":%zu,\"parse_mode\":\"%r\",\"text\":\"%r\"%r}",
		                      m->chat_id,
		                      nxs_chat_srv_c_tlgrm_parse_mode_map(m->parse_mode),
		                      &m->text,
		                      &reply_markup_str);

		if(nxs_chat_srv_d_tlgrm_request(NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_MESSAGE, &message, NULL, &http_code) !=
		   NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		if(http_code != NXS_HTTP_CODE_200_OK) {

			nxs_log_write_warn(&process,
			                   "[%s]: wrong telegram send message http code (http code: %d)",
			                   nxs_proc_get_name(&process),
			                   http_code);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}

error:

	nxs_string_free(&message);
	nxs_string_free(&reply_markup_str);

	return rc;
}

size_t nxs_chat_srv_u_tlgrm_sendmessage_add(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx,
                                            size_t                              chat_id,
                                            nxs_string_t *                      text,
                                            nxs_chat_srv_m_tlgrm_parse_mode_t   parse_mode)
{
	nxs_chat_srv_u_tlgrm_sendmessage_el_t *m;
	size_t                                 i;

	if(u_ctx == NULL) {

		return 0;
	}

	m = nxs_array_add(&u_ctx->messages);

	m->chat_id    = chat_id;
	m->parse_mode = parse_mode;

	nxs_string_init3(&m->text, text);

	m->reply_markup.type = NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_NONE;

	nxs_chat_srv_c_tlgrm_inl_keyboard_init(&m->reply_markup.ikms);
	nxs_chat_srv_c_tlgrm_force_reply_init(&m->reply_markup.fr);

	for(i = 0; i < nxs_array_count(&u_ctx->messages); i++) {

		if(nxs_array_get(&u_ctx->messages, i) == m) {

			return i;
		}
	}

	return 0;
}

nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_add(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx,
                                                                         size_t                              message_id,
                                                                         size_t                              pos_y,
                                                                         size_t                              pos_x,
                                                                         nxs_string_t *                      text,
                                                                         nxs_string_t *                      url,
                                                                         nxs_string_t *                      callback_data)
{
	nxs_chat_srv_u_tlgrm_sendmessage_el_t *m;
	nxs_chat_srv_m_tlgrm_inl_keybutton_t * ikm, *k;
	nxs_array_t *                          a, *b;
	size_t                                 i, c;

	if(u_ctx == NULL || text == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((url == NULL && callback_data == NULL) || (url != NULL && callback_data != NULL)) {

		/* see: https://core.telegram.org/bots/api#inlinekeyboardbutton
		 * "You must use exactly one of the optional fields." */

		return NXS_CHAT_SRV_E_ERR;
	}

	if((m = nxs_array_get(&u_ctx->messages, message_id)) == NULL) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(m->reply_markup.type != NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_NONE &&
	   m->reply_markup.type != NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_IKM) {

		return NXS_CHAT_SRV_E_TYPE;
	}

	m->reply_markup.type = NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_IKM;

	c = nxs_array_count(&m->reply_markup.ikms.inline_keyboard);
	a = nxs_array_add_i(&m->reply_markup.ikms.inline_keyboard, pos_y);

	/* initialized all allocated elements */
	for(i = c; i < nxs_array_count(&m->reply_markup.ikms.inline_keyboard); i++) {

		b = nxs_array_get(&m->reply_markup.ikms.inline_keyboard, i);

		nxs_array_init2(b, nxs_chat_srv_m_tlgrm_inl_keybutton_t);
	}

	c   = nxs_array_count(a);
	ikm = nxs_array_add_i(a, pos_x);

	/* initialized all allocated elements */
	for(i = c; i < nxs_array_count(a); i++) {

		k = nxs_array_get(a, i);

		nxs_chat_srv_c_tlgrm_inl_keybutton_init(k);
	}

	/* fill element (pos_y, pos_x) */

	ikm->_is_used = NXS_YES;

	nxs_string_clone(&ikm->text, text);

	if(url != NULL) {

		nxs_string_clone(&ikm->url, url);
	}

	if(callback_data != NULL) {

		nxs_string_clone(&ikm->callback_data, callback_data);
	}

	return NXS_CHAT_SRV_E_OK;
}

nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_sendmessage_force_reply_set(nxs_chat_srv_u_tlgrm_sendmessage_t *u_ctx, size_t message_id)
{
	nxs_chat_srv_u_tlgrm_sendmessage_el_t *m;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((m = nxs_array_get(&u_ctx->messages, message_id)) == NULL) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(m->reply_markup.type != NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_NONE &&
	   m->reply_markup.type != NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_FR) {

		return NXS_CHAT_SRV_E_TYPE;
	}

	m->reply_markup.type = NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_FR;

	m->reply_markup.fr._is_used    = NXS_YES;
	m->reply_markup.fr.force_reply = NXS_YES;

	return NXS_CHAT_SRV_E_OK;
}

/* Module internal (static) functions */

static void nxs_chat_srv_u_tlgrm_sendmessage_rmarkaup_serialize(nxs_chat_srv_u_tlgrm_sendmessage_reply_markup_t *rmarkaup,
                                                                nxs_string_t *                                   out_str)
{
	nxs_chat_srv_m_tlgrm_inl_keybutton_t *ikm;
	nxs_array_t *                         a;
	nxs_string_t                          p;
	size_t                                i, j;
	nxs_bool_t                            f;

	if(rmarkaup == NULL || out_str == NULL) {

		return;
	}

	nxs_string_clear(out_str);
	nxs_string_init(&p);

	switch(rmarkaup->type) {

		case NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_IKM:

			for(i = 0; i < nxs_array_count(&rmarkaup->ikms.inline_keyboard); i++) {

				a = nxs_array_get(&rmarkaup->ikms.inline_keyboard, i);

				if(i > 0) {

					nxs_string_char_add_char_dyn(&p, (u_char)',');
				}

				nxs_string_printf2_cat_dyn(&p, "[");

				for(f = NXS_NO, j = 0; j < nxs_array_count(a); j++) {

					ikm = nxs_array_get(a, j);

					if(ikm->_is_used == NXS_YES) {

						if(f == NXS_YES) {

							nxs_string_char_add_char_dyn(&p, (u_char)',');
						}

						f = NXS_YES;

						if(nxs_string_len(&ikm->url) > 0) {

							nxs_string_printf2_cat_dyn(
							        &p, "{\"text\":\"%r\",\"url\":\"%r\"}", &ikm->text, &ikm->url);
						}
						else {

							nxs_string_printf2_cat_dyn(&p,
							                           "{\"text\":\"%r\",\"callback_data\":\"%r\"}",
							                           &ikm->text,
							                           &ikm->callback_data);
						}
					}
				}

				nxs_string_printf2_cat_dyn(&p, "]");
			}

			nxs_string_printf2_cat_dyn(out_str, ",\"reply_markup\": {\"inline_keyboard\":[%r]}", &p);

			break;

		case NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_FR:

			if(rmarkaup->fr._is_used && rmarkaup->fr.force_reply == NXS_YES) {

				nxs_string_printf2_cat_dyn(out_str, ",\"reply_markup\": {\"force_reply\":true}");
			}

			break;

		default:

			break;
	}

	nxs_string_free(&p);
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
