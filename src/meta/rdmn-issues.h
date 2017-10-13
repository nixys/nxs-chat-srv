#ifndef _INCLUDE_NXS_CHAT_SRV_M_RDMN_ISSUES_H
#define _INCLUDE_NXS_CHAT_SRV_M_RDMN_ISSUES_H

// clang-format off

typedef struct
{
	size_t			id;
	nxs_string_t		subject;
} nxs_chat_srv_m_rdmn_issues_query_t;

typedef struct
{
	size_t			id;
	nxs_string_t		value;
} nxs_chat_srv_m_rdmn_issues_cf_t;

typedef struct
{
	nxs_string_t		file_path;
	nxs_string_t		file_name;
} nxs_chat_srv_m_rdmn_issues_upload_t;

#endif /* _INCLUDE_NXS_CHAT_SRV_M_RDMN_ISSUES_H */
