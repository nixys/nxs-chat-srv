#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_CACHE_C
#define _INCLUDE_NXS_CHAT_SRV_D_DB_CACHE_C
#endif

// clang-format off

#include <hiredis/hiredis.h>

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/db-cache/db-cache-typedefs.h>
#include <dal/db-cache/db-cache.h>
#include <dal/db-cache/db-cache-subdals.h>

/* Definitions */

#define NXS_CHAT_SRV_D_DB_CACHE_REDIS_PREFIX	"nxs-chat-srv:cache"

/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	nxs_chat_srv_d_db_cache_type_t		type;
	char					*name;
} nxs_chat_srv_d_db_cache_name_t;

struct nxs_chat_srv_d_db_cache_s
{
	redisContext		*redis_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

static redisContext *nxs_chat_srv_d_db_cache_redis_ctx_free(redisContext *redis_ctx);
static char *nxs_chat_srv_d_db_cache_get_name(nxs_chat_srv_d_db_cache_type_t cache_type);

// clang-format off

/* Module initializations */

nxs_chat_srv_d_db_cache_name_t cache_names[] =
{
	{NXS_CHAT_SRV_D_DB_CACHE_TYPE_PROJECTS,			"projects"},
	{NXS_CHAT_SRV_D_DB_CACHE_TYPE_USERS,			"users"},
	{NXS_CHAT_SRV_D_DB_CACHE_TYPE_ISSUE_PRIORITIES,		"issue_priorities"},

	{NXS_CHAT_SRV_D_DB_CACHE_TYPE_NONE,			""}
};

/* Module global functions */

// clang-format on

nxs_chat_srv_d_db_cache_t *nxs_chat_srv_d_db_cache_init(void)
{
	nxs_chat_srv_d_db_cache_t *d_ctx;

	d_ctx = (nxs_chat_srv_d_db_cache_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_d_db_cache_t));

	/* Connect to Redis */

	d_ctx->redis_ctx = redisConnect((char *)nxs_string_str(&nxs_chat_srv_cfg.redis.host), nxs_chat_srv_cfg.redis.port);

	if(d_ctx->redis_ctx == NULL || d_ctx->redis_ctx->err != 0) {

		/* Redis connection in error state */

		if(d_ctx->redis_ctx != NULL) {

			nxs_log_write_error(&process,
			                    "[%s]: cache error, can't connect to Redis: %s",
			                    nxs_proc_get_name(&process),
			                    d_ctx->redis_ctx->errstr);
		}
		else {

			nxs_log_write_error(&process,
			                    "[%s]: cache error, can't connect to Redis: can't allocate Redis context",
			                    nxs_proc_get_name(&process));
		}

		return nxs_chat_srv_d_db_cache_free(d_ctx);
	}

	return d_ctx;
}

nxs_chat_srv_d_db_cache_t *nxs_chat_srv_d_db_cache_free(nxs_chat_srv_d_db_cache_t *d_ctx)
{

	if(d_ctx == NULL) {

		return NULL;
	}

	d_ctx->redis_ctx = nxs_chat_srv_d_db_cache_redis_ctx_free(d_ctx->redis_ctx);

	return (nxs_chat_srv_d_db_cache_t *)nxs_free(d_ctx);
}

nxs_chat_srv_err_t
        nxs_chat_srv_d_db_cache_get(nxs_chat_srv_d_db_cache_t *d_ctx, nxs_chat_srv_d_db_cache_type_t cache_type, nxs_string_t *value)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;
	char *             cache_name;

	if(d_ctx == NULL || value == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(d_ctx->redis_ctx == NULL) {

		nxs_log_write_error(&process, "[%s]: cache get error: Redis context is NULL", nxs_proc_get_name(&process));

		return NXS_CHAT_SRV_E_ERR;
	}

	if((cache_name = nxs_chat_srv_d_db_cache_get_name(cache_type)) == NULL) {

		nxs_log_write_error(
		        &process, "[%s]: cache get error: unknown cache type (cache type: %d)", nxs_proc_get_name(&process), cache_type);

		return NXS_CHAT_SRV_E_TYPE;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = redisCommand(d_ctx->redis_ctx, "GET %s:%s", NXS_CHAT_SRV_D_DB_CACHE_REDIS_PREFIX, cache_name)) == NULL) {

		nxs_log_write_error(
		        &process, "[%s]: cache get error, Redis reply error: %s", nxs_proc_get_name(&process), d_ctx->redis_ctx->errstr);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(redis_reply->type == REDIS_REPLY_STRING) {

		nxs_string_char_ncpy(value, 0, (u_char *)redis_reply->str, (size_t)redis_reply->len);
	}
	else {

		if(redis_reply->type == REDIS_REPLY_NIL) {

			/* value not found by specified key */

			nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);
		}
		else {

			nxs_log_write_error(&process,
			                    "[%s]: cache get error: unexpected Redis reply type (expected type: %d, received type: %d)",
			                    nxs_proc_get_name(&process),
			                    REDIS_REPLY_STRING,
			                    redis_reply->type);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}

error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc != NXS_CHAT_SRV_E_OK && rc != NXS_CHAT_SRV_E_EXIST) {

		d_ctx->redis_ctx = nxs_chat_srv_d_db_cache_redis_ctx_free(d_ctx->redis_ctx);
	}

	return rc;
}

nxs_chat_srv_err_t
        nxs_chat_srv_d_db_cache_put(nxs_chat_srv_d_db_cache_t *d_ctx, nxs_chat_srv_d_db_cache_type_t cache_type, nxs_string_t *value)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;
	char *             cache_name;

	if(d_ctx == NULL || value == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(d_ctx->redis_ctx == NULL) {

		nxs_log_write_error(&process, "[%s]: cache put error: Redis context is NULL", nxs_proc_get_name(&process));

		return NXS_CHAT_SRV_E_ERR;
	}

	if((cache_name = nxs_chat_srv_d_db_cache_get_name(cache_type)) == NULL) {

		nxs_log_write_error(
		        &process, "[%s]: cache put error: unknown cache type (cache type: %d)", nxs_proc_get_name(&process), cache_type);

		return NXS_CHAT_SRV_E_TYPE;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = redisCommand(
	            d_ctx->redis_ctx, "SET %s:%s %s", NXS_CHAT_SRV_D_DB_CACHE_REDIS_PREFIX, cache_name, nxs_string_str(value))) == NULL) {

		nxs_log_write_error(
		        &process, "[%s]: cache put error, Redis reply error: %s", nxs_proc_get_name(&process), d_ctx->redis_ctx->errstr);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc != NXS_CHAT_SRV_E_OK) {

		d_ctx->redis_ctx = nxs_chat_srv_d_db_cache_redis_ctx_free(d_ctx->redis_ctx);
	}

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_cache_del(nxs_chat_srv_d_db_cache_t *d_ctx, nxs_chat_srv_d_db_cache_type_t cache_type)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;
	char *             cache_name;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(d_ctx->redis_ctx == NULL) {

		nxs_log_write_error(&process, "[%s]: cache del error: Redis context is NULL", nxs_proc_get_name(&process));

		return NXS_CHAT_SRV_E_ERR;
	}

	if((cache_name = nxs_chat_srv_d_db_cache_get_name(cache_type)) == NULL) {

		nxs_log_write_error(
		        &process, "[%s]: cache del error: unknown cache type (cache type: %d)", nxs_proc_get_name(&process), cache_type);

		return NXS_CHAT_SRV_E_TYPE;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = redisCommand(d_ctx->redis_ctx, "DEL %s:%s", NXS_CHAT_SRV_D_DB_CACHE_REDIS_PREFIX, cache_name)) == NULL) {

		nxs_log_write_error(
		        &process, "[%s]: cache del error, Redis reply error: %s", nxs_proc_get_name(&process), d_ctx->redis_ctx->errstr);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc != NXS_CHAT_SRV_E_OK) {

		d_ctx->redis_ctx = nxs_chat_srv_d_db_cache_redis_ctx_free(d_ctx->redis_ctx);
	}

	return rc;
}

/* Module internal (static) functions */

static redisContext *nxs_chat_srv_d_db_cache_redis_ctx_free(redisContext *redis_ctx)
{

	if(redis_ctx != NULL) {

		redisFree(redis_ctx);
	}

	return NULL;
}

static char *nxs_chat_srv_d_db_cache_get_name(nxs_chat_srv_d_db_cache_type_t cache_type)
{
	size_t i;

	for(i = 0; cache_names[i].type != NXS_CHAT_SRV_D_DB_CACHE_TYPE_NONE; i++) {

		if(cache_names[i].type == cache_type) {

			return cache_names[i].name;
		}
	}

	return NULL;
}
