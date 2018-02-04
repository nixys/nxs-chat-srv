#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/queue-worker/rdmn-update/win-issue-created/win-issue-created.h>
#include <proc/queue-worker/rdmn-update/win-issue-created/ctx/win-issue-created-ctx.h>
#include <proc/queue-worker/rdmn-update/win-issue-created/win-issue-created-subproc.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	nxs_string_t				file_extension;
	nxs_chat_srv_tlgrm_request_type_t	send_method;
} extension_send_method_t;

typedef struct
{
	nxs_string_t				content_type;
	nxs_chat_srv_tlgrm_request_type_t	send_method;
} mime_send_method_t;

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_tlgrm_request_type_t tlgrm_send_method_get(nxs_string_t *file_name, nxs_string_t *content_type);

// clang-format off

/* Module initializations */

static extension_send_method_t extension_send_method[] =
{
	{nxs_string("jpg"),			NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_PHOTO},
	{nxs_string("jpeg"),			NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_PHOTO},
	{nxs_string("png"),			NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_PHOTO},

	{nxs_string("webp"),			NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_STICKER},

	{nxs_string("mp3"),			NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VOICE},
	{nxs_string("ogg"),			NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VOICE},

	{nxs_string("3gp"),			NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VIDEO},
	{nxs_string("3gpp"),			NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VIDEO},
	{nxs_string("mp4"),			NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VIDEO},
	{nxs_string("mpeg"),			NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VIDEO},
	{nxs_string("mpg"),			NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VIDEO},
	{nxs_string("flv"),			NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VIDEO},
	{nxs_string("avi"),			NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VIDEO},

	{NXS_STRING_NULL_STR,			NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_DOCUMENT}
};

static mime_send_method_t mime_send_method[] =
{
	{nxs_string("image/webp"),		NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_STICKER},

	{nxs_string("audio/ogg"),		NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VOICE},
	{nxs_string("audio/mpeg"),		NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VOICE},

	{nxs_string("video/mp4"),		NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VIDEO},
	{nxs_string("video/mpeg"),		NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VIDEO},
	{nxs_string("video/3gpp"),		NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VIDEO},
	{nxs_string("video/x-msvideo"),		NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VIDEO},
	{nxs_string("video/x-flv"),		NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VIDEO},

	{NXS_STRING_NULL_STR,			NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_DOCUMENT}
};

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t
        nxs_chat_srv_p_queue_worker_rdmn_update_win_issue_created_runtime(nxs_chat_srv_m_user_ctx_t *         user_ctx,
                                                                          nxs_chat_srv_m_rdmn_update_t *      update,
                                                                          size_t                              tlgrm_userid,
                                                                          nxs_chat_srv_u_rdmn_attachments_t * rdmn_attachments_ctx,
                                                                          nxs_chat_srv_u_tlgrm_attachments_t *tlgrm_attachments_ctx)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_chat_srv_u_labels_t *           labels_ctx;
	nxs_string_t *s, message, private_issue, description_fmt, project_fmt, subject_fmt, author_fmt, status_fmt, status_label,
	        priority_fmt, priority_label, assigned_to_fmt, file_name, file_path, description, content_type;
	nxs_buf_t *                       out_buf;
	nxs_array_t                       d_chunks;
	nxs_bool_t                        response_status;
	nxs_chat_srv_m_tlgrm_message_t    tlgrm_message;
	nxs_chat_srv_m_rdmn_attachment_t *rdmn_attachment;
	nxs_chat_srv_u_db_issues_t *      db_issue_ctx;
	size_t                            i, message_id;

	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&message);
	nxs_string_init_empty(&private_issue);
	nxs_string_init_empty(&description_fmt);
	nxs_string_init_empty(&project_fmt);
	nxs_string_init_empty(&subject_fmt);
	nxs_string_init_empty(&author_fmt);
	nxs_string_init_empty(&status_fmt);
	nxs_string_init_empty(&status_label);
	nxs_string_init_empty(&priority_fmt);
	nxs_string_init_empty(&priority_label);
	nxs_string_init_empty(&assigned_to_fmt);
	nxs_string_init_empty(&file_name);
	nxs_string_init_empty(&file_path);
	nxs_string_init_empty(&description);
	nxs_string_init_empty(&content_type);

	nxs_array_init2(&d_chunks, nxs_string_t);

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();
	db_issue_ctx          = nxs_chat_srv_u_db_issues_init();
	labels_ctx            = nxs_chat_srv_u_labels_init();

	nxs_chat_srv_c_tlgrm_message_init(&tlgrm_message);

	if(update->data.issue.is_private == NXS_YES) {

		nxs_string_clone(&private_issue,
		                 nxs_chat_srv_u_labels_compile_key(labels_ctx,
		                                                   &user_ctx->r_userlang,
		                                                   NXS_CHAT_SRV_U_LABELS_KEY_CREATED_NEW_ISSUE_IN_REDMINE_IS_PRIVATE));
	}

	nxs_chat_srv_c_tlgrm_format_escape_html(&description_fmt, &update->data.issue.description);
	nxs_chat_srv_c_tlgrm_format_escape_html(&project_fmt, &update->data.issue.project.name);
	nxs_chat_srv_c_tlgrm_format_escape_html(&subject_fmt, &update->data.issue.subject);
	nxs_chat_srv_c_tlgrm_format_escape_html(&author_fmt, &update->data.issue.author.name);
	nxs_chat_srv_c_tlgrm_format_escape_html(&status_fmt, &update->data.issue.status.name);
	nxs_chat_srv_c_tlgrm_format_escape_html(&priority_fmt, &update->data.issue.priority.name);
	nxs_chat_srv_c_tlgrm_format_escape_html(&assigned_to_fmt, &update->data.issue.assigned_to.name);

	/* trying to translate the priority name */
	nxs_string_clone(&priority_label,
	                 nxs_chat_srv_u_labels_compile_key(labels_ctx, &user_ctx->r_userlang, (char *)nxs_string_str(&priority_fmt)));

	/* trying to translate the status name */
	nxs_string_clone(&status_label,
	                 nxs_chat_srv_u_labels_compile_key(labels_ctx, &user_ctx->r_userlang, (char *)nxs_string_str(&status_fmt)));

	nxs_chat_srv_u_labels_variable_add(labels_ctx,
	                                   "issue_link",
	                                   NXS_CHAT_SRV_TLGRM_MESSAGE_ISSUE_LINK_FMT,
	                                   &nxs_chat_srv_cfg.rdmn.host,
	                                   update->data.issue.id,
	                                   &project_fmt,
	                                   update->data.issue.id,
	                                   &subject_fmt);
	nxs_chat_srv_u_labels_variable_add(labels_ctx, "is_private", "%r", &private_issue);
	nxs_chat_srv_u_labels_variable_add(labels_ctx, "author", "%r", &author_fmt);
	nxs_chat_srv_u_labels_variable_add(labels_ctx, "status", "%r", &status_label);
	nxs_chat_srv_u_labels_variable_add(labels_ctx, "priority", "%r", &priority_label);
	nxs_chat_srv_u_labels_variable_add(labels_ctx, "assigned_to", "%r", &assigned_to_fmt);

	nxs_string_clone(&message,
	                 nxs_chat_srv_u_labels_compile_key(
	                         labels_ctx, &user_ctx->r_userlang, NXS_CHAT_SRV_U_LABELS_KEY_NEW_ISSUE_CREATED_IN_REDMINE));

	if(nxs_chat_srv_c_tlgrm_make_message_chunks(&message, &description_fmt, &d_chunks) == NXS_CHAT_SRV_E_WARN) {

		nxs_chat_srv_u_labels_variable_clear(labels_ctx);

		s = nxs_array_add(&d_chunks);

		nxs_string_init(s);

		nxs_string_printf(s,
		                  "%r%r",
		                  &message,
		                  nxs_chat_srv_u_labels_compile_key(
		                          labels_ctx, &nxs_chat_srv_cfg.labels.default_lang, NXS_CHAT_SRV_U_LABELS_KEY_MESSAGE_TOO_LARGE));
	}

	/* create new comment: send description chunks */
	for(i = 0; i < nxs_array_count(&d_chunks); i++) {

		s = nxs_array_get(&d_chunks, i);

		nxs_chat_srv_u_tlgrm_sendmessage_add(tlgrm_sendmessage_ctx, tlgrm_userid, s, NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_HTML);

		nxs_chat_srv_u_tlgrm_sendmessage_disable_web_page_preview(tlgrm_sendmessage_ctx);

		if(nxs_chat_srv_u_tlgrm_sendmessage_push(tlgrm_sendmessage_ctx) != NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		out_buf = nxs_chat_srv_u_tlgrm_sendmessage_get_response_buf(tlgrm_sendmessage_ctx);

		if(nxs_chat_srv_c_tlgrm_message_result_pull_json(&tlgrm_message, &response_status, out_buf) == NXS_CHAT_SRV_E_OK) {

			nxs_chat_srv_u_db_issues_set(db_issue_ctx, tlgrm_userid, tlgrm_message.message_id, update->data.issue.id);
		}
	}

	/* sending attachments to user */

	for(i = 0; i < nxs_array_count(&update->data.issue.attachments); i++) {

		rdmn_attachment = nxs_array_get(&update->data.issue.attachments, i);

		if(nxs_chat_srv_u_rdmn_attachments_download(
		           rdmn_attachments_ctx, rdmn_attachment->id, &file_name, &file_path, &description, &content_type) ==
		   NXS_CHAT_SRV_E_OK) {

			switch(tlgrm_send_method_get(&file_name, &content_type)) {

				case NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_PHOTO:

					if(nxs_chat_srv_u_tlgrm_attachments_send_photo(
					           tlgrm_attachments_ctx, tlgrm_userid, &file_path, &description, &message_id) ==
					   NXS_CHAT_SRV_E_OK) {

						nxs_chat_srv_u_db_issues_set(db_issue_ctx, tlgrm_userid, message_id, update->data.issue.id);
					}

					break;

				case NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_STICKER:

					if(nxs_chat_srv_u_tlgrm_attachments_send_sticker(
					           tlgrm_attachments_ctx, tlgrm_userid, &file_path, &message_id) == NXS_CHAT_SRV_E_OK) {

						nxs_chat_srv_u_db_issues_set(db_issue_ctx, tlgrm_userid, message_id, update->data.issue.id);
					}

					break;

				case NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VOICE:

					if(nxs_chat_srv_u_tlgrm_attachments_send_voice(
					           tlgrm_attachments_ctx, tlgrm_userid, &file_path, &description, &message_id) ==
					   NXS_CHAT_SRV_E_OK) {

						nxs_chat_srv_u_db_issues_set(db_issue_ctx, tlgrm_userid, message_id, update->data.issue.id);
					}

					break;

				case NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_VIDEO:

					if(nxs_chat_srv_u_tlgrm_attachments_send_video(
					           tlgrm_attachments_ctx, tlgrm_userid, &file_path, &description, &message_id) ==
					   NXS_CHAT_SRV_E_OK) {

						nxs_chat_srv_u_db_issues_set(db_issue_ctx, tlgrm_userid, message_id, update->data.issue.id);
					}

					break;

				case NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_DOCUMENT:
				default:

					if(nxs_chat_srv_u_tlgrm_attachments_send_document(
					           tlgrm_attachments_ctx, tlgrm_userid, &file_path, &description, &message_id) ==
					   NXS_CHAT_SRV_E_OK) {

						nxs_chat_srv_u_db_issues_set(db_issue_ctx, tlgrm_userid, message_id, update->data.issue.id);
					}

					break;
			}
		}
	}

error:

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_free(tlgrm_sendmessage_ctx);
	db_issue_ctx          = nxs_chat_srv_u_db_issues_free(db_issue_ctx);
	labels_ctx            = nxs_chat_srv_u_labels_free(labels_ctx);

	nxs_chat_srv_c_tlgrm_message_free(&tlgrm_message);

	for(i = 0; i < nxs_array_count(&d_chunks); i++) {

		s = nxs_array_get(&d_chunks, i);

		nxs_string_free(s);
	}

	nxs_array_free(&d_chunks);

	nxs_string_free(&message);
	nxs_string_free(&private_issue);
	nxs_string_free(&description_fmt);
	nxs_string_free(&project_fmt);
	nxs_string_free(&subject_fmt);
	nxs_string_free(&author_fmt);
	nxs_string_free(&status_fmt);
	nxs_string_free(&status_label);
	nxs_string_free(&priority_fmt);
	nxs_string_free(&priority_label);
	nxs_string_free(&assigned_to_fmt);
	nxs_string_free(&file_name);
	nxs_string_free(&file_path);
	nxs_string_free(&description);
	nxs_string_free(&content_type);

	return rc;
}

/* Module internal (static) functions */

/*
 * If content_type is set (len > 0) and found in mime_send_method[] - mime_send_method.send_method will be returned
 * else extension_send_method.send_method will be returned
 */
static nxs_chat_srv_tlgrm_request_type_t tlgrm_send_method_get(nxs_string_t *file_name, nxs_string_t *content_type)
{
	u_char *ext;
	size_t  i;

	if(file_name == NULL) {

		return NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_DOCUMENT;
	}

	if(nxs_string_len(content_type) > 0) {

		for(i = 0; nxs_string_len(&mime_send_method[i].content_type) > 0; i++) {

			if(nxs_string_cmp(&mime_send_method[i].content_type, 0, content_type, 0) == NXS_YES) {

				return mime_send_method[i].send_method;
			}
		}
	}

	if((ext = nxs_chat_srv_c_mime_get_file_extension(file_name)) == NULL) {

		return NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_DOCUMENT;
	}

	for(i = 0; nxs_string_len(&extension_send_method[i].file_extension) > 0; i++) {

		if(nxs_string_char_cmp(&extension_send_method[i].file_extension, 0, ext) == NXS_YES) {

			return extension_send_method[i].send_method;
		}
	}

	return NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_DOCUMENT;
}
