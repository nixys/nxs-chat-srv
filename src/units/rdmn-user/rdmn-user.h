#ifndef _INCLUDE_NXS_CHAT_SRV_U_RDMN_USER_H
#define _INCLUDE_NXS_CHAT_SRV_U_RDMN_USER_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_rdmn_user_s;

/* Prototypes */

nxs_chat_srv_u_rdmn_user_t		*nxs_chat_srv_u_rdmn_user_init			(void);
nxs_chat_srv_u_rdmn_user_t		*nxs_chat_srv_u_rdmn_user_free			(nxs_chat_srv_u_rdmn_user_t *u_ctx);

nxs_chat_srv_err_t			nxs_chat_srv_u_rdmn_user_pull			(nxs_chat_srv_u_rdmn_user_t *u_ctx, size_t user_id);
nxs_chat_srv_err_t			nxs_chat_srv_u_rdmn_user_pull_current		(nxs_chat_srv_u_rdmn_user_t *u_ctx, nxs_string_t *api_key);

nxs_string_t				*nxs_chat_srv_u_rdmn_user_get_api_key		(nxs_chat_srv_u_rdmn_user_t *u_ctx);
size_t					nxs_chat_srv_u_rdmn_user_get_id			(nxs_chat_srv_u_rdmn_user_t *u_ctx);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_RDMN_USER_H */
