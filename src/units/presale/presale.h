#ifndef _INCLUDE_NXS_CHAT_SRV_U_PRESALE_H
#define _INCLUDE_NXS_CHAT_SRV_U_PRESALE_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_presale_s;

/* Prototypes */

nxs_chat_srv_u_presale_t	*nxs_chat_srv_u_presale_init			(void);
nxs_chat_srv_u_presale_t	*nxs_chat_srv_u_presale_free			(nxs_chat_srv_u_presale_t *u_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_u_presale_pull			(nxs_chat_srv_u_presale_t *u_ctx);

nxs_chat_srv_err_t		nxs_chat_srv_u_presale_w_set			(nxs_chat_srv_u_presale_t *u_ctx, size_t tlgrm_userid, nxs_string_t *update);
nxs_string_t			*nxs_chat_srv_u_presale_w_get			(nxs_chat_srv_u_presale_t *u_ctx, size_t i, size_t *tlgrm_userid);
nxs_chat_srv_err_t		nxs_chat_srv_u_presale_w_del			(nxs_chat_srv_u_presale_t *u_ctx, size_t tlgrm_userid);

nxs_chat_srv_err_t		nxs_chat_srv_u_presale_p_get			(nxs_chat_srv_u_presale_t *u_ctx, size_t i, size_t *tlgrm_userid, size_t *rdmn_issue_id);
nxs_chat_srv_err_t		nxs_chat_srv_u_presale_p_get_by_user_id		(nxs_chat_srv_u_presale_t *u_ctx, size_t tlgrm_userid, size_t *rdmn_issue_id);
nxs_chat_srv_err_t		nxs_chat_srv_u_presale_p_get_by_issue_id	(nxs_chat_srv_u_presale_t *u_ctx, size_t rdmn_issue_id, size_t *tlgrm_userid);
nxs_chat_srv_err_t		nxs_chat_srv_u_presale_p_set			(nxs_chat_srv_u_presale_t *u_ctx, size_t tlgrm_userid, size_t rdmn_issue_id);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_PRESALE_H */
