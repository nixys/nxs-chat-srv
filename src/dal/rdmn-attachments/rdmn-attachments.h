#ifndef _INCLUDE_NXS_CHAT_SRV_D_RDMN_ATTACHMENTS_H
#define _INCLUDE_NXS_CHAT_SRV_D_RDMN_ATTACHMENTS_H

// clang-format off

/* Structs declarations */



/* Prototypes */

nxs_chat_srv_err_t		nxs_chat_srv_d_rdmn_attachments_info		(size_t attachment_id, nxs_http_code_t *http_code, nxs_buf_t *out_buf);
nxs_chat_srv_err_t		nxs_chat_srv_d_rdmn_attachments_download	(nxs_string_t *rdmn_content_url, nxs_string_t *file_save_path, nxs_http_code_t *http_code);

#endif /* _INCLUDE_NXS_CHAT_SRV_D_RDMN_ATTACHMENTS_H */
