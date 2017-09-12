#ifndef _INCLUDE_NXS_CHAT_SRV_M_DB_CACHE_H
#define _INCLUDE_NXS_CHAT_SRV_M_DB_CACHE_H

// clang-format off

typedef struct
{
	size_t			id;
	nxs_string_t		*name;
} nxs_chat_srv_m_db_cache_project_t;

typedef struct
{
	size_t			id;
	nxs_bool_t		is_default;
	nxs_string_t		*name;
} nxs_chat_srv_m_db_cache_issue_priority_t;

#endif /* _INCLUDE_NXS_CHAT_SRV_M_DB_CACHE_H */
