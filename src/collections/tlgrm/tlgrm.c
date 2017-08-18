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



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



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

	nxs_chat_srv_c_tlgrm_chat_init(&message->chat);
}

void nxs_chat_srv_c_tlgrm_message_free(nxs_chat_srv_m_tlgrm_message_t *message)
{

	if(message == NULL) {

		return;
	}

	message->_is_used   = NXS_NO;
	message->message_id = 0;

	nxs_string_free(&message->text);

	nxs_chat_srv_c_tlgrm_chat_free(&message->chat);
}

void nxs_chat_srv_c_tlgrm_chat_init(nxs_chat_srv_m_tlgrm_chat_t *chat)
{

	if(chat == NULL) {

		return;
	}

	chat->_is_used = NXS_NO;
	chat->id       = 0;

	nxs_string_init2(&chat->username, 0, NXS_STRING_EMPTY_STR);
}

void nxs_chat_srv_c_tlgrm_chat_free(nxs_chat_srv_m_tlgrm_chat_t *chat)
{

	if(chat == NULL) {

		return;
	}

	chat->_is_used = NXS_NO;
	chat->id       = 0;

	nxs_string_free(&chat->username);
}

/* Module internal (static) functions */
