#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_STATISTIC_C
#define _INCLUDE_NXS_CHAT_SRV_D_DB_STATISTIC_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/db-statistic/db-statistic-typedefs.h>
#include <dal/db-statistic/db-statistic.h>
#include <dal/db-statistic/db-statistic-subdals.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_d_db_statistic_s
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

nxs_chat_srv_d_db_statistic_t *nxs_chat_srv_d_db_statistic_init(void)
{
	nxs_chat_srv_d_db_statistic_t *d_ctx;

	d_ctx = (nxs_chat_srv_d_db_statistic_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_d_db_statistic_t));

	nxs_string_init(&d_ctx->redis_prefix);

	nxs_chat_srv_c_redis_get_prefix(&d_ctx->redis_prefix, "nxs-chat-srv:%r:statistic");

	/* Connect to Redis */

	if(nxs_chat_srv_c_redis_init(&d_ctx->redis_ctx, &nxs_chat_srv_cfg.redis.nodes, nxs_chat_srv_cfg.redis.is_cluster) !=
	   NXS_CHAT_SRV_E_OK) {

		return nxs_chat_srv_d_db_statistic_free(d_ctx);
	}

	return d_ctx;
}

nxs_chat_srv_d_db_statistic_t *nxs_chat_srv_d_db_statistic_free(nxs_chat_srv_d_db_statistic_t *d_ctx)
{

	if(d_ctx == NULL) {

		return NULL;
	}

	nxs_string_free(&d_ctx->redis_prefix);

	nxs_chat_srv_c_redis_free(&d_ctx->redis_ctx);

	return (nxs_chat_srv_d_db_statistic_t *)nxs_free(d_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_statistic_add(nxs_chat_srv_d_db_statistic_t *d_ctx, nxs_string_t *action)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL || action == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = nxs_chat_srv_c_redis_command(
	            &d_ctx->redis_ctx, "RPUSH %s %s", nxs_string_str(&d_ctx->redis_prefix), nxs_string_str(action))) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db statistic add error, Redis reply error: %s",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx));

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_log_write_debug(&process, "[%s]: db statistic add: success", nxs_proc_get_name(&process));

error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc == NXS_CHAT_SRV_E_ERR) {

		nxs_chat_srv_c_redis_free(&d_ctx->redis_ctx);
	}

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_statistic_get(nxs_chat_srv_d_db_statistic_t *d_ctx, nxs_array_t *actions)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;
	nxs_string_t *     s;
	size_t             i;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = nxs_chat_srv_c_redis_command(&d_ctx->redis_ctx, "LRANGE %s 0 -1", nxs_string_str(&d_ctx->redis_prefix))) ==
	   NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db statistic get error, Redis reply error: %s",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx));

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	switch(redis_reply->type) {

		case REDIS_REPLY_ARRAY:

			nxs_log_write_debug(&process, "[%s]: db statistic get: success", nxs_proc_get_name(&process));

			for(i = 0; i < redis_reply->elements; i++) {

				s = nxs_array_add(actions);

				nxs_string_init(s);

				nxs_string_char_ncpy(s, 0, (u_char *)redis_reply->element[i]->str, (size_t)redis_reply->element[i]->len);
			}

			break;

		case REDIS_REPLY_NIL:

			/* value not found by specified key */

			nxs_log_write_debug(&process, "[%s]: db statistic get: value does not exist)", nxs_proc_get_name(&process));

			nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);

		default:

			nxs_log_write_error(&process,
			                    "[%s]: db statistic get error: unexpected Redis reply type (expected type: %d, "
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
