#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_IDS_C
#define _INCLUDE_NXS_CHAT_SRV_D_DB_IDS_C
#endif

// clang-format off

#include <hiredis/hiredis.h>

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/db-ids/db-ids-typedefs.h>
#include <dal/db-ids/db-ids.h>
#include <dal/db-ids/db-ids-subdals.h>

/* Definitions */

#define NXS_CHAT_SRV_D_DB_IDS_REDIS_PREFIX	"nxs-chat-srv:ids"

/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_d_db_ids_s
{
	redisContext				*redis_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

static redisContext *nxs_chat_srv_d_db_ids_redis_ctx_free(redisContext *redis_ctx);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_d_db_ids_t *nxs_chat_srv_d_db_ids_init(void)
{
	nxs_chat_srv_d_db_ids_t *d_ctx;

	d_ctx = (nxs_chat_srv_d_db_ids_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_d_db_ids_t));

	/* Connect to Redis */

	d_ctx->redis_ctx = redisConnect((char *)nxs_string_str(&nxs_chat_srv_cfg.redis.host), nxs_chat_srv_cfg.redis.port);

	if(d_ctx->redis_ctx == NULL || d_ctx->redis_ctx->err != 0) {

		/* Redis connection in error state */

		if(d_ctx->redis_ctx != NULL) {

			nxs_log_write_error(&process,
			                    "[%s]: db ids error, can't connect to Redis: %s",
			                    nxs_proc_get_name(&process),
			                    d_ctx->redis_ctx->errstr);
		}
		else {

			nxs_log_write_error(&process,
			                    "[%s]: db ids error, can't connect to Redis: can't allocate Redis context",
			                    nxs_proc_get_name(&process));
		}

		return nxs_chat_srv_d_db_ids_free(d_ctx);
	}

	return d_ctx;
}

nxs_chat_srv_d_db_ids_t *nxs_chat_srv_d_db_ids_free(nxs_chat_srv_d_db_ids_t *d_ctx)
{

	if(d_ctx == NULL) {

		return NULL;
	}

	d_ctx->redis_ctx = nxs_chat_srv_d_db_ids_redis_ctx_free(d_ctx->redis_ctx);

	return (nxs_chat_srv_d_db_ids_t *)nxs_free(d_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_ids_get(nxs_chat_srv_d_db_ids_t *d_ctx, size_t rdmn_userid, nxs_string_t *value)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL || value == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(d_ctx->redis_ctx == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db ids get error: Redis context is NULL (rdmn userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    rdmn_userid);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = redisCommand(d_ctx->redis_ctx, "HGET %s %lu", NXS_CHAT_SRV_D_DB_IDS_REDIS_PREFIX, rdmn_userid)) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db ids get error, Redis reply error: %s (rdmn userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    d_ctx->redis_ctx->errstr,
		                    rdmn_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(redis_reply->type == REDIS_REPLY_STRING) {

		nxs_log_write_debug(&process, "[%s]: db ids get: success (rdmn userid: %zu)", nxs_proc_get_name(&process), rdmn_userid);

		nxs_string_char_ncpy(value, 0, (u_char *)redis_reply->str, (size_t)redis_reply->len);
	}
	else {

		if(redis_reply->type == REDIS_REPLY_NIL) {

			/* value not found by specified key */

			nxs_log_write_debug(&process,
			                    "[%s]: db ids get: value does not exist (rdmn userid: %zu)",
			                    nxs_proc_get_name(&process),
			                    rdmn_userid);

			nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);
		}
		else {

			nxs_log_write_error(&process,
			                    "[%s]: db ids get error: unexpected Redis reply type (rdmn userid: %zu, expected type: %d, "
			                    "received type: %d)",
			                    nxs_proc_get_name(&process),
			                    rdmn_userid,
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

		d_ctx->redis_ctx = nxs_chat_srv_d_db_ids_redis_ctx_free(d_ctx->redis_ctx);
	}

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_ids_put(nxs_chat_srv_d_db_ids_t *d_ctx, size_t rdmn_userid, nxs_string_t *value)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL || value == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(d_ctx->redis_ctx == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db ids put error: Redis context is NULL (rdmn userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    rdmn_userid);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = redisCommand(
	            d_ctx->redis_ctx, "HSET %s %lu %s", NXS_CHAT_SRV_D_DB_IDS_REDIS_PREFIX, rdmn_userid, nxs_string_str(value))) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db ids put error, Redis reply error: %s (rdmn userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    d_ctx->redis_ctx->errstr,
		                    rdmn_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_log_write_debug(&process, "[%s]: db ids put: success (rdmn userid: %zu)", nxs_proc_get_name(&process), rdmn_userid);

error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc != NXS_CHAT_SRV_E_OK) {

		d_ctx->redis_ctx = nxs_chat_srv_d_db_ids_redis_ctx_free(d_ctx->redis_ctx);
	}

	return rc;
}

/* Module internal (static) functions */

static redisContext *nxs_chat_srv_d_db_ids_redis_ctx_free(redisContext *redis_ctx)
{

	if(redis_ctx != NULL) {

		redisFree(redis_ctx);
	}

	return NULL;
}
