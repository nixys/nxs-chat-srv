#ifndef _INCLUDE_NXS_CHAT_SRV_C_REDIS_H
#define _INCLUDE_NXS_CHAT_SRV_C_REDIS_H

// clang-format off

/* Prototypes */

nxs_chat_srv_err_t		nxs_chat_srv_c_redis_init		(nxs_chat_srv_m_redis_ctx_t *redis_ctx, nxs_array_t *nodes, nxs_bool_t is_cluster);
nxs_chat_srv_err_t		nxs_chat_srv_c_redis_free		(nxs_chat_srv_m_redis_ctx_t *redis_ctx);

redisReply			*nxs_chat_srv_c_redis_command		(nxs_chat_srv_m_redis_ctx_t *redis_ctx, const char *query, ...);

char				*nxs_chat_srv_c_redis_get_error		(nxs_chat_srv_m_redis_ctx_t *redis_ctx);
void				nxs_chat_srv_c_redis_get_prefix		(nxs_string_t *prefix_dst, char *prefix_fmt);

#endif /* _INCLUDE_NXS_CHAT_SRV_C_REDIS_H */
