#ifndef _INCLUDE_NXS_CHAT_SRV_M_RDMN_H
#define _INCLUDE_NXS_CHAT_SRV_M_RDMN_H

// clang-format off

typedef struct			nxs_chat_srv_m_rdmn_project_s		nxs_chat_srv_m_rdmn_project_t;
typedef struct			nxs_chat_srv_m_rdmn_tracker_s		nxs_chat_srv_m_rdmn_tracker_t;
typedef struct			nxs_chat_srv_m_rdmn_status_s		nxs_chat_srv_m_rdmn_status_t;
typedef struct			nxs_chat_srv_m_rdmn_priority_s		nxs_chat_srv_m_rdmn_priority_t;
typedef struct			nxs_chat_srv_m_rdmn_user_s		nxs_chat_srv_m_rdmn_user_t;
typedef struct			nxs_chat_srv_m_rdmn_journal_s		nxs_chat_srv_m_rdmn_journal_t;
typedef struct			nxs_chat_srv_m_rdmn_issue_s		nxs_chat_srv_m_rdmn_issue_t;
typedef struct			nxs_chat_srv_m_rdmn_data_s		nxs_chat_srv_m_rdmn_data_t;
typedef struct			nxs_chat_srv_m_rdmn_update_s		nxs_chat_srv_m_rdmn_update_t;

struct nxs_chat_srv_m_rdmn_project_s
{
	nxs_bool_t				_is_used;

	size_t					id;
	nxs_string_t				name;
};

struct nxs_chat_srv_m_rdmn_tracker_s
{
	nxs_bool_t				_is_used;

	size_t					id;
	nxs_string_t				name;
};

struct nxs_chat_srv_m_rdmn_status_s
{
	nxs_bool_t				_is_used;

	size_t					id;
	nxs_string_t				name;
};

struct nxs_chat_srv_m_rdmn_priority_s
{
	nxs_bool_t				_is_used;

	size_t					id;
	nxs_string_t				name;
};

struct nxs_chat_srv_m_rdmn_user_s
{
	nxs_bool_t				_is_used;

	size_t					id;
	nxs_string_t				name;
};

struct nxs_chat_srv_m_rdmn_journal_s
{
	nxs_bool_t				_is_used;

	size_t					id;
	nxs_string_t				notes;

	nxs_chat_srv_m_rdmn_user_t		user;
};

struct nxs_chat_srv_m_rdmn_issue_s
{
	nxs_bool_t				_is_used;

	size_t					id;

	nxs_string_t				subject;
	nxs_string_t				description;
	size_t					done_ratio;
	nxs_bool_t				is_private;
	double					spent_hours;
	nxs_array_t				journals;		/* type: nxs_chat_srv_m_rdmn_journal_t. */
	nxs_array_t				watchers;		/* type: nxs_chat_srv_m_rdmn_user_t. */

	nxs_chat_srv_m_rdmn_project_t		project;
	nxs_chat_srv_m_rdmn_tracker_t		tracker;
	nxs_chat_srv_m_rdmn_status_t		status;
	nxs_chat_srv_m_rdmn_priority_t		priority;
	nxs_chat_srv_m_rdmn_user_t		author;
	nxs_chat_srv_m_rdmn_user_t		assigned_to;
};

struct nxs_chat_srv_m_rdmn_data_s
{
	nxs_bool_t				_is_used;

	nxs_chat_srv_m_rdmn_issue_t		issue;
};

struct nxs_chat_srv_m_rdmn_update_s
{
	nxs_string_t				action;
	nxs_chat_srv_m_rdmn_data_t		data;
};

#endif /* _INCLUDE_NXS_CHAT_SRV_M_RDMN_H */
