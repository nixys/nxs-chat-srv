#ifndef _INCLUDE_NXS_CHAT_SRV_M_TLGRM_H
#define _INCLUDE_NXS_CHAT_SRV_M_TLGRM_H

/* See https://core.telegram.org/bots/api#available-types for details */

// clang-format off

typedef enum			nxs_chat_srv_m_tlgrm_update_type_e		nxs_chat_srv_m_tlgrm_update_type_t;
typedef enum			nxs_chat_srv_m_tlgrm_chat_type_e		nxs_chat_srv_m_tlgrm_chat_type_t;
typedef enum			nxs_chat_srv_m_tlgrm_reply_markup_type_e	nxs_chat_srv_m_tlgrm_reply_markup_type_t;
typedef enum			nxs_chat_srv_m_tlgrm_parse_mode_e		nxs_chat_srv_m_tlgrm_parse_mode_t;
typedef enum			nxs_chat_srv_m_tlgrm_bttn_callback_type_e	nxs_chat_srv_m_tlgrm_bttn_callback_type_t;

typedef struct			nxs_chat_srv_m_tlgrm_file_s			nxs_chat_srv_m_tlgrm_file_t;
typedef struct			nxs_chat_srv_m_tlgrm_update_s			nxs_chat_srv_m_tlgrm_update_t;
typedef struct			nxs_chat_srv_m_tlgrm_callback_query_s		nxs_chat_srv_m_tlgrm_callback_query_t;
typedef struct			nxs_chat_srv_m_tlgrm_message_entity_s		nxs_chat_srv_m_tlgrm_message_entity_t;
typedef struct			nxs_chat_srv_m_tlgrm_photo_size_s		nxs_chat_srv_m_tlgrm_photo_size_t;
typedef struct			nxs_chat_srv_m_tlgrm_document_s			nxs_chat_srv_m_tlgrm_document_t;
typedef struct			nxs_chat_srv_m_tlgrm_sticker_s			nxs_chat_srv_m_tlgrm_sticker_t;
typedef struct			nxs_chat_srv_m_tlgrm_voice_s			nxs_chat_srv_m_tlgrm_voice_t;
typedef struct			nxs_chat_srv_m_tlgrm_video_s			nxs_chat_srv_m_tlgrm_video_t;
typedef struct			nxs_chat_srv_m_tlgrm_message_s			nxs_chat_srv_m_tlgrm_message_t;
typedef struct			nxs_chat_srv_m_tlgrm_chat_s			nxs_chat_srv_m_tlgrm_chat_t;
typedef struct			nxs_chat_srv_m_tlgrm_user_s			nxs_chat_srv_m_tlgrm_user_t;
typedef struct			nxs_chat_srv_m_tlgrm_inl_keyboard_s		nxs_chat_srv_m_tlgrm_inl_keyboard_t;
typedef struct			nxs_chat_srv_m_tlgrm_inl_keybutton_s		nxs_chat_srv_m_tlgrm_inl_keybutton_t;
typedef struct			nxs_chat_srv_m_tlgrm_force_reply_s		nxs_chat_srv_m_tlgrm_force_reply_t;
typedef struct			nxs_chat_srv_m_tlgrm_bttn_callback_s		nxs_chat_srv_m_tlgrm_bttn_callback_t;
typedef struct			nxs_chat_srv_m_tlgrm_pre_uploads_s		nxs_chat_srv_m_tlgrm_pre_uploads_t;
typedef struct			nxs_chat_srv_m_tlgrm_webhookinfo_s		nxs_chat_srv_m_tlgrm_webhookinfo_t;
typedef struct			nxs_chat_srv_m_tlgrm_webhookset_s		nxs_chat_srv_m_tlgrm_webhookset_t;

enum nxs_chat_srv_m_tlgrm_update_type_e
{
	NXS_CHAT_SRV_M_TLGRM_UPDATE_TYPE_NONE,
	NXS_CHAT_SRV_M_TLGRM_UPDATE_TYPE_MESSAGE,
	NXS_CHAT_SRV_M_TLGRM_UPDATE_TYPE_CALLBACK,
	NXS_CHAT_SRV_M_TLGRM_UPDATE_TYPE_COMMAND,
};

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
	NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_IKM,				/* InlineKeyboardMarkup */
	NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_RKM,				/* ReplyKeyboardMarkup */
	NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_RKR,				/* ReplyKeyboardRemove */
	NXS_CHAT_SRV_M_TLGRM_REPLY_MARKUP_TYPE_FR				/* ForceReply */
};

enum nxs_chat_srv_m_tlgrm_parse_mode_e
{
	NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_NONE,
	NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN,
	NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_HTML
};

enum nxs_chat_srv_m_tlgrm_bttn_callback_type_e
{
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_NONE,
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_NEW_ISSUE,			/* create new issue */
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE,			/* add comment to existing issue */
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE_EXT_S_IN_PROGRESS,	/* add comment to existing issue (and change status to 'In progress') */
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE_EXT_S_NEED_FEEDBACK,	/* add comment to existing issue (and change status to 'Need Feedback') */
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE_EXT_S_RESOLVED,	/* add comment to existing issue (and change status to 'Resolved') */
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE_EXT_PRIVATE,		/* add comment to existing issue (and make it private) */
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE_EXT_WF_IGNORE,		/* add comment to existing issue (and ignore workflow statuses errors) */
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE_EXT_TAKE_ISSUE,	/* add comment to existing issue (and take issue to process) */
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_CREATE_ISSUE,			/* create new issue */
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECT_PROJECT,			/* select project */
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECTED_PROJECT,		/* selected project */
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECT_PRIORITY,		/* select priority */
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECTED_PRIORITY,		/* selected priority */
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SELECT_ISSUE,			/* select issues to add comment */
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_CHANGE_DESCRIPTION,		/* change issue description */
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_SESSION_DESTROY,		/* destroy current session */
	NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_BACK				/* back to previous 'window' */
};

struct nxs_chat_srv_m_tlgrm_file_s
{
	nxs_bool_t					_is_used;

	nxs_string_t					file_id;		/* Required */
	size_t						file_size;		/* Optional */
	nxs_string_t					file_path;		/* Required */
};

struct nxs_chat_srv_m_tlgrm_force_reply_s
{
	nxs_bool_t					_is_used;

	nxs_bool_t					force_reply;		/* Required */
};

struct nxs_chat_srv_m_tlgrm_inl_keybutton_s
{
	nxs_bool_t					_is_used;

	nxs_string_t					text;			/* Required */
	nxs_string_t					url;			/* Optional */
	nxs_string_t					callback_data;		/* Optional */
};

struct nxs_chat_srv_m_tlgrm_inl_keyboard_s
{
	nxs_bool_t					_is_used;

	nxs_array_t					inline_keyboard;	/* type: array of arrays nxs_chat_srv_m_tlgrm_inl_keybutton_t */
};

struct nxs_chat_srv_m_tlgrm_user_s
{
	nxs_bool_t					_is_used;

	size_t						id;			/* Required */
	nxs_string_t					first_name;		/* Required */
	nxs_string_t					last_name;		/* Optional */
	nxs_string_t					username;		/* Optional */
	nxs_string_t					language_code;		/* Optional */
};

struct nxs_chat_srv_m_tlgrm_chat_s
{
	nxs_bool_t					_is_used;

	size_t						id;			/* Required */
	nxs_string_t					username;		/* Optional */
	nxs_chat_srv_m_tlgrm_chat_type_t		type;			/* Required */
};

struct nxs_chat_srv_m_tlgrm_message_entity_s
{
	nxs_bool_t					_is_used;

	nxs_string_t					type;			/* Required */
	size_t						offset;			/* Required */
	size_t						length;			/* Required */
	nxs_string_t					url;			/* Optional */
};

struct nxs_chat_srv_m_tlgrm_photo_size_s
{
	nxs_bool_t					_is_used;

	nxs_string_t					file_id;		/* Required */
	size_t						width;			/* Required */
	size_t						height;			/* Required */
	size_t						file_size;		/* Optional */
};

struct nxs_chat_srv_m_tlgrm_document_s
{
	nxs_bool_t					_is_used;

	nxs_string_t					file_id;		/* Required */
	nxs_chat_srv_m_tlgrm_photo_size_t		thumb;			/* Optional */
	nxs_string_t					file_name;		/* Optional */
	nxs_string_t					mime_type;		/* Optional */
	size_t						file_size;		/* Optional */
};

struct nxs_chat_srv_m_tlgrm_sticker_s
{
	nxs_bool_t					_is_used;

	nxs_string_t					file_id;		/* Required */
	size_t						width;			/* Required */
	size_t						height;			/* Required */
	nxs_chat_srv_m_tlgrm_photo_size_t		thumb;			/* Optional */
	nxs_string_t					emoji;			/* Optional */
	nxs_string_t					set_name;		/* Optional */
	size_t						file_size;		/* Optional */
};

struct nxs_chat_srv_m_tlgrm_voice_s
{
	nxs_bool_t					_is_used;

	nxs_string_t					file_id;		/* Required */
	size_t						duration;		/* Required */
	nxs_string_t					mime_type;		/* Optional */
	size_t						file_size;		/* Optional */
};

struct nxs_chat_srv_m_tlgrm_video_s
{
	nxs_bool_t					_is_used;

	nxs_string_t					file_id;		/* Required */
	size_t						width;			/* Required */
	size_t						height;			/* Required */
	size_t						duration;		/* Required */
	nxs_chat_srv_m_tlgrm_photo_size_t		thumb;			/* Optional */
	nxs_string_t					mime_type;		/* Optional */
	size_t						file_size;		/* Optional */
};


struct nxs_chat_srv_m_tlgrm_message_s
{
	nxs_bool_t					_is_used;

	size_t						message_id;		/* Required */
	nxs_chat_srv_m_tlgrm_user_t			from;			/* Optional */
	nxs_chat_srv_m_tlgrm_chat_t			chat;			/* Required */
	nxs_chat_srv_m_tlgrm_message_t			*reply_to_message;	/* Optional */
	nxs_string_t					text;			/* Optional */
	nxs_string_t					caption;		/* Optional */

	nxs_array_t					photo;			/* type: nxs_chat_srv_m_tlgrm_photo_size_t. Optional */
	nxs_chat_srv_m_tlgrm_document_t			document;		/* Optional */
	nxs_chat_srv_m_tlgrm_sticker_t			sticker;		/* Optional */
	nxs_chat_srv_m_tlgrm_voice_t			voice;			/* Optional */
	nxs_chat_srv_m_tlgrm_video_t			video;			/* Optional */

	nxs_array_t					entities;		/* type: nxs_chat_srv_m_tlgrm_message_entity_t. Optional */
};

struct nxs_chat_srv_m_tlgrm_callback_query_s
{
	nxs_bool_t					_is_used;

	nxs_string_t					id;			/* Required */
	nxs_chat_srv_m_tlgrm_user_t			from;			/* Required */
	nxs_chat_srv_m_tlgrm_message_t			message;		/* Optional */
	nxs_string_t					chat_instance;		/* Required */
	nxs_string_t					data;			/* Optional */
};

struct nxs_chat_srv_m_tlgrm_update_s
{
	nxs_bool_t					_is_used;

	size_t						update_id;		/* Required */
	nxs_chat_srv_m_tlgrm_message_t			message;		/* Optional */
	nxs_chat_srv_m_tlgrm_callback_query_t		callback_query;		/* Optional */
};

struct nxs_chat_srv_m_tlgrm_bttn_callback_s
{
	nxs_chat_srv_m_tlgrm_bttn_callback_type_t	type;			/* Required */
	size_t						object_id;		/* Required */
};

struct nxs_chat_srv_m_tlgrm_pre_uploads_s
{
	nxs_string_t		file_id;
	nxs_string_t		file_name;
	nxs_string_t		mime_type;
};

struct nxs_chat_srv_m_tlgrm_webhookinfo_s
{
	nxs_bool_t					_is_used;

	nxs_string_t					url;			/* Required */
	nxs_bool_t					has_custom_certificate;	/* Required */
	size_t						pending_update_count;	/* Required */
	size_t						last_error_date;	/* Optional */
	nxs_string_t					last_error_message;	/* Optional */
	size_t						max_connections;	/* Optional */
	nxs_array_t					allowed_updates;	/* type: nxs_string_t. Optional */
};

struct nxs_chat_srv_m_tlgrm_webhookset_s
{
	nxs_bool_t					ok;			/* Required */
	nxs_string_t					description;		/* Optional */
	nxs_bool_t					result;			/* Optional */
	size_t						error_code;		/* Optional */
};

#endif /* _INCLUDE_NXS_CHAT_SRV_M_TLGRM_H */
