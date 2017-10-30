#ifndef _INCLUDE_NXS_CHAT_SRV_D_RA_QUEUE_C
#define _INCLUDE_NXS_CHAT_SRV_D_RA_QUEUE_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/ra-queue/ra-queue-typedefs.h>
#include <dal/ra-queue/ra-queue.h>
#include <dal/ra-queue/ra-queue-subdals.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_d_ra_queue_s
{
	nxs_string_t			redis_prefix;
	nxs_chat_srv_m_redis_ctx_t	redis_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_d_ra_queue_t *nxs_chat_srv_d_ra_queue_init(void)
{
	nxs_chat_srv_d_ra_queue_t *d_ctx;

	d_ctx = (nxs_chat_srv_d_ra_queue_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_d_ra_queue_t));

	nxs_string_init(&d_ctx->redis_prefix);

	nxs_chat_srv_c_redis_get_prefix(&d_ctx->redis_prefix, "nxs-chat-srv:%r:ra-queue");

	/* Connect to Redis */

	if(nxs_chat_srv_c_redis_init(&d_ctx->redis_ctx, &nxs_chat_srv_cfg.redis.nodes, nxs_chat_srv_cfg.redis.is_cluster) !=
	   NXS_CHAT_SRV_E_OK) {

		return nxs_chat_srv_d_ra_queue_free(d_ctx);
	}

	return d_ctx;
}

nxs_chat_srv_d_ra_queue_t *nxs_chat_srv_d_ra_queue_free(nxs_chat_srv_d_ra_queue_t *d_ctx)
{

	if(d_ctx == NULL) {

		return NULL;
	}

	nxs_string_free(&d_ctx->redis_prefix);

	nxs_chat_srv_c_redis_free(&d_ctx->redis_ctx);

	return (nxs_chat_srv_d_ra_queue_t *)nxs_free(d_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_d_ra_queue_add(nxs_chat_srv_d_ra_queue_t *d_ctx, nxs_string_t *value)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL || value == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = nxs_chat_srv_c_redis_command(
	            &d_ctx->redis_ctx, "RPUSH %s %s", nxs_string_str(&d_ctx->redis_prefix), nxs_string_str(value))) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: ra queue add error, Redis reply error: %s",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx));

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_log_write_debug(&process, "[%s]: ra queue add: success", nxs_proc_get_name(&process));

error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc == NXS_CHAT_SRV_E_ERR) {

		nxs_chat_srv_c_redis_free(&d_ctx->redis_ctx);
	}

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_ra_queue_get(nxs_chat_srv_d_ra_queue_t *d_ctx, nxs_string_t *value)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL || value == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = nxs_chat_srv_c_redis_command(&d_ctx->redis_ctx, "LPOP %s", nxs_string_str(&d_ctx->redis_prefix))) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: ra queue get error, Redis reply error: %s",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx));

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	switch(redis_reply->type) {

		case REDIS_REPLY_STRING:

			/*
			 * If used "BLPOP" command:
			 * <pre>
			 * nxs_string_char_ncpy(value, 0, (u_char *)redis_reply->element[1]->str, (size_t)redis_reply->element[1]->len);
			 * </pre>
			 */

			nxs_string_char_ncpy(value, 0, (u_char *)redis_reply->str, (size_t)redis_reply->len);

			nxs_log_write_debug(&process, "[%s]: ra queue get: success", nxs_proc_get_name(&process));

			break;

		case REDIS_REPLY_NIL:

			/* timeout */

			nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);

		default:

			nxs_log_write_error(&process,
			                    "[%s]: ra queue get error: unexpected Redis reply type (expected type: %d, received type: %d)",
			                    nxs_proc_get_name(&process),
			                    REDIS_REPLY_STRING,
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
