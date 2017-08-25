#ifndef _INCLUDE_NXS_CHAT_SRV_C_TLGRM_C
#define _INCLUDE_NXS_CHAT_SRV_C_TLGRM_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	nxs_chat_srv_m_tlgrm_chat_type_t	type;
	nxs_string_t				name;
} nxs_chat_srv_c_tlgrm_types_t;

typedef struct
{
	nxs_chat_srv_m_tlgrm_parse_mode_t	parse_mode;
	nxs_string_t				name;
} nxs_chat_srv_c_tlgrm_parse_mode_t;

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_err_t nxs_chat_srv_c_tlgrm_update_pull_json_extract(nxs_chat_srv_m_tlgrm_update_t *update, nxs_buf_t *json_buf);
static nxs_chat_srv_err_t nxs_chat_srv_c_tlgrm_message_result_pull_json_extract(nxs_chat_srv_m_tlgrm_message_t *message,
                                                                                nxs_bool_t *                    status,
                                                                                nxs_buf_t *                     json_buf);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_message(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_callback_query(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_chat(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

// clang-format off

/* Module initializations */

static nxs_string_t	_s_par_update_id	= nxs_string("update_id");
static nxs_string_t	_s_par_message		= nxs_string("message");
static nxs_string_t	_s_par_message_id	= nxs_string("message_id");
static nxs_string_t	_s_par_text		= nxs_string("text");
static nxs_string_t	_s_par_chat		= nxs_string("chat");
static nxs_string_t	_s_par_id		= nxs_string("id");
static nxs_string_t	_s_par_username		= nxs_string("username");
static nxs_string_t	_s_par_type		= nxs_string("type");
static nxs_string_t	_s_par_from		= nxs_string("from");
static nxs_string_t	_s_par_first_name	= nxs_string("first_name");
static nxs_string_t	_s_par_last_name	= nxs_string("last_name");
static nxs_string_t	_s_par_language_code	= nxs_string("language_code");
static nxs_string_t	_s_par_reply_to_message	= nxs_string("reply_to_message");
static nxs_string_t	_s_par_callback_query	= nxs_string("callback_query");
static nxs_string_t	_s_par_chat_instance	= nxs_string("chat_instance");
static nxs_string_t	_s_par_data		= nxs_string("data");
static nxs_string_t	_s_par_result		= nxs_string("result");
static nxs_string_t	_s_par_ok		= nxs_string("ok");

static nxs_chat_srv_c_tlgrm_types_t chat_types[] =
{
	{NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_PRIVATE,		nxs_string("private")},
	{NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_GROUP,			nxs_string("group")},
	{NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_SUPERGROUP,		nxs_string("supergroup")},
	{NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_CHANNEL,		nxs_string("channel")},

	{NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_NONE,			nxs_string("")}
};

static nxs_chat_srv_c_tlgrm_parse_mode_t parse_mode[] =
{
	{NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN,		nxs_string("Markdown")},
	{NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_HTML,		nxs_string("HTML")},

	{NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_NONE,		nxs_string("")}
};

/* Module global functions */

// clang-format on

void nxs_chat_srv_c_tlgrm_update_init(nxs_chat_srv_m_tlgrm_update_t *update)
{

	if(update == NULL) {

		return;
	}

	update->_is_used  = NXS_NO;
	update->update_id = 0;

	nxs_chat_srv_c_tlgrm_callback_query_init(&update->callback_query);
	nxs_chat_srv_c_tlgrm_message_init(&update->message);
}

void nxs_chat_srv_c_tlgrm_update_free(nxs_chat_srv_m_tlgrm_update_t *update)
{

	if(update == NULL) {

		return;
	}

	update->_is_used  = NXS_NO;
	update->update_id = 0;

	nxs_chat_srv_c_tlgrm_callback_query_free(&update->callback_query);
	nxs_chat_srv_c_tlgrm_message_free(&update->message);
}

nxs_chat_srv_err_t nxs_chat_srv_c_tlgrm_update_pull_json(nxs_chat_srv_m_tlgrm_update_t *update, nxs_buf_t *json_buf)
{

	if(update == NULL || json_buf == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	return nxs_chat_srv_c_tlgrm_update_pull_json_extract(update, json_buf);
}

void nxs_chat_srv_c_tlgrm_callback_query_init(nxs_chat_srv_m_tlgrm_callback_query_t *callback_query)
{
	if(callback_query == NULL) {

		return;
	}

	callback_query->_is_used = NXS_NO;

	nxs_string_init2(&callback_query->id, 0, NXS_STRING_EMPTY_STR);
	nxs_string_init2(&callback_query->chat_instance, 0, NXS_STRING_EMPTY_STR);
	nxs_string_init2(&callback_query->data, 0, NXS_STRING_EMPTY_STR);

	nxs_chat_srv_c_tlgrm_user_init(&callback_query->from);
	nxs_chat_srv_c_tlgrm_message_init(&callback_query->message);
}

void nxs_chat_srv_c_tlgrm_callback_query_free(nxs_chat_srv_m_tlgrm_callback_query_t *callback_query)
{
	if(callback_query == NULL) {

		return;
	}

	callback_query->_is_used = NXS_NO;

	nxs_string_free(&callback_query->id);
	nxs_string_free(&callback_query->chat_instance);
	nxs_string_free(&callback_query->data);

	nxs_chat_srv_c_tlgrm_user_free(&callback_query->from);
	nxs_chat_srv_c_tlgrm_message_free(&callback_query->message);
}

void nxs_chat_srv_c_tlgrm_message_init(nxs_chat_srv_m_tlgrm_message_t *message)
{

	if(message == NULL) {

		return;
	}

	message->_is_used   = NXS_NO;
	message->message_id = 0;

	nxs_string_init2(&message->text, 0, NXS_STRING_EMPTY_STR);

	nxs_chat_srv_c_tlgrm_user_init(&message->from);
	nxs_chat_srv_c_tlgrm_chat_init(&message->chat);

	message->reply_to_message = NULL;
}

void nxs_chat_srv_c_tlgrm_message_free(nxs_chat_srv_m_tlgrm_message_t *message)
{

	if(message == NULL) {

		return;
	}

	message->_is_used   = NXS_NO;
	message->message_id = 0;

	nxs_string_free(&message->text);

	nxs_chat_srv_c_tlgrm_user_free(&message->from);
	nxs_chat_srv_c_tlgrm_chat_free(&message->chat);

	if(message->reply_to_message != NULL) {

		message->reply_to_message = nxs_chat_srv_c_tlgrm_message_reply_destroy(message->reply_to_message);
	}
}

nxs_chat_srv_err_t
        nxs_chat_srv_c_tlgrm_message_result_pull_json(nxs_chat_srv_m_tlgrm_message_t *message, nxs_bool_t *status, nxs_buf_t *json_buf)
{

	if(message == NULL || status == NULL || json_buf == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	return nxs_chat_srv_c_tlgrm_message_result_pull_json_extract(message, status, json_buf);
}

nxs_chat_srv_m_tlgrm_message_t *nxs_chat_srv_c_tlgrm_message_reply_alloc(void)
{
	nxs_chat_srv_m_tlgrm_message_t *p;

	p = (nxs_chat_srv_m_tlgrm_message_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_m_tlgrm_message_t));

	nxs_chat_srv_c_tlgrm_message_init(p);

	return p;
}

nxs_chat_srv_m_tlgrm_message_t *nxs_chat_srv_c_tlgrm_message_reply_destroy(nxs_chat_srv_m_tlgrm_message_t *message)
{

	if(message == NULL) {

		return NULL;
	}

	nxs_chat_srv_c_tlgrm_message_free(message);

	return nxs_free(message);
}

void nxs_chat_srv_c_tlgrm_chat_init(nxs_chat_srv_m_tlgrm_chat_t *chat)
{

	if(chat == NULL) {

		return;
	}

	chat->_is_used = NXS_NO;
	chat->id       = 0;
	chat->type     = NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_NONE;

	nxs_string_init2(&chat->username, 0, NXS_STRING_EMPTY_STR);
}

void nxs_chat_srv_c_tlgrm_chat_free(nxs_chat_srv_m_tlgrm_chat_t *chat)
{

	if(chat == NULL) {

		return;
	}

	chat->_is_used = NXS_NO;
	chat->id       = 0;
	chat->type     = NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_NONE;

	nxs_string_free(&chat->username);
}

nxs_chat_srv_m_tlgrm_chat_type_t nxs_chat_srv_c_tlgrm_chat_type_map(nxs_string_t *type_name)
{
	size_t i;

	for(i = 0; chat_types[i].type != NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_NONE; i++) {

		if(nxs_string_cmp(&chat_types[i].name, 0, type_name, 0) == NXS_STRING_CMP_EQ) {

			return chat_types[i].type;
		}
	}

	return NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_NONE;
}

nxs_string_t *nxs_chat_srv_c_tlgrm_parse_mode_map(nxs_chat_srv_m_tlgrm_parse_mode_t mode)
{
	size_t i;

	for(i = 0; parse_mode[i].parse_mode != NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_NONE; i++) {

		if(parse_mode[i].parse_mode == mode) {

			return &parse_mode[i].name;
		}
	}

	return &parse_mode[i].name;
}

void nxs_chat_srv_c_tlgrm_user_init(nxs_chat_srv_m_tlgrm_user_t *user)
{

	if(user == NULL) {

		return;
	}

	user->_is_used = NXS_NO;
	user->id       = 0;

	nxs_string_init2(&user->first_name, 0, NXS_STRING_EMPTY_STR);
	nxs_string_init2(&user->last_name, 0, NXS_STRING_EMPTY_STR);
	nxs_string_init2(&user->username, 0, NXS_STRING_EMPTY_STR);
	nxs_string_init2(&user->language_code, 0, NXS_STRING_EMPTY_STR);
}

void nxs_chat_srv_c_tlgrm_user_free(nxs_chat_srv_m_tlgrm_user_t *user)
{

	if(user == NULL) {

		return;
	}

	user->_is_used = NXS_NO;
	user->id       = 0;

	nxs_string_free(&user->first_name);
	nxs_string_free(&user->last_name);
	nxs_string_free(&user->username);
	nxs_string_free(&user->language_code);
}

void nxs_chat_srv_c_tlgrm_inl_keyboard_init(nxs_chat_srv_m_tlgrm_inl_keyboard_t *inl_keyboard)
{

	if(inl_keyboard == NULL) {

		return;
	}

	inl_keyboard->_is_used = NXS_NO;

	nxs_array_init2(&inl_keyboard->inline_keyboard, nxs_array_t);
}

void nxs_chat_srv_c_tlgrm_inl_keyboard_free(nxs_chat_srv_m_tlgrm_inl_keyboard_t *inl_keyboard)
{
	nxs_chat_srv_m_tlgrm_inl_keybutton_t *ikm;
	nxs_array_t *                         a;
	size_t                                i, j;

	if(inl_keyboard == NULL) {

		return;
	}

	inl_keyboard->_is_used = NXS_NO;

	for(i = 0; i < nxs_array_count(&inl_keyboard->inline_keyboard); i++) {

		a = nxs_array_get(&inl_keyboard->inline_keyboard, i);

		for(j = 0; j < nxs_array_count(a); j++) {

			ikm = nxs_array_get(a, j);

			nxs_chat_srv_c_tlgrm_inl_keybutton_free(ikm);
		}

		nxs_array_free(a);
	}

	nxs_array_free(&inl_keyboard->inline_keyboard);
}

nxs_chat_srv_err_t nxs_chat_srv_c_tlgrm_inl_keyboard_add_button(nxs_chat_srv_m_tlgrm_inl_keyboard_t *inl_keyboard,
                                                                size_t                               pos_y,
                                                                size_t                               pos_x,
                                                                nxs_string_t *                       text,
                                                                nxs_string_t *                       url,
                                                                nxs_string_t *                       callback_data)
{
	nxs_chat_srv_m_tlgrm_inl_keybutton_t *ikm, *k;
	nxs_array_t *                         a, *b;
	size_t                                i, c;

	if(inl_keyboard == NULL || text == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((url == NULL && callback_data == NULL) || (url != NULL && callback_data != NULL)) {

		/* see: https://core.telegram.org/bots/api#inlinekeyboardbutton
		 * "You must use exactly one of the optional fields." */

		return NXS_CHAT_SRV_E_ERR;
	}

	inl_keyboard->_is_used = NXS_YES;

	c = nxs_array_count(&inl_keyboard->inline_keyboard);
	a = nxs_array_add_i(&inl_keyboard->inline_keyboard, pos_y);

	/* initialized all allocated elements */
	for(i = c; i < nxs_array_count(&inl_keyboard->inline_keyboard); i++) {

		b = nxs_array_get(&inl_keyboard->inline_keyboard, i);

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

void nxs_chat_srv_c_tlgrm_inl_keyboard_serialize(nxs_chat_srv_m_tlgrm_inl_keyboard_t *inl_keyboard, nxs_string_t *out_str)
{
	nxs_chat_srv_m_tlgrm_inl_keybutton_t *ikm;
	nxs_array_t *                         a;
	nxs_string_t                          p;
	nxs_bool_t                            f;
	size_t                                i, j;

	if(inl_keyboard == NULL || out_str == NULL) {

		return;
	}

	nxs_string_clear(out_str);

	if(inl_keyboard->_is_used == NXS_NO) {

		return;
	}

	nxs_string_init(&p);

	for(i = 0; i < nxs_array_count(&inl_keyboard->inline_keyboard); i++) {

		a = nxs_array_get(&inl_keyboard->inline_keyboard, i);

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

					nxs_string_printf2_cat_dyn(&p, "{\"text\":\"%r\",\"url\":\"%r\"}", &ikm->text, &ikm->url);
				}
				else {

					nxs_string_printf2_cat_dyn(
					        &p, "{\"text\":\"%r\",\"callback_data\":\"%r\"}", &ikm->text, &ikm->callback_data);
				}
			}
		}

		nxs_string_printf2_cat_dyn(&p, "]");
	}

	nxs_string_printf2_cat_dyn(out_str, ",\"reply_markup\": {\"inline_keyboard\":[%r]}", &p);

	nxs_string_free(&p);
}

void nxs_chat_srv_c_tlgrm_inl_keybutton_init(nxs_chat_srv_m_tlgrm_inl_keybutton_t *inl_keybutton)
{

	if(inl_keybutton == NULL) {

		return;
	}

	inl_keybutton->_is_used = NXS_NO;

	nxs_string_init2(&inl_keybutton->text, 0, NXS_STRING_EMPTY_STR);
	nxs_string_init2(&inl_keybutton->url, 0, NXS_STRING_EMPTY_STR);
	nxs_string_init2(&inl_keybutton->callback_data, 0, NXS_STRING_EMPTY_STR);
}

void nxs_chat_srv_c_tlgrm_inl_keybutton_free(nxs_chat_srv_m_tlgrm_inl_keybutton_t *inl_keybutton)
{

	if(inl_keybutton == NULL) {

		return;
	}

	inl_keybutton->_is_used = NXS_NO;

	nxs_string_free(&inl_keybutton->text);
	nxs_string_free(&inl_keybutton->url);
	nxs_string_free(&inl_keybutton->callback_data);
}

void nxs_chat_srv_c_tlgrm_force_reply_init(nxs_chat_srv_m_tlgrm_force_reply_t *force_reply)
{

	if(force_reply == NULL) {

		return;
	}

	force_reply->_is_used    = NXS_NO;
	force_reply->force_reply = NXS_NO;
}

void nxs_chat_srv_c_tlgrm_force_reply_free(nxs_chat_srv_m_tlgrm_force_reply_t *force_reply)
{

	if(force_reply == NULL) {

		return;
	}

	force_reply->_is_used    = NXS_NO;
	force_reply->force_reply = NXS_NO;
}

void nxs_chat_srv_c_tlgrm_force_reply_set(nxs_chat_srv_m_tlgrm_force_reply_t *force_reply)
{

	if(force_reply == NULL) {

		return;
	}

	force_reply->_is_used    = NXS_YES;
	force_reply->force_reply = NXS_YES;
}

void nxs_chat_srv_c_tlgrm_force_reply_serialize(nxs_chat_srv_m_tlgrm_force_reply_t *force_reply, nxs_string_t *out_str)
{

	if(force_reply == NULL || out_str == NULL) {

		return;
	}

	nxs_string_clear(out_str);

	if(force_reply->_is_used && force_reply->force_reply == NXS_YES) {

		nxs_string_printf2_cat_dyn(out_str, ",\"reply_markup\": {\"force_reply\":true}");
	}
}

/* Module internal (static) functions */

static nxs_chat_srv_err_t nxs_chat_srv_c_tlgrm_update_pull_json_extract(nxs_chat_srv_m_tlgrm_update_t *update, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	update->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_update_id,	&update->update_id,		NULL,							NULL,	NXS_CFG_JSON_TYPE_INT,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_message,	&update->message,		&nxs_chat_srv_c_tlgrm_extract_json_message,		NULL,	NXS_CFG_JSON_TYPE_VOID,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_callback_query,	&update->callback_query,	&nxs_chat_srv_c_tlgrm_extract_json_callback_query,	NULL,	NXS_CFG_JSON_TYPE_VOID,	0,	0,	NXS_NO,		NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: tlgrm update pull rest api error: parse json_buf error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_chat_srv_err_t nxs_chat_srv_c_tlgrm_message_result_pull_json_extract(nxs_chat_srv_m_tlgrm_message_t *message,
                                                                                nxs_bool_t *                    status,
                                                                                nxs_buf_t *                     json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_result,		message,	&nxs_chat_srv_c_tlgrm_extract_json_message,	NULL,	NXS_CFG_JSON_TYPE_VOID,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_ok,		status,		NULL,						NULL,	NXS_CFG_JSON_TYPE_BOOL,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(
		        &process, "[%s]: tlgrm message result json pull error: parse json_buf error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_message(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_tlgrm_message_t *var = nxs_cfg_json_get_val(cfg_json_par_el), *m;
	nxs_cfg_json_t                  cfg_json;
	nxs_array_t                     cfg_arr;
	nxs_cfg_json_state_t            rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	m = nxs_chat_srv_c_tlgrm_message_reply_alloc();

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_message_id,		&var->message_id,	NULL,						NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_from,			&var->from,		&nxs_chat_srv_c_tlgrm_extract_json_user,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_chat,			&var->chat,		&nxs_chat_srv_c_tlgrm_extract_json_chat,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_text,			&var->text,		NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_reply_to_message,	m,			&nxs_chat_srv_c_tlgrm_extract_json_message,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_NO,		NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "json read error: 'message' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

	if(m->_is_used == NXS_YES) {

		var->reply_to_message = m;

		m = NULL;
	}

error:

	m = nxs_chat_srv_c_tlgrm_message_reply_destroy(m);

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_callback_query(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_tlgrm_callback_query_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                         cfg_json;
	nxs_array_t                            cfg_arr;
	nxs_cfg_json_state_t                   rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&var->id,		NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_from,		&var->from,		&nxs_chat_srv_c_tlgrm_extract_json_user,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_message,	&var->message,		&nxs_chat_srv_c_tlgrm_extract_json_message,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_chat_instance,	&var->chat_instance,	NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_data,		&var->data,		NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "json read error: 'callback_query' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_chat(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_tlgrm_chat_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t               cfg_json;
	nxs_array_t                  cfg_arr;
	nxs_string_t                 chat_type;
	nxs_cfg_json_state_t         rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_string_init(&chat_type);

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&var->id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_username,	&var->username,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_type,		&chat_type,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "json read error: 'message.chat' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

	var->type = nxs_chat_srv_c_tlgrm_chat_type_map(&chat_type);

error:

	nxs_string_free(&chat_type);

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_tlgrm_user_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t               cfg_json;
	nxs_array_t                  cfg_arr;
	nxs_cfg_json_state_t         rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,			&var->id,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_first_name,		&var->first_name,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_last_name,		&var->last_name,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_username,		&var->username,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_language_code,		&var->language_code,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "json read error: 'message' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}
