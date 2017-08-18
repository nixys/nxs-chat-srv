#ifndef _INCLUDE_NXS_CHAT_SRV_P_REST_API_CTX_H
#define _INCLUDE_NXS_CHAT_SRV_P_REST_API_CTX_H

// clang-format off

typedef struct
{
	nxs_rest_api_ctx_t		ra_conn;	/* Rest API context */
} nxs_chat_srv_p_rest_api_ctx_t;

nxs_chat_srv_err_t		nxs_chat_srv_p_rest_api_ctx_init		(nxs_chat_srv_p_rest_api_ctx_t *p_ctx);
nxs_chat_srv_err_t		nxs_chat_srv_p_rest_api_ctx_free		(nxs_chat_srv_p_rest_api_ctx_t *p_ctx);

nxs_rest_api_ctx_t		*nxs_chat_srv_p_rest_api_ctx_get_ra_conn	(nxs_chat_srv_p_rest_api_ctx_t *p_ctx);

#endif /* _INCLUDE_NXS_CHAT_SRV_P_REST_API_CTX_H */
