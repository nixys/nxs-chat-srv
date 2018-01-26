#ifndef _INCLUDE_NXS_CHAT_SRV_D_TLGRM_H
#define _INCLUDE_NXS_CHAT_SRV_D_TLGRM_H

// clang-format off

/* Structs declarations */

/* Prototypes */

nxs_chat_srv_err_t		nxs_chat_srv_d_tlgrm_request		(nxs_chat_srv_tlgrm_request_type_t type, nxs_string_t *body, nxs_http_code_t *http_code, nxs_buf_t *out_buf);
nxs_chat_srv_err_t		nxs_chat_srv_d_tlgrm_download		(nxs_string_t *tlgrm_file_name, nxs_string_t *file_save_path, nxs_http_code_t *http_code);
nxs_chat_srv_err_t		nxs_chat_srv_d_tlgrm_upload		(nxs_chat_srv_tlgrm_request_type_t type, nxs_string_t *file_path, size_t chat_id, nxs_string_t *caption, nxs_http_code_t *http_code, nxs_buf_t *out_buf);
nxs_chat_srv_err_t		nxs_chat_srv_d_tlgrm_setwebhook		(nxs_string_t *url, nxs_string_t *ssl_srt_path, nxs_http_code_t *http_code, nxs_buf_t *out_buf);

#endif /* _INCLUDE_NXS_CHAT_SRV_D_TLGRM_H */
