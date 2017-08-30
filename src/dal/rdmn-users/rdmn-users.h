#ifndef _INCLUDE_NXS_CHAT_SRV_D_RDMN_USERS_H
#define _INCLUDE_NXS_CHAT_SRV_D_RDMN_USERS_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_d_rdmn_users_s;

/* Prototypes */

nxs_chat_srv_err_t		nxs_chat_srv_d_rdmn_users_get		(size_t user_id, nxs_string_t *api_key, nxs_buf_t *out_buf, nxs_http_code_t *http_code, size_t offset, size_t limit);

#endif /* _INCLUDE_NXS_CHAT_SRV_D_RDMN_USERS_H */
