#ifndef _INCLUDE_NXS_CHAT_SRV_M_USER_CTX_H
#define _INCLUDE_NXS_CHAT_SRV_M_USER_CTX_H

// clang-format off

typedef struct
{
	size_t			r_userid;
	nxs_string_t		t_username;
	nxs_string_t		t_userlang;
	nxs_string_t		r_username;
	nxs_string_t		r_userfname;
	nxs_string_t		r_userlname;
} nxs_chat_srv_m_user_ctx_t;

#endif /* _INCLUDE_NXS_CHAT_SRV_M_USER_CTX_H */
