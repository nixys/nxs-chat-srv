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

/* Module internal (static) functions */
