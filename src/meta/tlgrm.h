#ifndef _INCLUDE_NXS_CHAT_SRV_M_TLGRM_H
#define _INCLUDE_NXS_CHAT_SRV_M_TLGRM_H

/* See https://core.telegram.org/bots/api#available-types for details */

// clang-format off

typedef enum			nxs_chat_srv_m_tlgrm_chat_type_e	nxs_chat_srv_m_tlgrm_chat_type_t;

typedef struct			nxs_chat_srv_m_tlgrm_update_s		nxs_chat_srv_m_tlgrm_update_t;
typedef struct			nxs_chat_srv_m_tlgrm_message_s		nxs_chat_srv_m_tlgrm_message_t;
typedef struct			nxs_chat_srv_m_tlgrm_chat_s		nxs_chat_srv_m_tlgrm_chat_t;
typedef struct			nxs_chat_srv_m_tlgrm_user_s		nxs_chat_srv_m_tlgrm_user_t;

enum nxs_chat_srv_m_tlgrm_chat_type_e
{
	NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_NONE,
	NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_PRIVATE,
	NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_GROUP,
	NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_SUPERGROUP,
	NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_CHANNEL
};

struct nxs_chat_srv_m_tlgrm_user_s
{
	nxs_bool_t				_is_used;

	size_t					id;
	nxs_string_t				first_name;
	nxs_string_t				last_name;		/* Optional */
	nxs_string_t				username;		/* Optional */
	nxs_string_t				language_code;		/* Optional */
};

struct nxs_chat_srv_m_tlgrm_chat_s
{
	nxs_bool_t				_is_used;

	size_t					id;
	nxs_string_t				username;		/* Optional */
	nxs_chat_srv_m_tlgrm_chat_type_t	type;
};

struct nxs_chat_srv_m_tlgrm_message_s
{
	nxs_bool_t				_is_used;

	size_t					message_id;
	nxs_chat_srv_m_tlgrm_user_t		from;			/* Optional */
	nxs_chat_srv_m_tlgrm_chat_t		chat;
	nxs_chat_srv_m_tlgrm_message_t		*reply_to_message;	/* Optional */
	nxs_string_t				text;			/* Optional */
};

struct nxs_chat_srv_m_tlgrm_update_s
{
	nxs_bool_t				_is_used;

	size_t					update_id;
	nxs_chat_srv_m_tlgrm_message_t		message;		/* Optional */
};

#endif /* _INCLUDE_NXS_CHAT_SRV_M_TLGRM_H */
