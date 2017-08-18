#ifndef _INCLUDE_NXS_CHAT_SRV_M_DB_USERS_H
#define _INCLUDE_NXS_CHAT_SRV_M_DB_USERS_H

// clang-format off

typedef struct
{
	size_t			id;
	size_t			tlgrm_userid;
	size_t			rdmn_userid;
	nxs_string_t		*tlgrm_username;
	nxs_string_t		*tlgrm_userlang;
	nxs_string_t		*rdmn_username;
	nxs_string_t		*rdmn_userfname;
	nxs_string_t		*rdmn_userlname;
	nxs_string_t		*rdmn_user_api_key;
} nxs_chat_srv_m_db_users_t;

#endif /* _INCLUDE_NXS_CHAT_SRV_M_DB_USERS_H */
