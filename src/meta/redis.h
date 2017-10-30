#ifndef _INCLUDE_NXS_CHAT_SRV_M_REDIS_H
#define _INCLUDE_NXS_CHAT_SRV_M_REDIS_H

// clang-format off

typedef struct
{
	nxs_bool_t		is_cluster;
	redisContext		*redis_ctx;
	redisClusterContext	*redis_cluster_ctx;
} nxs_chat_srv_m_redis_ctx_t;

#endif /* _INCLUDE_NXS_CHAT_SRV_M_REDIS_H */
