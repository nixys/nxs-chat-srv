#ifndef _INCLUDE_NXS_CHAT_SRV_U_DB_QUEUE_C
#define _INCLUDE_NXS_CHAT_SRV_U_DB_QUEUE_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/db-queue/db-queue-typedefs.h>
#include <units/db-queue/db-queue.h>
#include <units/db-queue/db-queue-subunits.h>

/* Definitions */

#define NXS_CHAT_SRV_U_DB_QUEUE_LOCK_EXPIRE			30000

#define NXS_CHAT_SRV_U_DB_QUEUE_INACTIVE_INTERVAL_MESSAGE	1
#define NXS_CHAT_SRV_U_DB_QUEUE_INACTIVE_INTERVAL_CALLBACK	0
#define NXS_CHAT_SRV_U_DB_QUEUE_INACTIVE_INTERVAL_COMMAND	0

/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_u_db_queue_s
{
	size_t				tlgrm_userid;
	nxs_chat_srv_d_db_queue_t	*db_queue_ctx;

	nxs_list_t			updates;		/* type: nxs_chat_srv_m_tlgrm_update_t */
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_u_db_queue_t *nxs_chat_srv_u_db_queue_init(void)
{
	nxs_chat_srv_u_db_queue_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_db_queue_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_db_queue_t));

	u_ctx->tlgrm_userid = 0;

	nxs_list_init2(&u_ctx->updates, nxs_chat_srv_m_tlgrm_update_t);

	u_ctx->db_queue_ctx = nxs_chat_srv_d_db_queue_init();

	return u_ctx;
}

nxs_chat_srv_u_db_queue_t *nxs_chat_srv_u_db_queue_free(nxs_chat_srv_u_db_queue_t *u_ctx)
{
	nxs_chat_srv_m_tlgrm_update_t *u;

	if(u_ctx == NULL) {

		return NULL;
	}

	nxs_chat_srv_u_db_queue_unlock(u_ctx);

	for(u = nxs_list_ptr_init(&u_ctx->updates, NXS_LIST_PTR_INIT_HEAD); u != NULL;
	    u = nxs_list_del(&u_ctx->updates, NXS_LIST_MOVE_NEXT)) {

		nxs_chat_srv_c_tlgrm_update_free(u);
	}

	nxs_list_free(&u_ctx->updates);

	u_ctx->db_queue_ctx = nxs_chat_srv_d_db_queue_free(u_ctx->db_queue_ctx);

	return (nxs_chat_srv_u_db_queue_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_queue_add(nxs_chat_srv_u_db_queue_t *        u_ctx,
                                               size_t                             tlgrm_userid,
                                               nxs_chat_srv_m_tlgrm_update_type_t update_type,
                                               nxs_string_t *                     update_encoded)
{
	time_t inactive_till;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	switch(update_type) {

		case NXS_CHAT_SRV_M_TLGRM_UPDATE_TYPE_MESSAGE:

			inactive_till = time(NULL) + NXS_CHAT_SRV_U_DB_QUEUE_INACTIVE_INTERVAL_MESSAGE;

			break;

		case NXS_CHAT_SRV_M_TLGRM_UPDATE_TYPE_CALLBACK:

			inactive_till = time(NULL) + NXS_CHAT_SRV_U_DB_QUEUE_INACTIVE_INTERVAL_CALLBACK;

			break;

		case NXS_CHAT_SRV_M_TLGRM_UPDATE_TYPE_COMMAND:

			inactive_till = time(NULL) + NXS_CHAT_SRV_U_DB_QUEUE_INACTIVE_INTERVAL_COMMAND;

			break;

		default:

			return NXS_CHAT_SRV_E_TYPE;
	}

	return nxs_chat_srv_d_db_queue_add(u_ctx->db_queue_ctx, tlgrm_userid, inactive_till, update_encoded);
}

/*
 * Returns:
 * * NXS_CHAT_SRV_E_OK		- got queue
 * * NXS_CHAT_SRV_E_EXIST	- no suitable queue locks
 * * NXS_CHAT_SRV_E_ERR		- runtime errors
 */
nxs_chat_srv_err_t nxs_chat_srv_u_db_queue_get(nxs_chat_srv_u_db_queue_t *u_ctx)
{
	nxs_array_t                     metas, values;
	nxs_chat_srv_d_db_queue_meta_t *m;
	nxs_chat_srv_m_tlgrm_update_t * u;
	nxs_string_t *                  s, value_decoded;
	nxs_chat_srv_err_t              rc;
	size_t                          i;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	nxs_array_init2(&metas, nxs_chat_srv_d_db_queue_meta_t);
	nxs_array_init2(&values, nxs_string_t);

	nxs_string_init(&value_decoded);

	if((rc = nxs_chat_srv_d_db_queue_metas_get(u_ctx->db_queue_ctx, &metas)) != NXS_CHAT_SRV_E_OK) {

		goto error;
	}

	rc = NXS_CHAT_SRV_E_EXIST;

	for(i = 0; i < nxs_array_count(&metas); i++) {

		m = nxs_array_get(&metas, i);

		if(m->inactive_till <= time(NULL)) {

			/* found suitable element */

			switch(nxs_chat_srv_d_db_queue_lock_set(
			        u_ctx->db_queue_ctx, m->tlgrm_userid, NXS_CHAT_SRV_U_DB_QUEUE_LOCK_EXPIRE)) {

				case NXS_CHAT_SRV_E_OK:

					/* queue locked */

					u_ctx->tlgrm_userid = m->tlgrm_userid;

					switch(nxs_chat_srv_d_db_queue_get(u_ctx->db_queue_ctx, u_ctx->tlgrm_userid, &values)) {

						case NXS_CHAT_SRV_E_OK:

							/* values found in queue */

							for(i = 0; i < nxs_array_count(&values); i++) {

								s = nxs_array_get(&values, i);

								u = nxs_list_add_tail(&u_ctx->updates);

								nxs_chat_srv_c_tlgrm_update_init(u);

								nxs_base64_decode_string(&value_decoded, s);

								if(nxs_chat_srv_c_tlgrm_update_pull_json(u, (nxs_buf_t *)&value_decoded) !=
								   NXS_CHAT_SRV_E_OK) {

									nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
								}
							}

							nxs_list_ptr_init(&u_ctx->updates, NXS_LIST_PTR_INIT_HEAD);

							nxs_error(rc, NXS_CHAT_SRV_E_OK, error);

							break;

						case NXS_CHAT_SRV_E_EXIST:

							/* no values in that queue */

							/* unlock queue */
							if(nxs_chat_srv_u_db_queue_unlock(u_ctx) != NXS_CHAT_SRV_E_OK) {

								nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
							}

							break;

						default:

							/* unlock queue */
							nxs_chat_srv_u_db_queue_unlock(u_ctx);

							nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
					}

					break;

				case NXS_CHAT_SRV_E_EXIST:

					/* queue locked by another worker */

					break;

				default:

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}
		}
	}

error:

	for(i = 0; i < nxs_array_count(&values); i++) {

		s = nxs_array_get(&values, i);

		nxs_string_free(s);
	}

	nxs_array_free(&values);
	nxs_array_free(&metas);

	nxs_string_free(&value_decoded);

	return rc;
}

/* TODO: delete if not necessary */
/*
nxs_chat_srv_err_t nxs_chat_srv_u_db_queue_lock(nxs_chat_srv_u_db_queue_t *u_ctx, size_t tlgrm_userid)
{
        time_t             expired;
        nxs_chat_srv_err_t rc;

        if(u_ctx == NULL) {

                return NXS_CHAT_SRV_E_PTR;
        }

        if(u_ctx->tlgrm_userid != 0) {

                nxs_log_write_warn(&process,
                                   "[%s]: can't lock queue: another queue already locked (trying to lock queue: %zu, locked queue: %zu)",
                                   nxs_proc_get_name(&process),
                                   tlgrm_userid,
                                   u_ctx->tlgrm_userid);

                return NXS_CHAT_SRV_E_EXIST;
        }

        expired = time(NULL) + NXS_CHAT_SRV_U_DB_QUEUE_EXPIRE_INTERVAL;

        if((rc = nxs_chat_srv_d_db_queue_lock_set(&u_ctx->db_queue_ctx, tlgrm_userid, expired))) {

                u_ctx->tlgrm_userid = tlgrm_userid;
        }

        return rc;
}
*/

nxs_chat_srv_err_t nxs_chat_srv_u_db_queue_unlock(nxs_chat_srv_u_db_queue_t *u_ctx)
{
	nxs_chat_srv_err_t rc;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(u_ctx->tlgrm_userid == 0) {

		return NXS_CHAT_SRV_E_OK;
	}

	rc = nxs_chat_srv_d_db_queue_lock_del(u_ctx->db_queue_ctx, u_ctx->tlgrm_userid);

	u_ctx->tlgrm_userid = 0;

	return rc;
}

/* TODO: this function not used at the moment */
nxs_chat_srv_err_t nxs_chat_srv_u_db_queue_lock_update(nxs_chat_srv_u_db_queue_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(u_ctx->tlgrm_userid == 0) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	return nxs_chat_srv_d_db_queue_lock_update(u_ctx->db_queue_ctx, u_ctx->tlgrm_userid, NXS_CHAT_SRV_U_DB_QUEUE_LOCK_EXPIRE);
}

/*
 * Get head element from list
 */
nxs_chat_srv_m_tlgrm_update_t *nxs_chat_srv_u_db_queue_list_get_head(nxs_chat_srv_u_db_queue_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	return nxs_list_get(&u_ctx->updates);
}

/*
 * Delete current head element from list and return next element
 */
nxs_chat_srv_m_tlgrm_update_t *nxs_chat_srv_u_db_queue_list_del_head(nxs_chat_srv_u_db_queue_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	return nxs_list_ptr_next(&u_ctx->updates);
}

/*
 * Get current head element from list and move pointer to next element
 */
nxs_chat_srv_m_tlgrm_update_t *nxs_chat_srv_u_db_queue_list_pull_head(nxs_chat_srv_u_db_queue_t *u_ctx)
{
	nxs_chat_srv_m_tlgrm_update_t *u;

	if(u_ctx == NULL) {

		return NULL;
	}

	u = nxs_list_get(&u_ctx->updates);

	nxs_list_ptr_next(&u_ctx->updates);

	return u;
}

/* Module internal (static) functions */
