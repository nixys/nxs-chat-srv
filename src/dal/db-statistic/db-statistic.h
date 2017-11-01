#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_STATISTIC_H
#define _INCLUDE_NXS_CHAT_SRV_D_DB_STATISTIC_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_d_db_statistic_s;

/* Prototypes */

nxs_chat_srv_d_db_statistic_t		*nxs_chat_srv_d_db_statistic_init		(void);
nxs_chat_srv_d_db_statistic_t		*nxs_chat_srv_d_db_statistic_free		(nxs_chat_srv_d_db_statistic_t *d_ctx);

nxs_chat_srv_err_t			nxs_chat_srv_d_db_statistic_add			(nxs_chat_srv_d_db_statistic_t *d_ctx, nxs_string_t *action);
nxs_chat_srv_err_t			nxs_chat_srv_d_db_statistic_get			(nxs_chat_srv_d_db_statistic_t *d_ctx, nxs_array_t *actions);

#endif /* _INCLUDE_NXS_CHAT_SRV_D_DB_STATISTIC_H */
