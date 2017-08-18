#ifndef _INCLUDE_NXS_CHAT_SRV_M_TLGRM_H
#define _INCLUDE_NXS_CHAT_SRV_M_TLGRM_H

/* See https://core.telegram.org/bots/api#available-types for details */

// clang-format off

typedef struct			nxs_chat_srv_m_tlgrm_update_s		nxs_chat_srv_m_tlgrm_update_t;
typedef struct			nxs_chat_srv_m_tlgrm_message_s		nxs_chat_srv_m_tlgrm_message_t;
typedef struct			nxs_chat_srv_m_tlgrm_chat_s		nxs_chat_srv_m_tlgrm_chat_t;

struct nxs_chat_srv_m_tlgrm_chat_s
{
	nxs_bool_t				_is_used;

	size_t					id;
	nxs_string_t				username;
};

struct nxs_chat_srv_m_tlgrm_message_s
{
	nxs_bool_t				_is_used;

	size_t					message_id;
	nxs_string_t				text;
	nxs_chat_srv_m_tlgrm_chat_t		chat;
};

struct nxs_chat_srv_m_tlgrm_update_s
{
	nxs_bool_t				_is_used;

	size_t					update_id;
	nxs_chat_srv_m_tlgrm_message_t		message;
};

#endif /* _INCLUDE_NXS_CHAT_SRV_M_TLGRM_H */
