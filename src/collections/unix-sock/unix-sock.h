#ifndef _INCLUDE_NXS_CHAT_SRV_C_UNIX_SOCK_H
#define _INCLUDE_NXS_CHAT_SRV_C_UNIX_SOCK_H

// clang-format off

/* Prototypes */

nxs_chat_srv_err_t		nxs_chat_srv_c_unix_sock_create		(nxs_net_unix_server_t *sock);
nxs_chat_srv_err_t		nxs_chat_srv_c_unix_sock_send		(nxs_string_t *message);
nxs_chat_srv_err_t		nxs_chat_srv_c_unix_sock_recv		(nxs_net_unix_server_t *sock, nxs_string_t *data);

#endif /* _INCLUDE_NXS_CHAT_SRV_C_UNIX_SOCK_H */
