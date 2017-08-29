#ifndef _INCLUDE_NXS_CHAT_SRV_M_DB_SESS_H
#define _INCLUDE_NXS_CHAT_SRV_M_DB_SESS_H

// clang-format off

typedef enum
{
	NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE,
	NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE,
	NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE
} nxs_chat_srv_m_db_sess_type_t;

typedef enum
{
	NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE,
	NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_SUBJECT,
	NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_DESCRIPTION
} nxs_chat_srv_m_db_sess_wait_for_type_t;

typedef struct
{
	size_t						id;
	size_t						tlgrm_userid;
	size_t						chat_id;
	size_t						message_id;
	time_t						updated_at;
	nxs_chat_srv_m_db_sess_type_t			type;
	nxs_chat_srv_m_db_sess_wait_for_type_t		wait_for;
	nxs_string_t					*data;
} nxs_chat_srv_m_db_sess_t;

#endif /* _INCLUDE_NXS_CHAT_SRV_M_DB_SESS_H */
