#ifndef _INCLUDE_NXS_CHAT_SRV_U_DB_MYSQL_SETUP_H
#define _INCLUDE_NXS_CHAT_SRV_U_DB_MYSQL_SETUP_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_db_mysql_setup_s;

/* Prototypes */

nxs_chat_srv_u_db_mysql_setup_t		*nxs_chat_srv_u_db_mysql_setup_init			(void);
nxs_chat_srv_u_db_mysql_setup_t		*nxs_chat_srv_u_db_mysql_setup_free			(nxs_chat_srv_u_db_mysql_setup_t *u_ctx);

nxs_chat_srv_err_t			nxs_chat_srv_u_db_mysql_setup_create_tables		(nxs_chat_srv_u_db_mysql_setup_t *u_ctx, nxs_string_t *err_str);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_DB_MYSQL_SETUP_H */
