#ifndef _INCLUDE_NXS_CHAT_SRV_C_MIME_H
#define _INCLUDE_NXS_CHAT_SRV_C_MIME_H

// clang-format off

/* Prototypes */

nxs_string_t			*nxs_chat_srv_c_mime_get			(nxs_string_t *file_name);
u_char				*nxs_chat_srv_c_mime_get_file_extension		(nxs_string_t *file_name);
void				nxs_chat_srv_c_mime_add_file_extension		(nxs_string_t *file_name_dst, nxs_string_t *file_name_src, nxs_string_t *mime_type);

#endif /* _INCLUDE_NXS_CHAT_SRV_C_MIME_H */
