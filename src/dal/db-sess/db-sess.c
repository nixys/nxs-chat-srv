#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_SESS_C
#define _INCLUDE_NXS_CHAT_SRV_D_DB_SESS_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/db-sess/db-sess-typedefs.h>
#include <dal/db-sess/db-sess.h>
#include <dal/db-sess/db-sess-subdals.h>

/* Definitions */

#define NXS_CHAT_SRV_D_DB_SESS_REDIS_PREFIX	"nxs-chat-srv:sess"

/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_d_db_sess_s
{
	redisContext					*redis_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

static redisContext *nxs_chat_srv_d_db_sess_redis_ctx_free(redisContext *redis_ctx);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_d_db_sess_t *nxs_chat_srv_d_db_sess_init(void)
{
	nxs_chat_srv_d_db_sess_t *d_ctx;

	d_ctx = (nxs_chat_srv_d_db_sess_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_d_db_sess_t));

	/* Connect to Redis */

	d_ctx->redis_ctx = redisConnect((char *)nxs_string_str(&nxs_chat_srv_cfg.redis.host), nxs_chat_srv_cfg.redis.port);

	if(d_ctx->redis_ctx == NULL || d_ctx->redis_ctx->err != 0) {

		/* Redis connection in error state */

		if(d_ctx->redis_ctx != NULL) {

			nxs_log_write_error(&process,
			                    "[%s]: db sessions error, can't connect to Redis: %s",
			                    nxs_proc_get_name(&process),
			                    d_ctx->redis_ctx->errstr);
		}
		else {

			nxs_log_write_error(&process,
			                    "[%s]: db sessions error, can't connect to Redis: can't allocate Redis context",
			                    nxs_proc_get_name(&process));
		}

		return nxs_chat_srv_d_db_sess_free(d_ctx);
	}

	return d_ctx;
}

nxs_chat_srv_d_db_sess_t *nxs_chat_srv_d_db_sess_free(nxs_chat_srv_d_db_sess_t *d_ctx)
{

	if(d_ctx == NULL) {

		return NULL;
	}

	d_ctx->redis_ctx = nxs_chat_srv_d_db_sess_redis_ctx_free(d_ctx->redis_ctx);

	return (nxs_chat_srv_d_db_sess_t *)nxs_free(d_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_sess_get(nxs_chat_srv_d_db_sess_t *d_ctx, size_t tlgrm_userid, nxs_string_t *value)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL || value == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(d_ctx->redis_ctx == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db session get error: Redis context is NULL (tlgrm userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    tlgrm_userid);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = redisCommand(d_ctx->redis_ctx, "GET %s:%lu", NXS_CHAT_SRV_D_DB_SESS_REDIS_PREFIX, tlgrm_userid)) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db session get error, Redis reply error: %s (tlgrm userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    d_ctx->redis_ctx->errstr,
		                    tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(redis_reply->type == REDIS_REPLY_STRING) {

		nxs_log_write_debug(
		        &process, "[%s]: db session get: success (tlgrm userid: %zu)", nxs_proc_get_name(&process), tlgrm_userid);

		nxs_string_char_ncpy(value, 0, (u_char *)redis_reply->str, (size_t)redis_reply->len);
	}
	else {

		if(redis_reply->type == REDIS_REPLY_NIL) {

			/* value not found by specified key */

			nxs_log_write_debug(&process,
			                    "[%s]: db session get: value does not exist (tlgrm userid: %zu)",
			                    nxs_proc_get_name(&process),
			                    tlgrm_userid);

			nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);
		}
		else {

			nxs_log_write_error(&process,
			                    "[%s]: db session get error: unexpected Redis reply type (tlgrm userid: %zu, expected type: "
			                    "%d, received type: %d)",
			                    nxs_proc_get_name(&process),
			                    tlgrm_userid,
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

		d_ctx->redis_ctx = nxs_chat_srv_d_db_sess_redis_ctx_free(d_ctx->redis_ctx);
	}

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_sess_put(nxs_chat_srv_d_db_sess_t *d_ctx, size_t tlgrm_userid, nxs_string_t *value)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL || value == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(d_ctx->redis_ctx == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db session put error: Redis context is NULL (tlgrm userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    tlgrm_userid);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = redisCommand(
	            d_ctx->redis_ctx, "SET %s:%lu %s", NXS_CHAT_SRV_D_DB_SESS_REDIS_PREFIX, tlgrm_userid, nxs_string_str(value))) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db session put error, Redis reply error: %s (tlgrm userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    d_ctx->redis_ctx->errstr,
		                    tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_log_write_debug(&process, "[%s]: db session put: success (tlgrm userid: %zu)", nxs_proc_get_name(&process), tlgrm_userid);

error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc != NXS_CHAT_SRV_E_OK) {

		d_ctx->redis_ctx = nxs_chat_srv_d_db_sess_redis_ctx_free(d_ctx->redis_ctx);
	}

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_sess_del(nxs_chat_srv_d_db_sess_t *d_ctx, size_t tlgrm_userid)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(d_ctx->redis_ctx == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db session del error: Redis context is NULL (tlgrm userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    tlgrm_userid);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = redisCommand(d_ctx->redis_ctx, "DEL %s:%lu", NXS_CHAT_SRV_D_DB_SESS_REDIS_PREFIX, tlgrm_userid)) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db session del error, Redis reply error: %s (tlgrm userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    d_ctx->redis_ctx->errstr,
		                    tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_log_write_debug(&process, "[%s]: db session del: success (tlgrm userid: %zu)", nxs_proc_get_name(&process), tlgrm_userid);

error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc != NXS_CHAT_SRV_E_OK) {

		d_ctx->redis_ctx = nxs_chat_srv_d_db_sess_redis_ctx_free(d_ctx->redis_ctx);
	}

	return rc;
}

/* Module internal (static) functions */

static redisContext *nxs_chat_srv_d_db_sess_redis_ctx_free(redisContext *redis_ctx)
{

	if(redis_ctx != NULL) {

		redisFree(redis_ctx);
	}

	return NULL;
}
