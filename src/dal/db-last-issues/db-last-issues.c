#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_LAST_ISSUES_C
#define _INCLUDE_NXS_CHAT_SRV_D_DB_LAST_ISSUES_C
#endif

// clang-format off

#include <hiredis/hiredis.h>

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/db-last-issues/db-last-issues-typedefs.h>
#include <dal/db-last-issues/db-last-issues.h>
#include <dal/db-last-issues/db-last-issues-subdals.h>

/* Definitions */

#define NXS_CHAT_SRV_D_DB_LAST_ISSUES_REDIS_PREFIX	"nxs-chat-srv:last-issues"

/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_d_db_last_issues_s
{
	redisContext				*redis_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

static redisContext *nxs_chat_srv_d_db_last_issues_redis_ctx_free(redisContext *redis_ctx);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_d_db_last_issues_t *nxs_chat_srv_d_db_last_issues_init(void)
{
	nxs_chat_srv_d_db_last_issues_t *d_ctx;

	d_ctx = (nxs_chat_srv_d_db_last_issues_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_d_db_last_issues_t));

	/* Connect to Redis */

	d_ctx->redis_ctx = redisConnect((char *)nxs_string_str(&nxs_chat_srv_cfg.redis.host), nxs_chat_srv_cfg.redis.port);

	if(d_ctx->redis_ctx == NULL || d_ctx->redis_ctx->err != 0) {

		/* Redis connection in error state */

		if(d_ctx->redis_ctx != NULL) {

			nxs_log_write_error(&process,
			                    "[%s]: db last issues error, can't connect to Redis: %s",
			                    nxs_proc_get_name(&process),
			                    d_ctx->redis_ctx->errstr);
		}
		else {

			nxs_log_write_error(&process,
			                    "[%s]: db last issues error, can't connect to Redis: can't allocate Redis context",
			                    nxs_proc_get_name(&process));
		}

		return nxs_chat_srv_d_db_last_issues_free(d_ctx);
	}

	return d_ctx;
}

nxs_chat_srv_d_db_last_issues_t *nxs_chat_srv_d_db_last_issues_free(nxs_chat_srv_d_db_last_issues_t *d_ctx)
{

	if(d_ctx == NULL) {

		return NULL;
	}

	d_ctx->redis_ctx = nxs_chat_srv_d_db_last_issues_redis_ctx_free(d_ctx->redis_ctx);

	return (nxs_chat_srv_d_db_last_issues_t *)nxs_free(d_ctx);
}

nxs_chat_srv_err_t
        nxs_chat_srv_d_db_last_issues_get(nxs_chat_srv_d_db_last_issues_t *d_ctx, size_t tlgrm_userid, size_t *rdmn_last_issue_id)
{
	redisReply *       redis_reply;
	nxs_string_t       value;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL || rdmn_last_issue_id == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(d_ctx->redis_ctx == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db last issues get error: Redis context is NULL (tlgrm userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    tlgrm_userid);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&value);

	if((redis_reply = redisCommand(d_ctx->redis_ctx, "HGET %s %lu", NXS_CHAT_SRV_D_DB_LAST_ISSUES_REDIS_PREFIX, tlgrm_userid)) ==
	   NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db last issues get error, Redis reply error: %s (tlgrm userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    d_ctx->redis_ctx->errstr,
		                    tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(redis_reply->type == REDIS_REPLY_STRING) {

		nxs_string_char_ncpy(&value, 0, (u_char *)redis_reply->str, (size_t)redis_reply->len);

		*rdmn_last_issue_id = nxs_string_atoi(&value);

		nxs_log_write_debug(&process,
		                    "[%s]: db last issues get: success (tlgrm userid: %zu, rdmn last issue id: %zu)",
		                    nxs_proc_get_name(&process),
		                    tlgrm_userid,
		                    *rdmn_last_issue_id);
	}
	else {

		if(redis_reply->type == REDIS_REPLY_NIL) {

			/* value not found by specified key */

			nxs_log_write_debug(&process,
			                    "[%s]: db last issues get: value does not exist (tlgrm userid: %zu)",
			                    nxs_proc_get_name(&process),
			                    tlgrm_userid);

			nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);
		}
		else {

			nxs_log_write_error(
			        &process,
			        "[%s]: db last issues get error: unexpected Redis reply type (tlgrm userid: %zu, expected type: "
			        "%d, received type: %d)",
			        nxs_proc_get_name(&process),
			        tlgrm_userid,
			        REDIS_REPLY_STRING,
			        redis_reply->type);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}

error:

	nxs_string_free(&value);

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc != NXS_CHAT_SRV_E_OK && rc != NXS_CHAT_SRV_E_EXIST) {

		d_ctx->redis_ctx = nxs_chat_srv_d_db_last_issues_redis_ctx_free(d_ctx->redis_ctx);
	}

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_last_issues_put(nxs_chat_srv_d_db_last_issues_t *d_ctx, size_t tlgrm_userid, size_t rdmn_last_issue_id)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(d_ctx->redis_ctx == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db last issues put error: Redis context is NULL (tlgrm userid: %zu, rdmn last issue id: %zu)",
		                    nxs_proc_get_name(&process),
		                    tlgrm_userid,
		                    rdmn_last_issue_id);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = redisCommand(
	            d_ctx->redis_ctx, "HSET %s %lu %lu", NXS_CHAT_SRV_D_DB_LAST_ISSUES_REDIS_PREFIX, tlgrm_userid, rdmn_last_issue_id)) ==
	   NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db last issues put error, Redis reply error: %s (tlgrm userid: %zu, rdmn last issue id: %zu)",
		                    nxs_proc_get_name(&process),
		                    d_ctx->redis_ctx->errstr,
		                    tlgrm_userid,
		                    rdmn_last_issue_id);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_log_write_debug(&process,
	                    "[%s]: db last issues put: success (tlgrm userid: %zu, rdmn last issue id: %zu)",
	                    nxs_proc_get_name(&process),
	                    tlgrm_userid,
	                    rdmn_last_issue_id);

error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc != NXS_CHAT_SRV_E_OK) {

		d_ctx->redis_ctx = nxs_chat_srv_d_db_last_issues_redis_ctx_free(d_ctx->redis_ctx);
	}

	return rc;
}

/* Module internal (static) functions */

static redisContext *nxs_chat_srv_d_db_last_issues_redis_ctx_free(redisContext *redis_ctx)
{

	if(redis_ctx != NULL) {

		redisFree(redis_ctx);
	}

	return NULL;
}
