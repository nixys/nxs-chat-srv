#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_PRESALE_C
#define _INCLUDE_NXS_CHAT_SRV_D_DB_PRESALE_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/db-presale/db-presale-typedefs.h>
#include <dal/db-presale/db-presale.h>
#include <dal/db-presale/db-presale-subdals.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	size_t				tlgrm_userid;
	size_t				rdmn_issue_id;
} nxs_chat_srv_d_db_presale_el_t;

struct nxs_chat_srv_d_db_presale_s
{
	nxs_array_t			presales;		/* type: nxs_chat_srv_d_db_presale_el_t */
	nxs_string_t			redis_prefix;
	nxs_chat_srv_m_redis_ctx_t	redis_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

static void nxs_chat_srv_d_db_presale_el_init(void *element);
static void nxs_chat_srv_d_db_presale_el_free(void *element);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_d_db_presale_t *nxs_chat_srv_d_db_presale_init(void)
{
	nxs_chat_srv_d_db_presale_t *d_ctx;

	d_ctx = (nxs_chat_srv_d_db_presale_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_d_db_presale_t));

	nxs_string_init(&d_ctx->redis_prefix);

	nxs_chat_srv_c_redis_get_prefix(&d_ctx->redis_prefix, "nxs-chat-srv:%r:presale");

	nxs_array_init3(
	        &d_ctx->presales, nxs_chat_srv_d_db_presale_el_t, &nxs_chat_srv_d_db_presale_el_init, &nxs_chat_srv_d_db_presale_el_free);

	if(nxs_chat_srv_c_redis_init(&d_ctx->redis_ctx, &nxs_chat_srv_cfg.redis.nodes, nxs_chat_srv_cfg.redis.is_cluster) !=
	   NXS_CHAT_SRV_E_OK) {

		return nxs_chat_srv_d_db_presale_free(d_ctx);
	}

	return d_ctx;
}

nxs_chat_srv_d_db_presale_t *nxs_chat_srv_d_db_presale_free(nxs_chat_srv_d_db_presale_t *d_ctx)
{

	if(d_ctx == NULL) {

		return NULL;
	}

	nxs_string_free(&d_ctx->redis_prefix);

	nxs_array_free(&d_ctx->presales);

	nxs_chat_srv_c_redis_free(&d_ctx->redis_ctx);

	return (nxs_chat_srv_d_db_presale_t *)nxs_free(d_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_presale_pull(nxs_chat_srv_d_db_presale_t *d_ctx)
{
	redisReply *                    redis_reply;
	nxs_chat_srv_err_t              rc;
	nxs_chat_srv_d_db_presale_el_t *el;
	size_t                          i;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = nxs_chat_srv_c_redis_command(&d_ctx->redis_ctx, "HGETALL %s", nxs_string_str(&d_ctx->redis_prefix))) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db presale get error, Redis reply error: %s",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx));

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	switch(redis_reply->type) {

		case REDIS_REPLY_ARRAY:

			nxs_log_write_debug(&process, "[%s]: db presale get: success", nxs_proc_get_name(&process));

			for(i = 0; i < redis_reply->elements; i += 2) {

				el = nxs_array_add(&d_ctx->presales);

				el->tlgrm_userid  = atoi(redis_reply->element[i]->str);
				el->rdmn_issue_id = atoi(redis_reply->element[i + 1]->str);
			}

			break;

		case REDIS_REPLY_NIL:

			/* value not found by specified key */

			nxs_log_write_debug(&process, "[%s]: db presale get: no values exists", nxs_proc_get_name(&process));

			nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);

		default:

			nxs_log_write_error(
			        &process,
			        "[%s]: db presale get error: unexpected Redis reply type (expected type: %d, received type: %d)",
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

nxs_chat_srv_err_t nxs_chat_srv_d_db_presale_set(nxs_chat_srv_d_db_presale_t *d_ctx, size_t tlgrm_userid, size_t rdmn_issue_id)
{
	redisReply *       redis_reply;
	nxs_chat_srv_err_t rc;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((redis_reply = nxs_chat_srv_c_redis_command(
	            &d_ctx->redis_ctx, "HSET %s %lu %lu", nxs_string_str(&d_ctx->redis_prefix), tlgrm_userid, rdmn_issue_id)) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db presale set error, Redis reply error: %s (tlgrm userid: %zu, rdmn issue id: %zu)",
		                    nxs_proc_get_name(&process),
		                    nxs_chat_srv_c_redis_get_error(&d_ctx->redis_ctx),
		                    tlgrm_userid,
		                    rdmn_issue_id);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_log_write_debug(&process,
	                    "[%s]: db presale set: success (tlgrm userid: %zu, rdmn issue id: %zu)",
	                    nxs_proc_get_name(&process),
	                    tlgrm_userid,
	                    rdmn_issue_id);

error:

	if(redis_reply != NULL) {

		freeReplyObject(redis_reply);
	}

	if(rc == NXS_CHAT_SRV_E_ERR) {

		nxs_chat_srv_c_redis_free(&d_ctx->redis_ctx);
	}

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_presale_get(nxs_chat_srv_d_db_presale_t *d_ctx, size_t i, size_t *tlgrm_userid, size_t *rdmn_issue_id)
{
	nxs_chat_srv_d_db_presale_el_t *el;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((el = nxs_array_get(&d_ctx->presales, i)) == NULL) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(rdmn_issue_id != NULL) {

		*rdmn_issue_id = el->rdmn_issue_id;
	}

	if(tlgrm_userid != NULL) {

		*tlgrm_userid = el->tlgrm_userid;
	}

	return NXS_CHAT_SRV_E_OK;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_presale_get_by_user_id(nxs_chat_srv_d_db_presale_t *d_ctx, size_t tlgrm_userid, size_t *rdmn_issue_id)
{
	nxs_chat_srv_d_db_presale_el_t *el;
	size_t                          i;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	for(i = 0; i < nxs_array_count(&d_ctx->presales); i++) {

		el = nxs_array_get(&d_ctx->presales, i);

		if(el->tlgrm_userid == tlgrm_userid) {

			if(rdmn_issue_id != NULL) {

				*rdmn_issue_id = el->rdmn_issue_id;
			}

			return NXS_CHAT_SRV_E_OK;
		}
	}

	return NXS_CHAT_SRV_E_EXIST;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_presale_get_by_issue_id(nxs_chat_srv_d_db_presale_t *d_ctx, size_t rdmn_issue_id, size_t *tlgrm_userid)
{
	nxs_chat_srv_d_db_presale_el_t *el;
	size_t                          i;

	if(d_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	for(i = 0; i < nxs_array_count(&d_ctx->presales); i++) {

		el = nxs_array_get(&d_ctx->presales, i);

		if(el->rdmn_issue_id == rdmn_issue_id) {

			if(tlgrm_userid != NULL) {

				*tlgrm_userid = el->tlgrm_userid;
			}

			return NXS_CHAT_SRV_E_OK;
		}
	}

	return NXS_CHAT_SRV_E_EXIST;
}

/*
 * Specified element deletes only from from set, but not from context array
 */
nxs_chat_srv_err_t nxs_chat_srv_d_db_presale_del(nxs_chat_srv_d_db_presale_t *d_ctx, size_t tlgrm_userid)
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
		                    "[%s]: db presale delete, Redis reply error: %s (tlgrm userid: %zu)",
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

static void nxs_chat_srv_d_db_presale_el_init(void *element)
{
	nxs_chat_srv_d_db_presale_el_t *el = element;

	el->tlgrm_userid  = 0;
	el->rdmn_issue_id = 0;
}

static void nxs_chat_srv_d_db_presale_el_free(void *element)
{
	nxs_chat_srv_d_db_presale_el_t *el = element;

	el->tlgrm_userid  = 0;
	el->rdmn_issue_id = 0;
}
