#ifndef _INCLUDE_NXS_CHAT_SRV_P_CACHE_WORKER_CTX_H
#define _INCLUDE_NXS_CHAT_SRV_P_CACHE_WORKER_CTX_H

// clang-format off

typedef struct
{
	time_t			projects_updated_at;
	time_t			users_updated_at;
	time_t			issue_priorities_updated_at;
} nxs_chat_srv_p_cache_worker_ctx_t;

void				nxs_chat_srv_p_cache_worker_ctx_init	(nxs_chat_srv_p_cache_worker_ctx_t *p_ctx);
void				nxs_chat_srv_p_cache_worker_ctx_free	(nxs_chat_srv_p_cache_worker_ctx_t *p_ctx);

#endif /* _INCLUDE_NXS_CHAT_SRV_P_CACHE_WORKER_CTX_H */
