#ifndef _INCLUDE_NXS_CHAT_SRV_M_TLGRM_H
#define _INCLUDE_NXS_CHAT_SRV_M_TLGRM_H

/* See https://core.telegram.org/bots/api#available-types for details */

// clang-format off

typedef enum			nxs_chat_srv_m_tlgrm_chat_type_e		nxs_chat_srv_m_tlgrm_chat_type_t;
typedef enum			nxs_chat_srv_m_tlgrm_reply_markup_type_e	nxs_chat_srv_m_tlgrm_reply_markup_type_t;
typedef enum			nxs_chat_srv_m_tlgrm_parse_mode_e		nxs_chat_srv_m_tlgrm_parse_mode_t;

typedef struct			nxs_chat_srv_m_tlgrm_update_s			nxs_chat_srv_m_tlgrm_update_t;
typedef struct			nxs_chat_srv_m_tlgrm_message_s			nxs_chat_srv_m_tlgrm_message_t;
typedef struct			nxs_chat_srv_m_tlgrm_chat_s			nxs_chat_srv_m_tlgrm_chat_t;
typedef struct			nxs_chat_srv_m_tlgrm_user_s			nxs_chat_srv_m_tlgrm_user_t;
typedef struct			nxs_chat_srv_m_tlgrm_inl_keyboard_s		nxs_chat_srv_m_tlgrm_inl_keyboard_t;
typedef struct			nxs_chat_srv_m_tlgrm_inl_keybutton_s		nxs_chat_srv_m_tlgrm_inl_keybutton_t;
typedef struct			nxs_chat_srv_m_tlgrm_force_reply_s		nxs_chat_srv_m_tlgrm_force_reply_t;

enum nxs_chat_srv_m_tlgrm_chat_type_e
{
	NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_NONE,
	NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_PRIVATE,
	NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_GROUP,
	NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_SUPERGROUP,
	NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_CHANNEL
};

enum nxs_chat_srv_m_tlgrm_reply_markup_type_e
{
	NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_NONE,
	NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_IKM,			/* InlineKeyboardMarkup */
	NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_RKM,			/* ReplyKeyboardMarkup */
	NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_RKR,			/* ReplyKeyboardRemove */
	NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_FR			/* ForceReply */
};

enum nxs_chat_srv_m_tlgrm_parse_mode_e
{
	NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_NONE,
	NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN,
	NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_HTML
};

struct nxs_chat_srv_m_tlgrm_force_reply_s
{
	nxs_bool_t				_is_used;

	nxs_bool_t				force_reply;
};

struct nxs_chat_srv_m_tlgrm_inl_keybutton_s
{
	nxs_bool_t				_is_used;

	nxs_string_t				text;
	nxs_string_t				url;			/* Optional */
	nxs_string_t				callback_data;		/* Optional */
};

struct nxs_chat_srv_m_tlgrm_inl_keyboard_s
{
	nxs_bool_t				_is_used;

	nxs_array_t				inline_keyboard;	/* type: array of arrays nxs_chat_srv_m_tlgrm_inl_keybutton_t */
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
