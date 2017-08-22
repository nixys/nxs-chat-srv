#ifndef _INCLUDE_NXS_CHAT_SRV_C_UNIX_SOCK_C
#define _INCLUDE_NXS_CHAT_SRV_C_UNIX_SOCK_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

/* Definitions */

#define NXS_CHAT_SRV_C_UNIX_SOCK_POLL_TIMEOUT	500
#define NXS_CHAT_SRV_C_UNIX_SOCK_RECV_TIMEOUT	500

/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_c_unix_sock_create(nxs_net_unix_server_t *sock)
{
	int                ec;
	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	if((ec = nxs_net_unix_server_init(&process,
	                                  sock,
	                                  nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_path),
	                                  (int)nxs_chat_srv_cfg.queue_worker.sock_max_conn,
	                                  nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_user),
	                                  nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_group),
	                                  &nxs_chat_srv_cfg.queue_worker.sock_mode)) != NXS_NET_E_OK) {

		switch(ec) {

			case NXS_NET_E_MAX_UNIX_PATH_SIZE:

				nxs_log_write_error(&process,
				                    "[%s]: failed to create queue socket: socket path is too long (sock path: \"%s\")",
				                    nxs_proc_get_name(&process),
				                    nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_path));

				break;

			case NXS_NET_E_PWNAM:

				nxs_log_write_error(&process,
				                    "[%s]: failed to create queue socket: can't resolve user name (sock path: \"%s\")",
				                    nxs_proc_get_name(&process),
				                    nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_path));

				break;

			case NXS_NET_E_GRNAM:

				nxs_log_write_error(&process,
				                    "[%s]: failed to create queue socket: can't resolve group name (sock path: \"%s\")",
				                    nxs_proc_get_name(&process),
				                    nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_path));

				break;

			default:

				nxs_log_write_error(&process,
				                    "[%s]: failed to create queue socket (sock path: \"%s\", error code: %d)",
				                    nxs_proc_get_name(&process),
				                    nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_path),
				                    ec);

				break;
		}

		return NXS_CHAT_SRV_E_ERR;
	}

	if((ec = nxs_net_unix_listen(&process, sock)) != NXS_NET_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: failed to create queue socket: listen error (sock path: \"%s\", error: \"%s\", error code: %d)",
		                    nxs_proc_get_name(&process),
		                    nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_path),
		                    strerror(errno),
		                    ec);

		return NXS_CHAT_SRV_E_ERR;
	}

	if((ec = nxs_net_unix_accept_poll_init(sock)) != NXS_NET_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: failed to create queue socket: poll error (sock path: \"%s\", error code: %d)",
		                    nxs_proc_get_name(&process),
		                    nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_path),
		                    ec);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	if(rc != NXS_CHAT_SRV_E_OK) {

		nxs_net_unix_server_unlink(&process, sock);
	}

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_c_unix_sock_send(nxs_string_t *message)
{
	nxs_chat_srv_err_t rc;
	nxs_net_connect_t  ctl_conn;
	int                ec;

	rc = NXS_CHAT_SRV_E_OK;

	switch((ec = nxs_net_unix_connect_out_init(
	                &process, &ctl_conn, nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_path), 1, (u_char *)""))) {

		case NXS_NET_E_MAX_UNIX_PATH_SIZE:

			nxs_log_write_error(&process,
			                    "[%s]: error while connecting to socket: socket path is too long (sock path: \"%s\")",
			                    nxs_proc_get_name(&process),
			                    nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_path));

			return NXS_CHAT_SRV_E_ERR;

		default:

			if(ec != NXS_NET_E_OK) {

				nxs_log_write_error(&process,
				                    "[%s]: error while connecting to socket (sock path: \"%s\", error code: %d)",
				                    nxs_proc_get_name(&process),
				                    nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_path),
				                    ec);

				return NXS_CHAT_SRV_E_ERR;
			}

			break;
	}

	switch((ec = nxs_net_unix_connect(&process, &ctl_conn))) {

		case NXS_NET_E_SOCK_CREATE:

			nxs_log_write_error(&process,
			                    "[%s]: error while connecting to socket: socket create error (sock path: \"%s\")",
			                    nxs_proc_get_name(&process),
			                    nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_path));

			return NXS_CHAT_SRV_E_ERR;

		case NXS_NET_E_CONNECT:

			nxs_log_write_error(&process,
			                    "[%s]: error while connecting to socket: socket connect error, check socket existence and "
			                    "permissions (sock path: \"%s\")",
			                    nxs_proc_get_name(&process),
			                    nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_path));

			return NXS_CHAT_SRV_E_ERR;

		default:

			if(ec != NXS_NET_E_OK) {

				nxs_log_write_error(&process,
				                    "[%s]: error while connecting to socket (sock path: \"%s\", error code: %d)",
				                    nxs_proc_get_name(&process),
				                    nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_path),
				                    ec);

				return NXS_CHAT_SRV_E_ERR;
			}
	}

	nxs_log_write_debug(&process,
	                    "[%s]: connected to socket (sock path: \"%s\")",
	                    nxs_proc_get_name(&process),
	                    nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_path));

	if((ec = nxs_net_send(&process, &ctl_conn, NXS_NET_TIMEOUT_DEFAULT, message, NXS_NET_DATA_TYPE_STRING)) != NXS_NET_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: error while sending command to socket (sock path: \"%s\", error code: %d)",
		                    nxs_proc_get_name(&process),
		                    nxs_string_str(&nxs_chat_srv_cfg.queue_worker.sock_path),
		                    ec);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

/* TODO: add here receive confirmation from peer */

error:

	nxs_net_unix_connect_close(&process, &ctl_conn);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_c_unix_sock_recv(nxs_net_unix_server_t *sock, nxs_buf_t *data)
{
	nxs_chat_srv_err_t rc;
	int                ec;
	nxs_net_connect_t  client;

	if((ec = nxs_net_unix_accept_poll_check(&process, sock, NXS_CHAT_SRV_C_UNIX_SOCK_POLL_TIMEOUT)) == NXS_NET_E_TIMEOUT) {

		return NXS_CHAT_SRV_E_TIMEOUT;
	}

	if(ec != NXS_NET_E_OK) {

		nxs_log_write_error(&process, "[%s]: queue recieve error: poll error (error code: %d)", nxs_proc_get_name(&process), ec);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_net_unix_connect_init(&client, (u_char *)"");

	if((ec = nxs_net_unix_accept(&process, sock, &client)) != NXS_NET_E_OK) {

		nxs_log_write_warn(&process, "[%s]: queue recieve error: accept error (error code: %d)", nxs_proc_get_name(&process), ec);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if((ec = nxs_net_recv(&process, &client, NXS_CHAT_SRV_C_UNIX_SOCK_RECV_TIMEOUT, data, NXS_NET_DATA_TYPE_STRING)) != NXS_NET_E_OK) {

		nxs_log_write_warn(
		        &process, "[%s]: queue recieve error: receive command error (error code: %d)", nxs_proc_get_name(&process), ec);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_buf_set_char(data, nxs_buf_get_len(data), (u_char)'\0');

	nxs_log_write_debug(
	        &process, "[%s]: got queue request (request: \"%s\")", nxs_proc_get_name(&process), nxs_buf_get_subbuf(data, 0));

/* TODO: add send confirm to rest-api peer */

error:

	nxs_net_unix_connect_close(&process, &client);

	return rc;
}

/* Module internal (static) functions */
