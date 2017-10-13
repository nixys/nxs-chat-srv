#ifndef _INCLUDE_NXS_CHAT_SRV_U_DB_QUEUE_H
#define _INCLUDE_NXS_CHAT_SRV_U_DB_QUEUE_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_db_queue_s;

/* Prototypes */

nxs_chat_srv_u_db_queue_t		*nxs_chat_srv_u_db_queue_init			(void);
nxs_chat_srv_u_db_queue_t		*nxs_chat_srv_u_db_queue_free			(nxs_chat_srv_u_db_queue_t *u_ctx);

nxs_chat_srv_err_t			nxs_chat_srv_u_db_queue_add			(nxs_chat_srv_u_db_queue_t *u_ctx, size_t tlgrm_userid, nxs_chat_srv_m_tlgrm_update_type_t update_type, nxs_string_t *update_encoded);
nxs_chat_srv_err_t			nxs_chat_srv_u_db_queue_get			(nxs_chat_srv_u_db_queue_t *u_ctx);

nxs_chat_srv_err_t			nxs_chat_srv_u_db_queue_unlock			(nxs_chat_srv_u_db_queue_t *u_ctx);
nxs_chat_srv_err_t			nxs_chat_srv_u_db_queue_lock_update		(nxs_chat_srv_u_db_queue_t *u_ctx);

nxs_chat_srv_m_tlgrm_update_t		*nxs_chat_srv_u_db_queue_list_get_head		(nxs_chat_srv_u_db_queue_t *u_ctx);
nxs_chat_srv_m_tlgrm_update_t		*nxs_chat_srv_u_db_queue_list_del_head		(nxs_chat_srv_u_db_queue_t *u_ctx);
nxs_chat_srv_m_tlgrm_update_t		*nxs_chat_srv_u_db_queue_list_pull_head		(nxs_chat_srv_u_db_queue_t *u_ctx);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_DB_QUEUE_H */
