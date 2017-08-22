#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/queue-worker/tlgrm-update/tlgrm-update.h>
#include <proc/queue-worker/tlgrm-update/ctx/tlgrm-update-ctx.h>
#include <proc/queue-worker/tlgrm-update/tlgrm-update-subproc.h>

/* Definitions */



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

/* TODO: sample function (junk) */
nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_runtime(nxs_chat_srv_m_queue_com_types_t type, nxs_string_t *data)
{
	nxs_string_t                        base64_decoded;
	nxs_chat_srv_u_tlgrm_update_t *     tlgrm_update_ctx;
	nxs_chat_srv_m_tlgrm_update_t *     update;
	nxs_chat_srv_u_db_users_t *         db_users_ctx;
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_chat_srv_m_db_users_t           m_user;
	nxs_chat_srv_err_t                  rc;

	nxs_string_t user_not_found = nxs_string("Пользователь не найден");
	nxs_string_t posted         = nxs_string("Комментарий отправлен в систему задач");

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&base64_decoded);

	tlgrm_update_ctx      = nxs_chat_srv_u_tlgrm_update_init();
	db_users_ctx          = nxs_chat_srv_u_db_users_init();
	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();

	nxs_base64_decode_string(&base64_decoded, data);

	if(nxs_chat_srv_u_tlgrm_update_pull_json(tlgrm_update_ctx, (nxs_buf_t *)&base64_decoded) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	update = nxs_chat_srv_u_tlgrm_update_get(tlgrm_update_ctx);

	if(update->message.reply_to_message != NULL) {

		/* reply to message */
	}
	else {

		/* empty reply to message */
	}

	if(nxs_chat_srv_u_db_users_pull(db_users_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_db_users_find_by_tid(db_users_ctx, update->message.from.id, &m_user) == NXS_CHAT_SRV_E_EXIST) {

		nxs_chat_srv_u_tlgrm_sendmessage_add(tlgrm_sendmessage_ctx, update->message.chat.id, &user_not_found);
	}
	else {

		nxs_chat_srv_u_tlgrm_sendmessage_add(tlgrm_sendmessage_ctx, update->message.chat.id, &posted);

		nxs_chat_srv_u_rdmn_issues_add_note(NULL, 46470, &update->message.text, m_user.rdmn_user_api_key);
	}

	if(nxs_chat_srv_u_tlgrm_sendmessage_push(tlgrm_sendmessage_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_string_free(&base64_decoded);

	tlgrm_update_ctx      = nxs_chat_srv_u_tlgrm_update_free(tlgrm_update_ctx);
	db_users_ctx          = nxs_chat_srv_u_db_users_free(db_users_ctx);
	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);

	return rc;
}

/* Module internal (static) functions */
