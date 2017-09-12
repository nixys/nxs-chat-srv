#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_CACHE_H
#define _INCLUDE_NXS_CHAT_SRV_D_DB_CACHE_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_d_db_cache_s;

enum nxs_chat_srv_d_db_cache_type_e
{
	NXS_CHAT_SRV_D_DB_CACHE_TYPE_NONE,
	NXS_CHAT_SRV_D_DB_CACHE_TYPE_PROJECTS,
	NXS_CHAT_SRV_D_DB_CACHE_TYPE_USERS,
	NXS_CHAT_SRV_D_DB_CACHE_TYPE_ISSUE_PRIORITIES,
};

/* Prototypes */

nxs_chat_srv_d_db_cache_t	*nxs_chat_srv_d_db_cache_init		(void);
nxs_chat_srv_d_db_cache_t	*nxs_chat_srv_d_db_cache_free		(nxs_chat_srv_d_db_cache_t *d_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_d_db_cache_get		(nxs_chat_srv_d_db_cache_t *d_ctx, nxs_chat_srv_d_db_cache_type_t cache_type, nxs_string_t *value);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_cache_put		(nxs_chat_srv_d_db_cache_t *d_ctx, nxs_chat_srv_d_db_cache_type_t cache_type, nxs_string_t *value);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_cache_del		(nxs_chat_srv_d_db_cache_t *d_ctx, nxs_chat_srv_d_db_cache_type_t cache_type);

#endif /* _INCLUDE_NXS_CHAT_SRV_D_DB_CACHE_H */
