#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_WAITINGS_C
#define _INCLUDE_NXS_CHAT_SRV_D_DB_WAITINGS_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/db-waitings/db-waitings-typedefs.h>
#include <dal/db-waitings/db-waitings.h>
#include <dal/db-waitings/db-waitings-subdals.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	size_t				tlgrm_userid;
	nxs_string_t			update;
} nxs_chat_srv_d_db_waitings_el_t;

struct nxs_chat_srv_d_db_waitings_s
{
	nxs_array_t			waitings;		/* type: nxs_chat_srv_d_db_waitings_el_t */
	nxs_string_t			redis_prefix;
	nxs_chat_srv_m_redis_ctx_t	redis_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

static void nxs_chat_srv_d_db_waitings_el_init(void *element);
static void nxs_chat_srv_d_db_waitings_el_free(void *element);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_d_db_waitings_t *nxs_chat_srv_d_db_waitings_init(void)
{
	nxs_chat_srv_d_db_waitings_t *d_ctx;

	d_ctx = (nxs_chat_srv_d_db_waitings_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_d_db_waitings_t));

	nxs_string_init(&d_ctx->redis_prefix);

	nxs_chat_srv_c_redis_get_prefix(&d_ctx->redis_prefix, "nxs-chat-srv:%r:waitings");

	nxs_array_init3(&d_ctx->waitings,
	                nxs_chat_srv_d_db_waitings_el_t,
	                &nxs_chat_srv_d_db_waitings_el_init,
	                &nxs_chat_srv_d_db_waitings_el_free);

	if(nxs_chat_srv_c_redis_init(&d_ctx->redis_ctx, &nxs_chat_srv_cfg.redis.nodes, nxs_chat_srv_cfg.redis.is_cluster) !=
	   NXS_CHAT_SRV_E_OK) {

		return nxs_chat_srv_d_db_waitings_free(d_ctx);
	}

	return d_ctx;
}

nxs_chat_srv_d_db_waitings_t *nxs_chat_srv_d_db_waitings_free(nxs_chat_srv_d_db_waitings_t *d_ctx)
{

	if(d_ctx == NULL) {

		return NULL;
	}

	nxs_string_free(&d_ctx->redis_prefix);

	nxs_array_free(&d_ctx->waitings);

	nxs_chat_srv_c_redis_free(&d_ctx->redis_ctx);

	return (nxs_chat_srv_d_db_waitings_t *)nxs_free(d_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_waitings_pull(nxs_chat_srv_d_db_waitings_t *d_ctx)
{
	redisReply *                     redis_reply;
	nxs_chat_srv_err_t               rc;
	nxs_chat_srv_d_db_waitings_el_t *el;
	size_t                           i;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = nxs_chat_srv_c_redis_command(&d_ctx->redis_ctx, "HGETALL %s", nxs_string_str(&d_ctx->redis_prefix))) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db waitings get error, Redis reply error: %s",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx));

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	switch(redis_reply->type) {

		case REDIS_REPLY_ARRAY:

			nxs_log_write_debug(&process, "[%s]: db waitings get: success", nxs_proc_get_name(&process));

			for(i = 0; i < redis_reply->elements; i += 2) {

				el = nxs_array_add(&d_ctx->waitings);

				el->tlgrm_userid = atoi(redis_reply->element[i]->str);

				nxs_string_char_ncpy(&el->update,
				                     0,
				                     (u_char *)redis_reply->element[i + 1]->str,
				                     (size_t)redis_reply->element[i + 1]->len);
			}

			break;

		case REDIS_REPLY_NIL:

			/* value not found by specified key */

			nxs_log_write_debug(&process, "[%s]: db waitings get: no values exists", nxs_proc_get_name(&process));

			nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);

		default:

			nxs_log_write_error(
			        &process,
			        "[%s]: db waitings get error: unexpected Redis reply type (expected type: %d, received type: %d)",
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

nxs_chat_srv_err_t nxs_chat_srv_d_db_waitings_set(nxs_chat_srv_d_db_waitings_t *d_ctx, size_t tlgrm_userid, nxs_string_t *update)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL || update == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = nxs_chat_srv_c_redis_command(
	            &d_ctx->redis_ctx, "HSET %s %lu %s", nxs_string_str(&d_ctx->redis_prefix), tlgrm_userid, nxs_string_str(update))) ==
	   NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db waitings set error, Redis reply error: %s (tlgrm userid: %zu)",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx),
		                    tlgrm_userid);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_log_write_debug(&process, "[%s]: db waitings set: success (tlgrm userid: %zu)", nxs_proc_get_name(&process), tlgrm_userid);

error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc == NXS_CHAT_SRV_E_ERR) {

		nxs_chat_srv_c_redis_free(&d_ctx->redis_ctx);
	}

	return rc;
}

nxs_string_t *nxs_chat_srv_d_db_waitings_get(nxs_chat_srv_d_db_waitings_t *d_ctx, size_t i, size_t *tlgrm_userid)
{
	nxs_chat_srv_d_db_waitings_el_t *el;

	if(d_ctx == NULL) {

		return NULL;
	}

	if((el = nxs_array_get(&d_ctx->waitings, i)) == NULL) {

		return NULL;
	}

	if(tlgrm_userid != NULL) {

		*tlgrm_userid = el->tlgrm_userid;
	}

	return &el->update;
}

/*
 * Specified element deletes only from from set, but not from context array
 */
nxs_chat_srv_err_t nxs_chat_srv_d_db_waitings_del(nxs_chat_srv_d_db_waitings_t *d_ctx, size_t tlgrm_userid)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = nxs_chat_srv_c_redis_command(
	            &d_ctx->redis_ctx, "HDEL %s %lu", nxs_string_str(&d_ctx->redis_prefix), tlgrm_userid)) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db waitings delete, Redis reply error: %s (tlgrm userid: %zu)",
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

/* Module internal (static) functions */

static void nxs_chat_srv_d_db_waitings_el_init(void *element)
{
	nxs_chat_srv_d_db_waitings_el_t *el = element;

	el->tlgrm_userid = 0;

	nxs_string_init(&el->update);
}

static void nxs_chat_srv_d_db_waitings_el_free(void *element)
{
	nxs_chat_srv_d_db_waitings_el_t *el = element;

	el->tlgrm_userid = 0;

	nxs_string_free(&el->update);
}
