#ifndef _INCLUDE_NXS_CHAT_SRV_U_DB_SESS_H
#define _INCLUDE_NXS_CHAT_SRV_U_DB_SESS_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_db_sess_s;

/* Prototypes */

nxs_chat_srv_u_db_sess_t			*nxs_chat_srv_u_db_sess_init			(void);
nxs_chat_srv_u_db_sess_t			*nxs_chat_srv_u_db_sess_free			(nxs_chat_srv_u_db_sess_t *u_ctx);

nxs_chat_srv_err_t				nxs_chat_srv_u_db_sess_pull			(nxs_chat_srv_u_db_sess_t *u_ctx, size_t tlgrm_userid);

nxs_chat_srv_err_t				nxs_chat_srv_u_db_sess_start			(nxs_chat_srv_u_db_sess_t *u_ctx, size_t chat_id, size_t message_id, nxs_chat_srv_m_db_sess_type_t type, nxs_chat_srv_m_db_sess_wait_for_type_t wait_for);
nxs_chat_srv_err_t				nxs_chat_srv_u_db_sess_destroy			(nxs_chat_srv_u_db_sess_t *u_ctx);

nxs_bool_t					nxs_chat_srv_u_db_sess_check_exist		(nxs_chat_srv_u_db_sess_t *u_ctx);

size_t						nxs_chat_srv_u_db_sess_get_chat_id		(nxs_chat_srv_u_db_sess_t *u_ctx);
size_t						nxs_chat_srv_u_db_sess_get_message_id		(nxs_chat_srv_u_db_sess_t *u_ctx);
nxs_chat_srv_m_db_sess_type_t			nxs_chat_srv_u_db_sess_get_type			(nxs_chat_srv_u_db_sess_t *u_ctx);
nxs_chat_srv_m_db_sess_wait_for_type_t		nxs_chat_srv_u_db_sess_get_wait_for		(nxs_chat_srv_u_db_sess_t *u_ctx);

nxs_chat_srv_err_t				nxs_chat_srv_u_db_sess_set_wait_for		(nxs_chat_srv_u_db_sess_t *u_ctx, nxs_chat_srv_m_db_sess_wait_for_type_t wait_for);
nxs_chat_srv_err_t				nxs_chat_srv_u_db_sess_set_ids			(nxs_chat_srv_u_db_sess_t *u_ctx, size_t chat_id, size_t message_id);

nxs_string_t 					*nxs_chat_srv_u_db_sess_t_get_message		(nxs_chat_srv_u_db_sess_t *u_ctx);
nxs_chat_srv_err_t				nxs_chat_srv_u_db_sess_t_set_message		(nxs_chat_srv_u_db_sess_t *u_ctx, nxs_string_t *message);

nxs_chat_srv_err_t				nxs_chat_srv_u_db_sess_t_get_new_issue		(nxs_chat_srv_u_db_sess_t *u_ctx, size_t *project_id, nxs_string_t *project_name, size_t *priority_id, nxs_string_t *priority_name, nxs_string_t *subject, nxs_string_t *description, nxs_string_t *project_name_regex);
nxs_chat_srv_err_t				nxs_chat_srv_u_db_sess_t_set_new_issue		(nxs_chat_srv_u_db_sess_t *u_ctx, size_t project_id, nxs_string_t *project_name, size_t priority_id, nxs_string_t *priority_name, nxs_string_t *subject, nxs_string_t *description, nxs_string_t *project_name_regex);

nxs_chat_srv_err_t				nxs_chat_srv_u_db_sess_t_conv_to_new_issue	(nxs_chat_srv_u_db_sess_t *u_ctx, size_t project_id, nxs_string_t *project_name, size_t priority_id, nxs_string_t *priority_name, nxs_string_t *subject, nxs_string_t *description, nxs_string_t *project_name_regex);
nxs_chat_srv_err_t				nxs_chat_srv_u_db_sess_t_conv_to_message	(nxs_chat_srv_u_db_sess_t *u_ctx, nxs_string_t *message);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_DB_SESS_H */
