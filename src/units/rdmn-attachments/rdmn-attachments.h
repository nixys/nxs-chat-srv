#ifndef _INCLUDE_NXS_CHAT_SRV_U_RDMN_ATTACHMENTS_H
#define _INCLUDE_NXS_CHAT_SRV_U_RDMN_ATTACHMENTS_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_rdmn_attachments_s;

/* Prototypes */

nxs_chat_srv_u_rdmn_attachments_t		*nxs_chat_srv_u_rdmn_attachments_init		(void);
nxs_chat_srv_u_rdmn_attachments_t		*nxs_chat_srv_u_rdmn_attachments_free		(nxs_chat_srv_u_rdmn_attachments_t *u_ctx);
void						nxs_chat_srv_u_rdmn_attachments_clear		(nxs_chat_srv_u_rdmn_attachments_t *u_ctx);

nxs_chat_srv_err_t				nxs_chat_srv_u_rdmn_attachments_download	(nxs_chat_srv_u_rdmn_attachments_t *u_ctx, size_t attachment_id, nxs_string_t *file_name, nxs_string_t *file_path, nxs_string_t *description, nxs_string_t *content_type);

nxs_chat_srv_err_t				nxs_chat_srv_u_rdmn_attachments_upload		(nxs_chat_srv_u_rdmn_attachments_t *u_ctx, nxs_string_t *api_key, nxs_string_t *file_name, nxs_string_t *file_path, nxs_string_t *mime_type);
nxs_array_t					*nxs_chat_srv_u_rdmn_attachments_uploads_get	(nxs_chat_srv_u_rdmn_attachments_t *u_ctx);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_RDMN_ATTACHMENTS_H */
