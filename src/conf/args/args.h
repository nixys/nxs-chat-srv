#ifndef _INCLUDE_NXS_CHAT_SRV_CONF_ARGS_H
#define _INCLUDE_NXS_CHAT_SRV_CONF_ARGS_H

// clang-format off

void						nxs_chat_srv_conf_args_init				(nxs_args_t *args, void *user_ctx);
nxs_chat_srv_err_t			nxs_chat_srv_conf_args_runtime				(nxs_args_t args, int argc, u_char **argv);

#endif /* _INCLUDE_NXS_CHAT_SRV_CONF_ARGS_H */
