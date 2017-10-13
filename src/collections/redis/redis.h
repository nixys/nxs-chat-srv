#ifndef _INCLUDE_NXS_CHAT_SRV_C_REDIS_H
#define _INCLUDE_NXS_CHAT_SRV_C_REDIS_H

// clang-format off

/* Prototypes */

redisContext			*nxs_chat_srv_c_redis_init		(nxs_string_t *host, uint16_t port);
redisContext			*nxs_chat_srv_c_redis_free		(redisContext *redis_ctx);

#endif /* _INCLUDE_NXS_CHAT_SRV_C_REDIS_H */
