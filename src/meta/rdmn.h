#ifndef _INCLUDE_NXS_CHAT_SRV_M_RDMN_H
#define _INCLUDE_NXS_CHAT_SRV_M_RDMN_H

// clang-format off

typedef enum			nxs_chat_srv_m_rdmn_issues_visibility_e		nxs_chat_srv_m_rdmn_issues_visibility_t;

typedef struct			nxs_chat_srv_m_rdmn_access_s			nxs_chat_srv_m_rdmn_access_t;
typedef struct			nxs_chat_srv_m_rdmn_permissions_s		nxs_chat_srv_m_rdmn_permissions_t;
typedef struct			nxs_chat_srv_m_rdmn_role_s			nxs_chat_srv_m_rdmn_role_t;
typedef struct			nxs_chat_srv_m_rdmn_member_s			nxs_chat_srv_m_rdmn_member_t;
typedef struct			nxs_chat_srv_m_rdmn_project_s			nxs_chat_srv_m_rdmn_project_t;
typedef struct			nxs_chat_srv_m_rdmn_tracker_s			nxs_chat_srv_m_rdmn_tracker_t;
typedef struct			nxs_chat_srv_m_rdmn_status_s			nxs_chat_srv_m_rdmn_status_t;
typedef struct			nxs_chat_srv_m_rdmn_priority_s			nxs_chat_srv_m_rdmn_priority_t;
typedef struct			nxs_chat_srv_m_rdmn_user_s			nxs_chat_srv_m_rdmn_user_t;
typedef struct			nxs_chat_srv_m_rdmn_detail_s			nxs_chat_srv_m_rdmn_detail_t;
typedef struct			nxs_chat_srv_m_rdmn_journal_s			nxs_chat_srv_m_rdmn_journal_t;
typedef struct			nxs_chat_srv_m_rdmn_issue_s			nxs_chat_srv_m_rdmn_issue_t;
typedef struct			nxs_chat_srv_m_rdmn_data_s			nxs_chat_srv_m_rdmn_data_t;
typedef struct			nxs_chat_srv_m_rdmn_update_s			nxs_chat_srv_m_rdmn_update_t;

enum nxs_chat_srv_m_rdmn_issues_visibility_e
{
	NXS_CHAT_SRV_M_RDMN_ISSUES_VISIBILITY_NONE,
	NXS_CHAT_SRV_M_RDMN_ISSUES_VISIBILITY_ALL,
	NXS_CHAT_SRV_M_RDMN_ISSUES_VISIBILITY_DEFAULT,
	NXS_CHAT_SRV_M_RDMN_ISSUES_VISIBILITY_OWN
};

struct nxs_chat_srv_m_rdmn_access_s
{
	nxs_bool_t				_is_used;

	nxs_bool_t				view_current_issue;
	nxs_bool_t				view_private_notes;
};

struct nxs_chat_srv_m_rdmn_permissions_s
{
	nxs_bool_t				_is_used;

	nxs_chat_srv_m_rdmn_issues_visibility_t	issues_visibility;
	nxs_bool_t				view_private_notes;
};

struct nxs_chat_srv_m_rdmn_role_s
{
	nxs_bool_t				_is_used;

	size_t					id;
	nxs_string_t				name;
	nxs_chat_srv_m_rdmn_permissions_t	permissions;
};

struct nxs_chat_srv_m_rdmn_member_s
{
	nxs_bool_t				_is_used;

	size_t					id;
	nxs_string_t				name;
	nxs_chat_srv_m_rdmn_access_t		access;
	nxs_array_t				roles;			/* type: nxs_chat_srv_m_rdmn_role_t */
};

struct nxs_chat_srv_m_rdmn_project_s
{
	nxs_bool_t				_is_used;

	size_t					id;
	nxs_string_t				name;
	nxs_array_t				members;		/* type: nxs_chat_srv_m_rdmn_member_t */
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

struct nxs_chat_srv_m_rdmn_detail_s
{
	nxs_bool_t				_is_used;

	nxs_string_t				property;
	nxs_string_t				name;
};

struct nxs_chat_srv_m_rdmn_journal_s
{
	nxs_bool_t				_is_used;

	size_t					id;
	nxs_string_t				notes;
	nxs_bool_t				private_notes;

	nxs_chat_srv_m_rdmn_user_t		user;

	nxs_array_t				details;		/* type: nxs_chat_srv_m_rdmn_detail_t. */
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
	nxs_array_t				cf_watchers;		/* type: size_t. */

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
