#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/queue-worker/tlgrm-update/win-begin/win-begin.h>
#include <proc/queue-worker/tlgrm-update/win-begin/ctx/win-begin-ctx.h>
#include <proc/queue-worker/tlgrm-update/win-begin/win-begin-subproc.h>

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

static nxs_string_t	_s_msg_begin	= nxs_string(NXS_CHAT_SRV_TLGRM_MESSAGE_BEGIN);

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t nxs_chat_srv_p_queue_worker_tlgrm_update_win_begin(nxs_chat_srv_u_db_sess_t *     sess_ctx,
                                                                      size_t                         chat_id,
                                                                      size_t                         message_id,
                                                                      nxs_chat_srv_m_tlgrm_update_t *update,
                                                                      nxs_buf_t *                    response_buf)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *    tlgrm_sendmessage_ctx;
	nxs_chat_srv_u_tlgrm_editmessagetext_t *tlgrm_editmessagetext_ctx;
	nxs_string_t                            callback_str;
	nxs_buf_t *                             b;
	nxs_chat_srv_err_t                      rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&callback_str);

	tlgrm_sendmessage_ctx     = nxs_chat_srv_u_tlgrm_sendmessage_init();
	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_init();

	if(message_id == 0) {

		/* create new comment */

		nxs_chat_srv_u_tlgrm_sendmessage_add(
		        tlgrm_sendmessage_ctx, chat_id, &_s_msg_begin, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(tlgrm_sendmessage_ctx,
		                                                               0,
		                                                               0,
		                                                               NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE,
		                                                               44938,
		                                                               NXS_CHAT_SRV_TLGRM_BUTTON_CAPTON_ADD_LAST_ACTIVE);

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(tlgrm_sendmessage_ctx,
		                                                               0,
		                                                               1,
		                                                               NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_NEW_ISSUE,
		                                                               0,
		                                                               NXS_CHAT_SRV_TLGRM_BUTTON_CAPTON_NEW_ISSUE);

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(
		        tlgrm_sendmessage_ctx,
		        1,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE,
		        46470,
		        NXS_CHAT_SRV_TLGRM_BUTTON_CAPTON_ADD_ADD_TO_ISSUE,
		        46470,
		        "Подключение услуг к серверу 148.251.68.145 в соответствии с выбранным тарифным планом Nixys Standard Plus");

		nxs_chat_srv_u_tlgrm_sendmessage_inline_keybutton_callback_add(
		        tlgrm_sendmessage_ctx,
		        2,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE,
		        46653,
		        NXS_CHAT_SRV_TLGRM_BUTTON_CAPTON_ADD_ADD_TO_ISSUE,
		        46653,
		        "Monitoring Alert: Spamassassin is not running on Seolinks-s1");

		// nxs_chat_srv_u_tlgrm_sendmessage_force_reply_set(tlgrm_sendmessage_ctx, 0);

		if(nxs_chat_srv_u_tlgrm_sendmessage_push(tlgrm_sendmessage_ctx) != NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		if(response_buf != NULL) {

			b = nxs_chat_srv_u_tlgrm_sendmessage_get_response_buf(tlgrm_sendmessage_ctx);

			nxs_buf_clone(response_buf, b);
		}
	}
	else {

		/* update existing comment */

		nxs_chat_srv_u_tlgrm_editmessagetext_add(
		        tlgrm_editmessagetext_ctx, chat_id, message_id, &_s_msg_begin, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN);

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(tlgrm_editmessagetext_ctx,
		                                                                   0,
		                                                                   0,
		                                                                   NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE,
		                                                                   44938,
		                                                                   NXS_CHAT_SRV_TLGRM_BUTTON_CAPTON_ADD_LAST_ACTIVE);

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(tlgrm_editmessagetext_ctx,
		                                                                   0,
		                                                                   1,
		                                                                   NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_NEW_ISSUE,
		                                                                   0,
		                                                                   NXS_CHAT_SRV_TLGRM_BUTTON_CAPTON_NEW_ISSUE);

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
		        tlgrm_editmessagetext_ctx,
		        1,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE,
		        46470,
		        NXS_CHAT_SRV_TLGRM_BUTTON_CAPTON_ADD_ADD_TO_ISSUE,
		        46470,
		        "Подключение услуг к серверу 148.251.68.145 в соответствии с выбранным тарифным планом Nixys Standard Plus");

		nxs_chat_srv_u_tlgrm_editmessagetext_inline_keybutton_callback_add(
		        tlgrm_editmessagetext_ctx,
		        2,
		        0,
		        NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_TO_ISSUE,
		        46653,
		        NXS_CHAT_SRV_TLGRM_BUTTON_CAPTON_ADD_ADD_TO_ISSUE,
		        46653,
		        "Monitoring Alert: Spamassassin is not running on Seolinks-s1");

		if(nxs_chat_srv_u_tlgrm_editmessagetext_push(tlgrm_editmessagetext_ctx) != NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		if(response_buf != NULL) {

			b = nxs_chat_srv_u_tlgrm_editmessagetext_get_response_buf(tlgrm_editmessagetext_ctx);

			nxs_buf_clone(response_buf, b);
		}
	}

error:

	nxs_string_free(&callback_str);

	tlgrm_sendmessage_ctx     = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);
	tlgrm_editmessagetext_ctx = nxs_chat_srv_u_tlgrm_editmessagetext_free(tlgrm_editmessagetext_ctx);

	return rc;
}

/* Module internal (static) functions */
