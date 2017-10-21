#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/queue-worker/rdmn-update/win-issue-updated/win-issue-updated.h>
#include <proc/queue-worker/rdmn-update/win-issue-updated/ctx/win-issue-updated-ctx.h>
#include <proc/queue-worker/rdmn-update/win-issue-updated/win-issue-updated-subproc.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef enum
{
	JOURNAL_PROPERTY_NONE,
	JOURNAL_PROPERTY_IS_PRIVATE,
	JOURNAL_PROPERTY_STATUS_ID,
	JOURNAL_PROPERTY_PRIORITY_ID,
	JOURNAL_PROPERTY_ASSIGNED_TO_ID,
} journal_property_t;

typedef struct
{
	nxs_string_t				name;
	journal_property_t			property;
} journal_properties_t;

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

static journal_property_t journal_attr_property_get(nxs_chat_srv_m_rdmn_detail_t *detail);
static size_t journal_attachment_id_get(nxs_chat_srv_m_rdmn_detail_t *detail);
static nxs_chat_srv_tlgrm_request_type_t tlgrm_send_method_get(nxs_string_t *file_name, nxs_string_t *content_type);

// clang-format off

/* Module initializations */

static journal_properties_t journal_properties[] =
{
	{nxs_string("is_private"),		JOURNAL_PROPERTY_IS_PRIVATE},
	{nxs_string("status_id"),		JOURNAL_PROPERTY_STATUS_ID},
	{nxs_string("priority_id"),		JOURNAL_PROPERTY_PRIORITY_ID},
	{nxs_string("assigned_to_id"),		JOURNAL_PROPERTY_ASSIGNED_TO_ID},

	{NXS_STRING_NULL_STR,			JOURNAL_PROPERTY_NONE}
};

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

static u_char		_s_private_message[]	= {NXS_CHAT_SRV_UTF8_PRIVATE_MESSAGE};

/* Module global functions */

// clang-format on

nxs_chat_srv_err_t
        nxs_chat_srv_p_queue_worker_rdmn_update_win_issue_updated_runtime(nxs_chat_srv_m_rdmn_update_t *      update,
                                                                          size_t                              tlgrm_userid,
                                                                          nxs_chat_srv_m_rdmn_journal_t *     journal,
                                                                          nxs_chat_srv_u_rdmn_attachments_t * rdmn_attachments_ctx,
                                                                          nxs_chat_srv_u_tlgrm_attachments_t *tlgrm_attachments_ctx)
{
	nxs_chat_srv_u_tlgrm_sendmessage_t *tlgrm_sendmessage_ctx;
	nxs_chat_srv_m_rdmn_detail_t *      d;
	nxs_string_t message, properties, property_is_private, property_status, property_priority, property_assigned_to, notes_fmt,
	        project_fmt, subject_fmt, user_fmt, status_fmt, priority_fmt, assigned_to_fmt, *s, file_name, file_path, description,
	        content_type;
	nxs_buf_t *                    out_buf;
	nxs_array_t                    m_chunks, attachments;
	nxs_bool_t                     response_status;
	nxs_chat_srv_m_tlgrm_message_t tlgrm_message;
	nxs_chat_srv_u_db_issues_t *   db_issue_ctx;
	nxs_bool_t                     use_property;
	size_t                         i, attachment_id, message_id, *a_id;

	if(update == NULL || journal == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	nxs_chat_srv_err_t rc;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&message);
	nxs_string_init_empty(&properties);
	nxs_string_init_empty(&property_is_private);
	nxs_string_init_empty(&property_status);
	nxs_string_init_empty(&property_priority);
	nxs_string_init_empty(&property_assigned_to);
	nxs_string_init_empty(&notes_fmt);
	nxs_string_init_empty(&project_fmt);
	nxs_string_init_empty(&subject_fmt);
	nxs_string_init_empty(&user_fmt);
	nxs_string_init_empty(&status_fmt);
	nxs_string_init_empty(&priority_fmt);
	nxs_string_init_empty(&assigned_to_fmt);
	nxs_string_init_empty(&file_name);
	nxs_string_init_empty(&file_path);
	nxs_string_init_empty(&description);
	nxs_string_init_empty(&content_type);

	nxs_array_init2(&m_chunks, nxs_string_t);
	nxs_array_init2(&attachments, size_t);

	tlgrm_sendmessage_ctx = nxs_chat_srv_u_tlgrm_sendmessage_init();
	db_issue_ctx          = nxs_chat_srv_u_db_issues_init();

	nxs_chat_srv_c_tlgrm_message_init(&tlgrm_message);

	/*
	 * Prepare property block content
	 */

	use_property = NXS_NO;

	for(i = 0; i < nxs_array_count(&journal->details); i++) {

		d = nxs_array_get(&journal->details, i);

		if((attachment_id = journal_attachment_id_get(d)) != 0) {

			a_id = nxs_array_add(&attachments);

			*a_id = attachment_id;
		}
		else {

			switch(journal_attr_property_get(d)) {

				case JOURNAL_PROPERTY_IS_PRIVATE:

					use_property = NXS_YES;

					if(update->data.issue.is_private == NXS_YES) {

						nxs_string_printf(&property_is_private,
						                  NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_UPDATED_IS_PRIVATE_YES);
					}
					else {

						nxs_string_printf(&property_is_private,
						                  NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_UPDATED_IS_PRIVATE_NO);
					}

					break;

				case JOURNAL_PROPERTY_STATUS_ID:

					use_property = NXS_YES;

					nxs_chat_srv_c_tlgrm_format_escape_html(&status_fmt, &update->data.issue.status.name);

					nxs_string_printf(&property_status, NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_UPDATED_STATUS, &status_fmt);

					break;

				case JOURNAL_PROPERTY_PRIORITY_ID:

					use_property = NXS_YES;

					nxs_chat_srv_c_tlgrm_format_escape_html(&priority_fmt, &update->data.issue.priority.name);

					nxs_string_printf(
					        &property_priority, NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_UPDATED_PRIORITY, &priority_fmt);

					break;

				case JOURNAL_PROPERTY_ASSIGNED_TO_ID:

					use_property = NXS_YES;

					nxs_chat_srv_c_tlgrm_format_escape_html(&assigned_to_fmt, &update->data.issue.assigned_to.name);

					nxs_string_printf(&property_assigned_to,
					                  NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_UPDATED_ASSIGNED_TO,
					                  &assigned_to_fmt);

					break;

				default:

					break;
			}
		}
	}

	if(use_property == NXS_YES) {

		nxs_string_printf(
		        &properties, "%r%r%r%r\n", &property_is_private, &property_status, &property_priority, &property_assigned_to);
	}

	nxs_chat_srv_c_tlgrm_format_escape_html(&project_fmt, &update->data.issue.project.name);
	nxs_chat_srv_c_tlgrm_format_escape_html(&subject_fmt, &update->data.issue.subject);
	nxs_chat_srv_c_tlgrm_format_escape_html(&user_fmt, &journal->user.name);

	if(nxs_string_len(&journal->notes) > 0) {

		nxs_chat_srv_c_tlgrm_format_escape_html(&notes_fmt, &journal->notes);

		nxs_string_printf(&message,
		                  NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_UPDATED,
		                  journal->private_notes == NXS_YES ? (char *)_s_private_message : "",
		                  &nxs_chat_srv_cfg.rdmn.host,
		                  update->data.issue.id,
		                  &project_fmt,
		                  update->data.issue.id,
		                  &subject_fmt,
		                  &properties,
		                  &user_fmt);

		nxs_chat_srv_c_tlgrm_make_message_chunks(&message, &notes_fmt, &m_chunks);
	}
	else {

		if(use_property == NXS_NO && nxs_array_count(&attachments) == 0) {

			nxs_error(rc, NXS_CHAT_SRV_E_OK, error);
		}

		nxs_string_printf(&message,
		                  NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_UPDATED_NO_MESSAGE,
		                  journal->private_notes == NXS_YES ? (char *)_s_private_message : "",
		                  &nxs_chat_srv_cfg.rdmn.host,
		                  update->data.issue.id,
		                  &project_fmt,
		                  update->data.issue.id,
		                  &subject_fmt,
		                  &properties,
		                  &user_fmt);

		nxs_chat_srv_c_tlgrm_make_message_chunks(&message, NULL, &m_chunks);
	}

	/* create new comment: send message chunks */
	for(i = 0; i < nxs_array_count(&m_chunks); i++) {

		s = nxs_array_get(&m_chunks, i);

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

	for(i = 0; i < nxs_array_count(&attachments); i++) {

		a_id = nxs_array_get(&attachments, i);

		if(nxs_chat_srv_u_rdmn_attachments_download(
		           rdmn_attachments_ctx, *a_id, &file_name, &file_path, &description, &content_type) == NXS_CHAT_SRV_E_OK) {

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

	nxs_chat_srv_c_tlgrm_message_free(&tlgrm_message);

	for(i = 0; i < nxs_array_count(&m_chunks); i++) {

		s = nxs_array_get(&m_chunks, i);

		nxs_string_free(s);
	}

	nxs_array_free(&m_chunks);
	nxs_array_free(&attachments);

	nxs_string_free(&message);
	nxs_string_free(&properties);
	nxs_string_free(&property_is_private);
	nxs_string_free(&property_status);
	nxs_string_free(&property_priority);
	nxs_string_free(&property_assigned_to);
	nxs_string_free(&notes_fmt);
	nxs_string_free(&project_fmt);
	nxs_string_free(&subject_fmt);
	nxs_string_free(&user_fmt);
	nxs_string_free(&status_fmt);
	nxs_string_free(&priority_fmt);
	nxs_string_free(&assigned_to_fmt);
	nxs_string_free(&file_name);
	nxs_string_free(&file_path);
	nxs_string_free(&description);
	nxs_string_free(&content_type);

	return rc;
}

/* Module internal (static) functions */

static journal_property_t journal_attr_property_get(nxs_chat_srv_m_rdmn_detail_t *detail)
{
	size_t i;

	if(detail == NULL || detail->_is_used == NXS_NO || detail->type != NXS_CHAT_SRV_M_RDMN_DETAIL_TYPE_ATTR ||
	   detail->attr._is_used == NXS_NO) {

		return JOURNAL_PROPERTY_NONE;
	}

	for(i = 0; journal_properties[i].property != JOURNAL_PROPERTY_NONE; i++) {

		if(nxs_string_cmp(&journal_properties[i].name, 0, &detail->attr.name, 0) == NXS_YES) {

			return journal_properties[i].property;
		}
	}

	return JOURNAL_PROPERTY_NONE;
}

static size_t journal_attachment_id_get(nxs_chat_srv_m_rdmn_detail_t *detail)
{

	if(detail == NULL || detail->_is_used == NXS_NO || detail->type != NXS_CHAT_SRV_M_RDMN_DETAIL_TYPE_ATTACHMENT ||
	   detail->attachment._is_used == NXS_NO) {

		return 0;
	}

	return detail->attachment.name;
}

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
