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

// clang-format off

/* Module initializations */

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

	nxs_chat_srv_c_tlgrm_message_init(&update->message);
}

void nxs_chat_srv_c_tlgrm_update_free(nxs_chat_srv_m_tlgrm_update_t *update)
{

	if(update == NULL) {

		return;
	}

	update->_is_used  = NXS_NO;
	update->update_id = 0;

	nxs_chat_srv_c_tlgrm_message_free(&update->message);
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

/* Module internal (static) functions */
