#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_QUEUE_H
#define _INCLUDE_NXS_CHAT_SRV_D_DB_QUEUE_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_d_db_queue_s;

struct nxs_chat_srv_d_db_queue_meta_s
{
	size_t			tlgrm_userid;
	time_t			inactive_till;
};

/* Prototypes */

nxs_chat_srv_d_db_queue_t	*nxs_chat_srv_d_db_queue_init		(void);
nxs_chat_srv_d_db_queue_t	*nxs_chat_srv_d_db_queue_free		(nxs_chat_srv_d_db_queue_t *d_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_d_db_queue_add		(nxs_chat_srv_d_db_queue_t *d_ctx, size_t tlgrm_userid, time_t inactive_till, nxs_string_t *value);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_queue_get		(nxs_chat_srv_d_db_queue_t *d_ctx, size_t tlgrm_userid, nxs_array_t *values);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_queue_lock_set	(nxs_chat_srv_d_db_queue_t *d_ctx, size_t tlgrm_userid, time_t expire);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_queue_lock_update	(nxs_chat_srv_d_db_queue_t *d_ctx, size_t tlgrm_userid, time_t expire);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_queue_lock_del	(nxs_chat_srv_d_db_queue_t *d_ctx, size_t tlgrm_userid);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_queue_metas_get	(nxs_chat_srv_d_db_queue_t *d_ctx, nxs_array_t *metas);

#endif /* _INCLUDE_NXS_CHAT_SRV_D_DB_QUEUE_H */
