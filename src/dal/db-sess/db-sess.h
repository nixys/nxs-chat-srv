#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_SESS_H
#define _INCLUDE_NXS_CHAT_SRV_D_DB_SESS_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_d_db_sess_s;

/* Prototypes */

nxs_chat_srv_d_db_sess_t	*nxs_chat_srv_d_db_sess_init		(void);
nxs_chat_srv_d_db_sess_t	*nxs_chat_srv_d_db_sess_free		(nxs_chat_srv_d_db_sess_t *d_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_d_db_sess_select		(nxs_chat_srv_d_db_sess_t *d_ctx, nxs_mysql_t *mysql_ctx);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_sess_insert		(nxs_mysql_t *mysql_ctx, size_t *sess_id, size_t tlgrm_userid, size_t chat_id, size_t message_id, time_t updated_at, nxs_chat_srv_m_db_sess_type_t type, nxs_chat_srv_m_db_sess_wait_for_type_t wait_for, nxs_string_t *data);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_sess_update		(nxs_mysql_t *mysql_ctx, size_t sess_id, size_t tlgrm_userid, size_t chat_id, size_t message_id, time_t updated_at, nxs_chat_srv_m_db_sess_type_t type, nxs_chat_srv_m_db_sess_wait_for_type_t wait_for, nxs_string_t *data);
nxs_chat_srv_err_t		nxs_chat_srv_d_db_sess_delete		(nxs_mysql_t *mysql_ctx, size_t sess_id);

nxs_chat_srv_err_t		nxs_chat_srv_d_db_sess_get		(nxs_chat_srv_d_db_sess_t *d_ctx, size_t i, nxs_chat_srv_m_db_sess_t *sess);

#endif /* _INCLUDE_NXS_CHAT_SRV_D_DB_SESS_H */
