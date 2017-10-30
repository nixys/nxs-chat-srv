#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_QUEUE_C
#define _INCLUDE_NXS_CHAT_SRV_D_DB_QUEUE_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/db-queue/db-queue-typedefs.h>
#include <dal/db-queue/db-queue.h>
#include <dal/db-queue/db-queue-subdals.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_d_db_queue_s
{
	nxs_string_t			redis_prefix;
	nxs_string_t			redis_prefix_meta;
	nxs_string_t			redis_prefix_lock;
	nxs_chat_srv_m_redis_ctx_t	redis_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */

static nxs_string_t _s_answer_ok	= nxs_string("OK");

/* Module global functions */

// clang-format on

nxs_chat_srv_d_db_queue_t *nxs_chat_srv_d_db_queue_init(void)
{
	nxs_chat_srv_d_db_queue_t *d_ctx;

	d_ctx = (nxs_chat_srv_d_db_queue_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_d_db_queue_t));

	nxs_string_init(&d_ctx->redis_prefix);
	nxs_string_init(&d_ctx->redis_prefix_meta);
	nxs_string_init(&d_ctx->redis_prefix_lock);

	nxs_chat_srv_c_redis_get_prefix(&d_ctx->redis_prefix, "nxs-chat-srv:%r:queue");
	nxs_chat_srv_c_redis_get_prefix(&d_ctx->redis_prefix_meta, "nxs-chat-srv:%r:queue:meta");
	nxs_chat_srv_c_redis_get_prefix(&d_ctx->redis_prefix_lock, "nxs-chat-srv:%r:queue:lock");

	/* Connect to Redis */

	if(nxs_chat_srv_c_redis_init(&d_ctx->redis_ctx, &nxs_chat_srv_cfg.redis.nodes, nxs_chat_srv_cfg.redis.is_cluster) !=
	   NXS_CHAT_SRV_E_OK) {

		return nxs_chat_srv_d_db_queue_free(d_ctx);
	}

	return d_ctx;
}

nxs_chat_srv_d_db_queue_t *nxs_chat_srv_d_db_queue_free(nxs_chat_srv_d_db_queue_t *d_ctx)
{

	if(d_ctx == NULL) {

		return NULL;
	}

	nxs_string_free(&d_ctx->redis_prefix);
	nxs_string_free(&d_ctx->redis_prefix_meta);
	nxs_string_free(&d_ctx->redis_prefix_lock);

	nxs_chat_srv_c_redis_free(&d_ctx->redis_ctx);

	return (nxs_chat_srv_d_db_queue_t *)nxs_free(d_ctx);
}

nxs_chat_srv_err_t
        nxs_chat_srv_d_db_queue_add(nxs_chat_srv_d_db_queue_t *d_ctx, size_t tlgrm_userid, time_t inactive_till, nxs_string_t *value)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = nxs_chat_srv_c_redis_command(
	            &d_ctx->redis_ctx, "HSET %s %lu %lu", nxs_string_str(&d_ctx->redis_prefix_meta), tlgrm_userid, inactive_till)) ==
	   NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db queue add error, Redis reply error: %s (tlgrm userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx),
		                    tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if((redis_reply = nxs_chat_srv_c_redis_command(
	            &d_ctx->redis_ctx, "RPUSH %s:%lu %s", nxs_string_str(&d_ctx->redis_prefix), tlgrm_userid, nxs_string_str(value))) ==
	   NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db queue add error, Redis reply error: %s (tlgrm userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx),
		                    tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_log_write_debug(&process, "[%s]: db queue add: success (tlgrm userid: %zu)", nxs_proc_get_name(&process), tlgrm_userid);

error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc == NXS_CHAT_SRV_E_ERR) {

		nxs_chat_srv_c_redis_free(&d_ctx->redis_ctx);
	}

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_queue_get(nxs_chat_srv_d_db_queue_t *d_ctx, size_t tlgrm_userid, nxs_array_t *values)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;
	nxs_string_t *     s;
	size_t             i, len;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = nxs_chat_srv_c_redis_command(
	            &d_ctx->redis_ctx, "HDEL %s %lu", nxs_string_str(&d_ctx->redis_prefix_meta), tlgrm_userid)) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db queue get error, Redis reply error: %s (stage: del value from meta, tlgrm user id: %zu)",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx),
		                    tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if((redis_reply = nxs_chat_srv_c_redis_command(
	            &d_ctx->redis_ctx, "LLEN %s:%lu", nxs_string_str(&d_ctx->redis_prefix), tlgrm_userid)) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db queue get error, Redis reply error: %s (stage: get queue len, tlgrm user id: %zu)",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx),
		                    tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(redis_reply->type == REDIS_REPLY_INTEGER) {

		nxs_log_write_debug(
		        &process, "[%s]: db queue get: success (tlgrm user id: %zu)", nxs_proc_get_name(&process), tlgrm_userid);

		len = redis_reply->integer;
	}
	else {

		nxs_log_write_error(&process,
		                    "[%s]: db queue get error: unexpected Redis reply type (tlgrm user id: %zu, expected type: %d, "
		                    "received type: %d)",
		                    nxs_proc_get_name(&process),
		                    tlgrm_userid,
		                    REDIS_REPLY_INTEGER,
		                    redis_reply->type);

		nxs_error(rc, NXS_CHAT_SRV_E_WARN, error);
	}

	if((redis_reply = nxs_chat_srv_c_redis_command(
	            &d_ctx->redis_ctx, "LRANGE %s:%lu 0 %lu", nxs_string_str(&d_ctx->redis_prefix), tlgrm_userid, len)) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db queue get error, Redis reply error: %s (stage: get queue range, tlgrm user id: %zu)",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx),
		                    tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	switch(redis_reply->type) {

		case REDIS_REPLY_ARRAY:

			nxs_log_write_debug(
			        &process, "[%s]: db queue get: success (tlgrm user id: %zu)", nxs_proc_get_name(&process), tlgrm_userid);

			for(i = 0; i < redis_reply->elements; i++) {

				s = nxs_array_add(values);

				nxs_string_init(s);

				nxs_string_char_ncpy(s, 0, (u_char *)redis_reply->element[i]->str, (size_t)redis_reply->element[i]->len);
			}

			break;

		case REDIS_REPLY_NIL:

			/* value not found by specified key */

			nxs_log_write_debug(&process,
			                    "[%s]: db queue get: value does not exist (tlgrm user id: %zu)",
			                    nxs_proc_get_name(&process),
			                    tlgrm_userid);

			nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);

		default:

			nxs_log_write_error(&process,
			                    "[%s]: db queue get error: unexpected Redis reply type (tlgrm user id: %zu, expected type: %d, "
			                    "received type: %d)",
			                    nxs_proc_get_name(&process),
			                    tlgrm_userid,
			                    REDIS_REPLY_ARRAY,
			                    redis_reply->type);

			nxs_error(rc, NXS_CHAT_SRV_E_WARN, error);
	}

	if((redis_reply = nxs_chat_srv_c_redis_command(
	            &d_ctx->redis_ctx, "LTRIM %s:%lu %lu -1", nxs_string_str(&d_ctx->redis_prefix), tlgrm_userid, len)) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db queue get error, Redis reply error: %s (stage: trim queue, tlgrm user id: %zu)",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx),
		                    tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc == NXS_CHAT_SRV_E_ERR) {

		nxs_chat_srv_c_redis_free(&d_ctx->redis_ctx);
	}

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_queue_lock_set(nxs_chat_srv_d_db_queue_t *d_ctx, size_t tlgrm_userid, time_t expire)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = nxs_chat_srv_c_redis_command(
	            &d_ctx->redis_ctx, "SET %s:%lu locked NX PX %lu", nxs_string_str(&d_ctx->redis_prefix_lock), tlgrm_userid, expire)) ==
	   NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db queue set lock error, Redis reply error: %s (tlgrm userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx),
		                    tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	switch(redis_reply->type) {

		case REDIS_REPLY_STATUS:

			if(nxs_string_char_ncmp(&_s_answer_ok, 0, (u_char *)redis_reply->str, (size_t)redis_reply->len) != NXS_YES) {

				nxs_log_write_error(&process,
				                    "[%s]: db queue set lock error: wrong string answer (tlgrm userid: %zu)",
				                    nxs_proc_get_name(&process),
				                    tlgrm_userid);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			nxs_log_write_debug(&process,
			                    "[%s]: db queue set lock: success (tlgrm user id: %zu, lock expire ms: %zu)",
			                    nxs_proc_get_name(&process),
			                    tlgrm_userid,
			                    expire);

			break;

		case REDIS_REPLY_NIL:

			/* key is already locked by another process */

			nxs_log_write_debug(&process,
			                    "[%s]: db queue set lock: key is already locked by another process (tlgrm user id: %zu)",
			                    nxs_proc_get_name(&process),
			                    tlgrm_userid);

			nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);

		default:

			nxs_log_write_error(
			        &process,
			        "[%s]: db queue set lock error: unexpected Redis reply type (tlgrm user id: %zu, expected type: %d or %d, "
			        "received type: %d)",
			        nxs_proc_get_name(&process),
			        tlgrm_userid,
			        REDIS_REPLY_STATUS,
			        REDIS_REPLY_NIL,
			        redis_reply->type);

			nxs_error(rc, NXS_CHAT_SRV_E_WARN, error);
	}

error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc == NXS_CHAT_SRV_E_ERR) {

		nxs_chat_srv_c_redis_free(&d_ctx->redis_ctx);
	}

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_queue_lock_update(nxs_chat_srv_d_db_queue_t *d_ctx, size_t tlgrm_userid, time_t expire)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = nxs_chat_srv_c_redis_command(
	            &d_ctx->redis_ctx, "SET %s:%lu locked PX %lu", nxs_string_str(&d_ctx->redis_prefix_lock), tlgrm_userid, expire)) ==
	   NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db queue update lock error, Redis reply error: %s (tlgrm userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx),
		                    tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_log_write_debug(&process,
	                    "[%s]: db queue update lock: success (tlgrm user id: %zu, lock expire ms: %zu)",
	                    nxs_proc_get_name(&process),
	                    tlgrm_userid,
	                    expire);
error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc == NXS_CHAT_SRV_E_ERR) {

		nxs_chat_srv_c_redis_free(&d_ctx->redis_ctx);
	}

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_queue_lock_del(nxs_chat_srv_d_db_queue_t *d_ctx, size_t tlgrm_userid)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = nxs_chat_srv_c_redis_command(
	            &d_ctx->redis_ctx, "DEL %s:%lu", nxs_string_str(&d_ctx->redis_prefix_lock), tlgrm_userid)) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db queue delete lock error, Redis reply error: %s (tlgrm userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx),
		                    tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc == NXS_CHAT_SRV_E_ERR) {

		nxs_chat_srv_c_redis_free(&d_ctx->redis_ctx);
	}

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_queue_metas_get(nxs_chat_srv_d_db_queue_t *d_ctx, nxs_array_t *metas)
{
	redisReply *                    redis_reply;
	nxs_chat_srv_err_t              rc;
	nxs_chat_srv_d_db_queue_meta_t *meta;
	size_t                          i;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = nxs_chat_srv_c_redis_command(&d_ctx->redis_ctx, "HGETALL %s", nxs_string_str(&d_ctx->redis_prefix_meta))) ==
	   NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db queue get metas error, Redis reply error: %s",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx));

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	switch(redis_reply->type) {

		case REDIS_REPLY_ARRAY:

			for(i = 0; i < redis_reply->elements; i += 2) {

				meta = nxs_array_add(metas);

				meta->tlgrm_userid  = atoi(redis_reply->element[i]->str);
				meta->inactive_till = atoi(redis_reply->element[i + 1]->str);
			}

			break;

		case REDIS_REPLY_NIL:

			/* value not found by specified key */

			nxs_log_write_debug(&process, "[%s]: db queue get metas: value does not exist", nxs_proc_get_name(&process));

			nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);

		default:

			nxs_log_write_error(&process,
			                    "[%s]: db queue get metas error: unexpected Redis reply type (expected type: %d, "
			                    "received type: %d)",
			                    nxs_proc_get_name(&process),
			                    REDIS_REPLY_ARRAY,
			                    redis_reply->type);

			nxs_error(rc, NXS_CHAT_SRV_E_WARN, error);
	}

error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc == NXS_CHAT_SRV_E_ERR) {

		nxs_chat_srv_c_redis_free(&d_ctx->redis_ctx);
	}

	return rc;
}

/* Module internal (static) functions */
