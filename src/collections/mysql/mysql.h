#ifndef _INCLUDE_NXS_CHAT_SRV_C_MYSQL_H
#define _INCLUDE_NXS_CHAT_SRV_C_MYSQL_H

// clang-format off

/* Prototypes */

nxs_chat_srv_err_t		nxs_chat_srv_c_mysql_connect				(nxs_mysql_t *mysql);
nxs_chat_srv_err_t		nxs_chat_srv_c_mysql_disconnect				(nxs_mysql_t *mysql);
nxs_chat_srv_err_t		nxs_chat_srv_c_mysql_transaction_start			(nxs_mysql_t *mysql);
nxs_chat_srv_err_t		nxs_chat_srv_c_mysql_transaction_commit			(nxs_mysql_t *mysql);
nxs_chat_srv_err_t		nxs_chat_srv_c_mysql_transaction_rollback		(nxs_mysql_t *mysql);

#endif /* _INCLUDE_NXS_CHAT_SRV_C_MYSQL_H */
