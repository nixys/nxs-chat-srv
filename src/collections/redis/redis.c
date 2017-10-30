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

nxs_chat_srv_err_t nxs_chat_srv_c_redis_init(nxs_chat_srv_m_redis_ctx_t *redis_ctx, nxs_array_t *nodes, nxs_bool_t is_cluster)
{
	size_t                         i;
	nxs_chat_srv_cfg_redis_node_t *node;
	nxs_string_t                   nodes_str;
	nxs_chat_srv_err_t             rc;

	if(redis_ctx == NULL || nodes == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_array_count(nodes) == 0) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&nodes_str);

	redis_ctx->is_cluster        = is_cluster;
	redis_ctx->redis_ctx         = NULL;
	redis_ctx->redis_cluster_ctx = NULL;

	if(redis_ctx->is_cluster == NXS_YES) {

		for(i = 0; i < nxs_array_count(nodes); i++) {

			if(i > 0) {

				nxs_string_char_add_char(&nodes_str, (u_char)',');
			}

			node = nxs_array_get(nodes, i);

			nxs_string_printf2_cat(&nodes_str, "%r:%d", &node->host, node->port);
		}

		if((redis_ctx->redis_cluster_ctx = redisClusterContextInit()) == NULL) {

			nxs_log_write_error(&process,
			                    "[%s]: can't connect to Redis (cluster): can't allocate Redis context",
			                    nxs_proc_get_name(&process));

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		redisClusterSetOptionAddNodes(redis_ctx->redis_cluster_ctx, (char *)nxs_string_str(&nodes_str));

		redisClusterConnect2(redis_ctx->redis_cluster_ctx);

		if(redis_ctx->redis_cluster_ctx == NULL || redis_ctx->redis_cluster_ctx->err != 0) {

			/* Redis connection in error state */

			if(redis_ctx->redis_cluster_ctx != NULL) {

				nxs_log_write_error(&process,
				                    "[%s]: can't connect to Redis (cluster): %s",
				                    nxs_proc_get_name(&process),
				                    redis_ctx->redis_cluster_ctx->errstr);
			}
			else {

				nxs_log_write_error(&process,
				                    "[%s]: can't connect to Redis (cluster): can't allocate Redis context",
				                    nxs_proc_get_name(&process));
			}

			nxs_chat_srv_c_redis_free(redis_ctx);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}
	else {

		node = nxs_array_get(nodes, 0);

		redis_ctx->redis_ctx = redisConnect((char *)nxs_string_str(&node->host), node->port);

		if(redis_ctx->redis_ctx == NULL || redis_ctx->redis_ctx->err != 0) {

			/* Redis connection in error state */

			if(redis_ctx->redis_ctx != NULL) {

				nxs_log_write_error(&process,
				                    "[%s]: can't connect to Redis (standalone): %s",
				                    nxs_proc_get_name(&process),
				                    redis_ctx->redis_ctx->errstr);
			}
			else {

				nxs_log_write_error(&process,
				                    "[%s]: can't connect to Redis (standalone): can't allocate Redis context",
				                    nxs_proc_get_name(&process));
			}

			nxs_chat_srv_c_redis_free(redis_ctx);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}

error:

	nxs_string_free(&nodes_str);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_c_redis_free(nxs_chat_srv_m_redis_ctx_t *redis_ctx)
{

	if(redis_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	redis_ctx->is_cluster = NXS_NO;

	if(redis_ctx->redis_ctx != NULL) {

		redisFree(redis_ctx->redis_ctx);

		redis_ctx->redis_ctx = NULL;
	}

	if(redis_ctx->redis_cluster_ctx != NULL) {

		redisClusterFree(redis_ctx->redis_cluster_ctx);

		redis_ctx->redis_cluster_ctx = NULL;
	}

	return NXS_CHAT_SRV_E_OK;
}

redisReply *nxs_chat_srv_c_redis_command(nxs_chat_srv_m_redis_ctx_t *redis_ctx, const char *query, ...)
{
	nxs_string_t q;
	va_list      arg;
	redisReply * redis_reply;

	if(redis_ctx == NULL || query == NULL) {

		return NULL;
	}

	nxs_string_init(&q);

	va_start(arg, query);
	nxs_string_vprintf(&q, query, arg);
	va_end(arg);

	if(redis_ctx->is_cluster == NXS_YES) {

		if(redis_ctx->redis_cluster_ctx == NULL) {

			nxs_log_write_error(&process,
			                    "[%s]: can't execute Redis (cluster) command: context is NULL (command: \"%r\")",
			                    nxs_proc_get_name(&process),
			                    &q);

			redis_reply = NULL;
		}
		else {

			redis_reply = redisClusterCommand(redis_ctx->redis_cluster_ctx, (char *)nxs_string_str(&q));
		}
	}
	else {

		if(redis_ctx->redis_ctx == NULL) {

			nxs_log_write_error(&process,
			                    "[%s]: can't execute Redis (standalone) command: context is NULL (command: \"%r\")",
			                    nxs_proc_get_name(&process),
			                    &q);

			redis_reply = NULL;
		}
		else {

			redis_reply = redisCommand(redis_ctx->redis_ctx, (char *)nxs_string_str(&q));
		}
	}

	if(redis_reply != NULL && redis_reply->type == REDIS_REPLY_ERROR) {

		nxs_log_write_error(&process,
		                    "[%s]: error while execution Redis (%s) command: %s (command: \"%r\")",
		                    nxs_proc_get_name(&process),
		                    redis_ctx->is_cluster == NXS_YES ? "cluster" : "standalone",
		                    redis_reply->str != NULL ? redis_reply->str : "NULL",
		                    &q);

		freeReplyObject(redis_reply);

		redis_reply = NULL;
	}

	nxs_string_free(&q);

	return redis_reply;
}

char *nxs_chat_srv_c_redis_get_error(nxs_chat_srv_m_redis_ctx_t *redis_ctx)
{

	if(redis_ctx == NULL) {

		return "NULL";
	}

	if(redis_ctx->is_cluster == NXS_YES) {

		if(redis_ctx->redis_cluster_ctx != NULL) {

			return redis_ctx->redis_cluster_ctx->errstr;
		}
	}
	else {

		if(redis_ctx->redis_ctx != NULL) {

			return redis_ctx->redis_ctx->errstr;
		}
	}

	return "NULL";
}

void nxs_chat_srv_c_redis_get_prefix(nxs_string_t *prefix_dst, char *prefix_fmt)
{

	if(prefix_dst == NULL || prefix_fmt == NULL) {

		return;
	}

	nxs_string_printf(prefix_dst, prefix_fmt, &nxs_chat_srv_cfg.redis.keys_space);
}

/* Module internal (static) functions */
