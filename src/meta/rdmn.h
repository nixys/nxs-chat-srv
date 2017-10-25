#ifndef _INCLUDE_NXS_CHAT_SRV_M_RDMN_H
#define _INCLUDE_NXS_CHAT_SRV_M_RDMN_H

// clang-format off

typedef enum			nxs_chat_srv_m_rdmn_detail_type_e		nxs_chat_srv_m_rdmn_detail_type_t;
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
typedef struct			nxs_chat_srv_m_rdmn_detail_attachment_s		nxs_chat_srv_m_rdmn_detail_attachment_t;
typedef struct			nxs_chat_srv_m_rdmn_detail_attr_s		nxs_chat_srv_m_rdmn_detail_attr_t;
typedef struct			nxs_chat_srv_m_rdmn_detail_s			nxs_chat_srv_m_rdmn_detail_t;
typedef struct			nxs_chat_srv_m_rdmn_journal_s			nxs_chat_srv_m_rdmn_journal_t;
typedef struct			nxs_chat_srv_m_rdmn_issue_s			nxs_chat_srv_m_rdmn_issue_t;
typedef struct			nxs_chat_srv_m_rdmn_data_s			nxs_chat_srv_m_rdmn_data_t;
typedef struct			nxs_chat_srv_m_rdmn_update_s			nxs_chat_srv_m_rdmn_update_t;
typedef struct			nxs_chat_srv_m_rdmn_attachment_s		nxs_chat_srv_m_rdmn_attachment_t;

enum nxs_chat_srv_m_rdmn_detail_type_e
{
	NXS_CHAT_SRV_M_RDMN_DETAIL_TYPE_NONE,
	NXS_CHAT_SRV_M_RDMN_DETAIL_TYPE_ATTR,
	NXS_CHAT_SRV_M_RDMN_DETAIL_TYPE_ATTACHMENT
};

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

	nxs_bool_t				view_current_issue;	/* Required */
	nxs_bool_t				view_private_notes;	/* Required */
};

struct nxs_chat_srv_m_rdmn_permissions_s
{
	nxs_bool_t				_is_used;

	nxs_chat_srv_m_rdmn_issues_visibility_t	issues_visibility;	/* Required */
	nxs_bool_t				view_private_notes;	/* Required */
};

struct nxs_chat_srv_m_rdmn_role_s
{
	nxs_bool_t				_is_used;

	size_t					id;			/* Required */
	nxs_string_t				name;			/* Optional */
	nxs_chat_srv_m_rdmn_permissions_t	permissions;		/* Required */
};

struct nxs_chat_srv_m_rdmn_member_s
{
	nxs_bool_t				_is_used;

	size_t					id;			/* Required */
	nxs_string_t				name;			/* Optional */
	nxs_chat_srv_m_rdmn_access_t		access;			/* Required */
	nxs_array_t				roles;			/* type: nxs_chat_srv_m_rdmn_role_t. Required */
};

struct nxs_chat_srv_m_rdmn_project_s
{
	nxs_bool_t				_is_used;

	size_t					id;			/* Required */
	nxs_string_t				name;			/* Optional */
	nxs_array_t				members;		/* type: nxs_chat_srv_m_rdmn_member_t. Optional */
};

struct nxs_chat_srv_m_rdmn_tracker_s
{
	nxs_bool_t				_is_used;

	size_t					id;			/* Required */
	nxs_string_t				name;			/* Optional */
};

struct nxs_chat_srv_m_rdmn_status_s
{
	nxs_bool_t				_is_used;

	size_t					id;			/* Required */
	nxs_string_t				name;			/* Optional */
};

struct nxs_chat_srv_m_rdmn_priority_s
{
	nxs_bool_t				_is_used;

	size_t					id;			/* Required */
	nxs_string_t				name;			/* Optional */
};

struct nxs_chat_srv_m_rdmn_user_s
{
	nxs_bool_t				_is_used;

	size_t					id;			/* Required */
	nxs_string_t				name;			/* Optional */
};

struct nxs_chat_srv_m_rdmn_detail_attachment_s
{
	nxs_bool_t				_is_used;

	size_t					name;			/* Required */
};

struct nxs_chat_srv_m_rdmn_detail_attr_s
{
	nxs_bool_t				_is_used;

	nxs_string_t				name;			/* Required */
};

struct nxs_chat_srv_m_rdmn_detail_s
{
	nxs_bool_t				_is_used;

	nxs_chat_srv_m_rdmn_detail_type_t	type;

	nxs_chat_srv_m_rdmn_detail_attachment_t	attachment;
	nxs_chat_srv_m_rdmn_detail_attr_t	attr;
};

struct nxs_chat_srv_m_rdmn_journal_s
{
	nxs_bool_t				_is_used;

	size_t					id;			/* Optional */
	nxs_string_t				notes;			/* Optional */
	nxs_bool_t				private_notes;		/* Optional */

	nxs_chat_srv_m_rdmn_user_t		user;			/* Required */

	nxs_array_t				details;		/* type: nxs_chat_srv_m_rdmn_detail_t. Optional */
};

struct nxs_chat_srv_m_rdmn_attachment_s
{
	nxs_bool_t				_is_used;

	size_t					id;			/* Required */
	nxs_string_t				filename;		/* Required */
	size_t					filesize;		/* Optional */
	nxs_string_t				content_type;		/* Optional */
	nxs_string_t				description;		/* Optional */
	nxs_string_t				content_url;		/* Required / Optional */
	nxs_chat_srv_m_rdmn_user_t		author;			/* Optional */
	nxs_string_t				created_on;		/* Optional */
};

struct nxs_chat_srv_m_rdmn_issue_s
{
	nxs_bool_t				_is_used;

	size_t					id;			/* Required */

	nxs_string_t				subject;		/* Required */
	nxs_string_t				description;		/* Required */
	size_t					done_ratio;		/* Optional */
	nxs_bool_t				is_private;		/* Optional */
	double					spent_hours;		/* Optional */
	nxs_array_t				journals;		/* type: nxs_chat_srv_m_rdmn_journal_t. Optional */
	nxs_array_t				watchers;		/* type: nxs_chat_srv_m_rdmn_user_t. Optional */
	nxs_array_t				cf_watchers;		/* type: size_t. Optional */
	nxs_array_t				attachments;		/* type: nxs_chat_srv_m_rdmn_attachment_t. Optional */

	nxs_chat_srv_m_rdmn_project_t		project;		/* Required */
	nxs_chat_srv_m_rdmn_tracker_t		tracker;		/* Required */
	nxs_chat_srv_m_rdmn_status_t		status;			/* Required */
	nxs_chat_srv_m_rdmn_priority_t		priority;		/* Required */
	nxs_chat_srv_m_rdmn_user_t		author;			/* Required */
	nxs_chat_srv_m_rdmn_user_t		assigned_to;		/* Optional */
};

struct nxs_chat_srv_m_rdmn_data_s
{
	nxs_bool_t				_is_used;

	nxs_chat_srv_m_rdmn_issue_t		issue;			/* Required */
};

struct nxs_chat_srv_m_rdmn_update_s
{
	nxs_string_t				action;			/* Required */
	nxs_chat_srv_m_rdmn_data_t		data;			/* Required */
};

#endif /* _INCLUDE_NXS_CHAT_SRV_M_RDMN_H */
