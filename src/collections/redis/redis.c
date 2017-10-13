#ifndef _INCLUDE_NXS_CHAT_SRV_C_REDIS_C
#define _INCLUDE_NXS_CHAT_SRV_C_REDIS_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

redisContext *nxs_chat_srv_c_redis_init(nxs_string_t *host, uint16_t port)
{
	redisContext *redis_ctx;

	if(host == NULL) {

		return NULL;
	}

	redis_ctx = redisConnect((char *)nxs_string_str(host), port);

	if(redis_ctx == NULL || redis_ctx->err != 0) {

		/* Redis connection in error state */

		if(redis_ctx != NULL) {

			nxs_log_write_error(&process, "[%s]: can't connect to Redis: %s", nxs_proc_get_name(&process), redis_ctx->errstr);
		}
		else {

			nxs_log_write_error(
			        &process, "[%s]: can't connect to Redis: can't allocate Redis context", nxs_proc_get_name(&process));
		}

		return nxs_chat_srv_c_redis_free(redis_ctx);
	}

	return redis_ctx;
}

redisContext *nxs_chat_srv_c_redis_free(redisContext *redis_ctx)
{

	if(redis_ctx != NULL) {

		redisFree(redis_ctx);
	}

	return NULL;
}

/* Module internal (static) functions */
