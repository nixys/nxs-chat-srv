#ifndef _INCLUDE_NXS_CHAT_SRV_C_TLGRM_C
#define _INCLUDE_NXS_CHAT_SRV_C_TLGRM_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	nxs_chat_srv_m_tlgrm_chat_type_t	type;
	nxs_string_t				name;
} nxs_chat_srv_c_tlgrm_types_t;

typedef struct
{
	nxs_chat_srv_m_tlgrm_parse_mode_t	parse_mode;
	nxs_string_t				name;
} nxs_chat_srv_c_tlgrm_parse_mode_t;

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_err_t nxs_chat_srv_c_tlgrm_update_pull_json_extract(nxs_chat_srv_m_tlgrm_update_t *update, nxs_buf_t *json_buf);
static nxs_chat_srv_err_t nxs_chat_srv_c_tlgrm_message_result_pull_json_extract(nxs_chat_srv_m_tlgrm_message_t *message,
                                                                                nxs_bool_t *                    status,
                                                                                nxs_buf_t *                     json_buf);
static nxs_chat_srv_err_t
        nxs_chat_srv_c_tlgrm_file_result_pull_json_extract(nxs_chat_srv_m_tlgrm_file_t *file, nxs_bool_t *status, nxs_buf_t *json_buf);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_message(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_callback_query(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_chat(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_chat_srv_c_tlgrm_extract_json_entity(nxs_process_t *     proc,
                                                                     nxs_json_t *        json,
                                                                     nxs_cfg_json_par_t *cfg_json_par_el,
                                                                     nxs_array_t *       cfg_arr);
static nxs_cfg_json_state_t nxs_chat_srv_c_tlgrm_extract_json_photo_sizes(nxs_process_t *     proc,
                                                                          nxs_json_t *        json,
                                                                          nxs_cfg_json_par_t *cfg_json_par_el,
                                                                          nxs_array_t *       cfg_arr);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_photo_size(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_file(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_document(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_sticker(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

// clang-format off

/* Module initializations */

static nxs_string_t	_s_par_update_id	= nxs_string("update_id");
static nxs_string_t	_s_par_message		= nxs_string("message");
static nxs_string_t	_s_par_message_id	= nxs_string("message_id");
static nxs_string_t	_s_par_text		= nxs_string("text");
static nxs_string_t	_s_par_chat		= nxs_string("chat");
static nxs_string_t	_s_par_id		= nxs_string("id");
static nxs_string_t	_s_par_username		= nxs_string("username");
static nxs_string_t	_s_par_type		= nxs_string("type");
static nxs_string_t	_s_par_from		= nxs_string("from");
static nxs_string_t	_s_par_first_name	= nxs_string("first_name");
static nxs_string_t	_s_par_last_name	= nxs_string("last_name");
static nxs_string_t	_s_par_language_code	= nxs_string("language_code");
static nxs_string_t	_s_par_reply_to_message	= nxs_string("reply_to_message");
static nxs_string_t	_s_par_entities		= nxs_string("entities");
static nxs_string_t	_s_par_callback_query	= nxs_string("callback_query");
static nxs_string_t	_s_par_chat_instance	= nxs_string("chat_instance");
static nxs_string_t	_s_par_data		= nxs_string("data");
static nxs_string_t	_s_par_result		= nxs_string("result");
static nxs_string_t	_s_par_ok		= nxs_string("ok");
static nxs_string_t	_s_par_t		= nxs_string("t");
static nxs_string_t	_s_par_o_id		= nxs_string("o_id");
static nxs_string_t	_s_par_offset		= nxs_string("offset");
static nxs_string_t	_s_par_length		= nxs_string("length");
static nxs_string_t	_s_par_url		= nxs_string("url");
static nxs_string_t	_s_par_caption		= nxs_string("caption");
static nxs_string_t	_s_par_photo		= nxs_string("photo");
static nxs_string_t	_s_par_file_id		= nxs_string("file_id");
static nxs_string_t	_s_par_width		= nxs_string("width");
static nxs_string_t	_s_par_height		= nxs_string("height");
static nxs_string_t	_s_par_file_size	= nxs_string("file_size");
static nxs_string_t	_s_par_file_path	= nxs_string("file_path");
static nxs_string_t	_s_par_file_name	= nxs_string("file_name");
static nxs_string_t	_s_par_document		= nxs_string("document");
static nxs_string_t	_s_par_thumb		= nxs_string("thumb");
static nxs_string_t	_s_par_mime_type	= nxs_string("mime_type");
static nxs_string_t	_s_par_emoji		= nxs_string("emoji");
static nxs_string_t	_s_par_set_name		= nxs_string("set_name");
static nxs_string_t	_s_par_sticker		= nxs_string("sticker");


static nxs_chat_srv_c_tlgrm_types_t chat_types[] =
{
	{NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_PRIVATE,		nxs_string("private")},
	{NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_GROUP,			nxs_string("group")},
	{NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_SUPERGROUP,		nxs_string("supergroup")},
	{NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_CHANNEL,		nxs_string("channel")},

	{NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_NONE,			nxs_string("")}
};

static nxs_chat_srv_c_tlgrm_parse_mode_t parse_mode[] =
{
	{NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_MARKDOWN,		nxs_string("Markdown")},
	{NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_HTML,		nxs_string("HTML")},

	{NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_NONE,		nxs_string("")}
};

/* Module global functions */

// clang-format on

void nxs_chat_srv_c_tlgrm_format_escape_markdown(nxs_string_t *text_dst, nxs_string_t *text_src)
{
	size_t       i;
	u_char       c;
	nxs_string_t text_dst_tmp, *t;

	if(text_dst == NULL) {

		nxs_string_init(&text_dst_tmp);

		t = &text_dst_tmp;
	}
	else {

		t = text_dst;
	}

	for(i = 0; i < nxs_string_len(text_src); i++) {

		c = nxs_string_get_char(text_src, i);

		switch(c) {

			case(u_char)'*':
			case(u_char)'_':
			case(u_char)'`':
			case(u_char)'[':

				nxs_string_char_add_char(t, (u_char)'\\');

				break;
		}

		nxs_string_char_add_char(t, c);
	}

	if(text_dst == NULL) {

		nxs_string_clone(text_src, t);

		nxs_string_free(&text_dst_tmp);
	}
}

void nxs_chat_srv_c_tlgrm_format_escape_html(nxs_string_t *text_dst, nxs_string_t *text_src)
{
	size_t       i;
	u_char       c;
	nxs_string_t text_dst_tmp, *t;

	if(text_dst == NULL) {

		nxs_string_init(&text_dst_tmp);

		t = &text_dst_tmp;
	}
	else {

		t = text_dst;
	}

	for(i = 0; i < nxs_string_len(text_src); i++) {

		c = nxs_string_get_char(text_src, i);

		switch(c) {

			case(u_char)'<':

				nxs_string_char_cat(t, (u_char *)"&lt;");

				break;

			case(u_char)'>':

				nxs_string_char_cat(t, (u_char *)"&gt;");

				break;

			case(u_char)'&':

				nxs_string_char_cat(t, (u_char *)"&amp;");

				break;

			default:

				nxs_string_char_add_char(t, c);

				break;
		}
	}

	if(text_dst == NULL) {

		nxs_string_clone(text_src, t);

		nxs_string_free(&text_dst_tmp);
	}
}

/*
 * Split 'message_src' to chunks and put 'message_header' into first chunk
 */
nxs_chat_srv_err_t nxs_chat_srv_c_tlgrm_make_message_chunks(nxs_string_t *message_header, nxs_string_t *message_src, nxs_array_t *chunks)
{
	nxs_string_t *s;
	nxs_bool_t    f;
	size_t        i, o;
	u_char        c;

	if(message_header == NULL || chunks == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(message_src == NULL) {

		s = nxs_array_add(chunks);

		nxs_string_init(s);

		nxs_string_printf(s, "%r", message_header);

		return NXS_CHAT_SRV_E_OK;
	}

	if(nxs_string_len(message_src) <= NXS_CHAT_SRV_TLGRM_MAX_MESSAGE_SIZE) {

		s = nxs_array_add(chunks);

		nxs_string_init(s);

		nxs_string_printf(s, "%r%r", message_header, message_src);

		return NXS_CHAT_SRV_E_OK;
	}

	o = 0;

	do {

		for(f = NXS_NO, i = o + NXS_CHAT_SRV_TLGRM_MAX_MESSAGE_SIZE; i > o; i--) {

			c = nxs_string_get_char(message_src, i);

			if(c == (u_char)' ' || c == (u_char)'\t' || c == (u_char)'\n') {

				f = NXS_YES;

				break;
			}
		}

		if(f == NXS_YES) {

			s = nxs_array_add(chunks);

			if(nxs_array_count(chunks) == 1) {

				nxs_string_init3(s, message_header);

				nxs_string_ncpy(s, nxs_string_len(s), message_src, o, i - o);
			}
			else {

				nxs_string_init(s);

				nxs_string_ncpy(s, 0, message_src, o, i - o);
			}

			o = i + 1;
		}
		else {

			/* message too big */

			for(i = 0; i < nxs_array_count(chunks); i++) {

				s = nxs_array_get(chunks, i);

				nxs_string_free(s);
			}

			nxs_array_clear(chunks);

			s = nxs_array_add(chunks);

			nxs_string_init(s);

			nxs_string_printf(s, "%r%s", message_header, NXS_CHAT_SRV_RDMN_MESSAGE_ISSUE_TOO_BIG);

			return NXS_CHAT_SRV_E_WARN;
		}

	} while(o + NXS_CHAT_SRV_TLGRM_MAX_MESSAGE_SIZE < nxs_string_len(message_src));

	if(nxs_string_len(message_src) - o > 0) {

		s = nxs_array_add(chunks);

		nxs_string_init(s);

		nxs_string_ncpy(s, 0, message_src, o, nxs_string_len(message_src) - o);
	}

	return NXS_CHAT_SRV_E_OK;
}

void nxs_chat_srv_c_tlgrm_update_init(nxs_chat_srv_m_tlgrm_update_t *update)
{

	if(update == NULL) {

		return;
	}

	update->_is_used  = NXS_NO;
	update->update_id = 0;

	nxs_chat_srv_c_tlgrm_callback_query_init(&update->callback_query);
	nxs_chat_srv_c_tlgrm_message_init(&update->message);
}

void nxs_chat_srv_c_tlgrm_update_free(nxs_chat_srv_m_tlgrm_update_t *update)
{

	if(update == NULL) {

		return;
	}

	update->_is_used  = NXS_NO;
	update->update_id = 0;

	nxs_chat_srv_c_tlgrm_callback_query_free(&update->callback_query);
	nxs_chat_srv_c_tlgrm_message_free(&update->message);
}

nxs_chat_srv_err_t nxs_chat_srv_c_tlgrm_update_pull_json(nxs_chat_srv_m_tlgrm_update_t *update, nxs_buf_t *json_buf)
{

	if(update == NULL || json_buf == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	return nxs_chat_srv_c_tlgrm_update_pull_json_extract(update, json_buf);
}
nxs_chat_srv_m_tlgrm_update_type_t nxs_chat_srv_c_tlgrm_update_get_type(nxs_chat_srv_m_tlgrm_update_t *update)
{

	if(update == NULL) {

		return NXS_CHAT_SRV_M_TLGRM_UPDATE_TYPE_NONE;
	}

	if(update->callback_query._is_used == NXS_YES) {

		return NXS_CHAT_SRV_M_TLGRM_UPDATE_TYPE_CALLBACK;
	}

	if(update->message._is_used == NXS_YES) {

		return NXS_CHAT_SRV_M_TLGRM_UPDATE_TYPE_MESSAGE;
	}

	return NXS_CHAT_SRV_M_TLGRM_UPDATE_TYPE_NONE;
}

void nxs_chat_srv_c_tlgrm_callback_query_init(nxs_chat_srv_m_tlgrm_callback_query_t *callback_query)
{
	if(callback_query == NULL) {

		return;
	}

	callback_query->_is_used = NXS_NO;

	nxs_string_init_empty(&callback_query->id);
	nxs_string_init_empty(&callback_query->chat_instance);
	nxs_string_init_empty(&callback_query->data);

	nxs_chat_srv_c_tlgrm_user_init(&callback_query->from);
	nxs_chat_srv_c_tlgrm_message_init(&callback_query->message);
}

void nxs_chat_srv_c_tlgrm_callback_query_free(nxs_chat_srv_m_tlgrm_callback_query_t *callback_query)
{
	if(callback_query == NULL) {

		return;
	}

	callback_query->_is_used = NXS_NO;

	nxs_string_free(&callback_query->id);
	nxs_string_free(&callback_query->chat_instance);
	nxs_string_free(&callback_query->data);

	nxs_chat_srv_c_tlgrm_user_free(&callback_query->from);
	nxs_chat_srv_c_tlgrm_message_free(&callback_query->message);
}

void nxs_chat_srv_c_tlgrm_message_entity_init(nxs_chat_srv_m_tlgrm_message_entity_t *entity)
{

	if(entity == NULL) {

		return;
	}

	entity->_is_used = NXS_NO;

	entity->length = 0;
	entity->offset = 0;

	nxs_string_init_empty(&entity->type);
	nxs_string_init_empty(&entity->url);
}

void nxs_chat_srv_c_tlgrm_message_entity_free(nxs_chat_srv_m_tlgrm_message_entity_t *entity)
{

	if(entity == NULL) {

		return;
	}

	entity->_is_used = NXS_NO;

	entity->length = 0;
	entity->offset = 0;

	nxs_string_free(&entity->type);
	nxs_string_free(&entity->url);
}

void nxs_chat_srv_c_tlgrm_photo_size_init(nxs_chat_srv_m_tlgrm_photo_size_t *photo_size)
{

	if(photo_size == NULL) {

		return;
	}

	photo_size->_is_used = NXS_NO;

	photo_size->width     = 0;
	photo_size->height    = 0;
	photo_size->file_size = 0;

	nxs_string_init_empty(&photo_size->file_id);
}

void nxs_chat_srv_c_tlgrm_photo_size_free(nxs_chat_srv_m_tlgrm_photo_size_t *photo_size)
{

	if(photo_size == NULL) {

		return;
	}

	photo_size->_is_used = NXS_NO;

	photo_size->width     = 0;
	photo_size->height    = 0;
	photo_size->file_size = 0;

	nxs_string_free(&photo_size->file_id);
}

void nxs_chat_srv_c_tlgrm_document_init(nxs_chat_srv_m_tlgrm_document_t *document)
{

	if(document == NULL) {

		return;
	}

	document->_is_used = NXS_NO;

	document->file_size = 0;

	nxs_chat_srv_c_tlgrm_photo_size_init(&document->thumb);

	nxs_string_init_empty(&document->file_id);
	nxs_string_init_empty(&document->file_name);
	nxs_string_init_empty(&document->mime_type);
}

void nxs_chat_srv_c_tlgrm_document_free(nxs_chat_srv_m_tlgrm_document_t *document)
{

	if(document == NULL) {

		return;
	}

	document->_is_used = NXS_NO;

	document->file_size = 0;

	nxs_chat_srv_c_tlgrm_photo_size_free(&document->thumb);

	nxs_string_free(&document->file_id);
	nxs_string_free(&document->file_name);
	nxs_string_free(&document->mime_type);
}

void nxs_chat_srv_c_tlgrm_sticker_init(nxs_chat_srv_m_tlgrm_sticker_t *sticker)
{

	if(sticker == NULL) {

		return;
	}

	sticker->_is_used = NXS_NO;

	sticker->file_size = 0;
	sticker->width     = 0;
	sticker->height    = 0;

	nxs_chat_srv_c_tlgrm_photo_size_init(&sticker->thumb);

	nxs_string_init_empty(&sticker->file_id);
	nxs_string_init_empty(&sticker->emoji);
	nxs_string_init_empty(&sticker->set_name);
}

void nxs_chat_srv_c_tlgrm_sticker_free(nxs_chat_srv_m_tlgrm_sticker_t *sticker)
{

	if(sticker == NULL) {

		return;
	}

	sticker->_is_used = NXS_NO;

	sticker->file_size = 0;
	sticker->width     = 0;
	sticker->height    = 0;

	nxs_chat_srv_c_tlgrm_photo_size_free(&sticker->thumb);

	nxs_string_free(&sticker->file_id);
	nxs_string_free(&sticker->emoji);
	nxs_string_free(&sticker->set_name);
}

void nxs_chat_srv_c_tlgrm_message_init(nxs_chat_srv_m_tlgrm_message_t *message)
{

	if(message == NULL) {

		return;
	}

	message->_is_used   = NXS_NO;
	message->message_id = 0;

	nxs_string_init_empty(&message->text);
	nxs_string_init_empty(&message->caption);

	nxs_chat_srv_c_tlgrm_user_init(&message->from);
	nxs_chat_srv_c_tlgrm_chat_init(&message->chat);
	nxs_chat_srv_c_tlgrm_document_init(&message->document);
	nxs_chat_srv_c_tlgrm_sticker_init(&message->sticker);

	message->reply_to_message = NULL;

	nxs_array_init2(&message->entities, nxs_chat_srv_m_tlgrm_message_entity_t);
	nxs_array_init2(&message->photo, nxs_chat_srv_m_tlgrm_photo_size_t);
}

void nxs_chat_srv_c_tlgrm_message_free(nxs_chat_srv_m_tlgrm_message_t *message)
{
	nxs_chat_srv_m_tlgrm_message_entity_t *e;
	nxs_chat_srv_m_tlgrm_photo_size_t *    p;
	size_t                                 i;

	if(message == NULL) {

		return;
	}

	message->_is_used   = NXS_NO;
	message->message_id = 0;

	nxs_string_free(&message->text);
	nxs_string_free(&message->caption);

	nxs_chat_srv_c_tlgrm_user_free(&message->from);
	nxs_chat_srv_c_tlgrm_chat_free(&message->chat);
	nxs_chat_srv_c_tlgrm_document_free(&message->document);
	nxs_chat_srv_c_tlgrm_sticker_free(&message->sticker);

	if(message->reply_to_message != NULL) {

		message->reply_to_message = nxs_chat_srv_c_tlgrm_message_reply_destroy(message->reply_to_message);
	}

	for(i = 0; i < nxs_array_count(&message->entities); i++) {

		e = nxs_array_get(&message->entities, i);

		nxs_chat_srv_c_tlgrm_message_entity_free(e);
	}

	for(i = 0; i < nxs_array_count(&message->photo); i++) {

		p = nxs_array_get(&message->photo, i);

		nxs_chat_srv_c_tlgrm_photo_size_free(p);
	}

	nxs_array_free(&message->entities);
	nxs_array_free(&message->photo);
}

nxs_chat_srv_err_t
        nxs_chat_srv_c_tlgrm_message_result_pull_json(nxs_chat_srv_m_tlgrm_message_t *message, nxs_bool_t *status, nxs_buf_t *json_buf)
{

	if(message == NULL || status == NULL || json_buf == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	return nxs_chat_srv_c_tlgrm_message_result_pull_json_extract(message, status, json_buf);
}

nxs_chat_srv_m_tlgrm_message_t *nxs_chat_srv_c_tlgrm_message_reply_alloc(void)
{
	nxs_chat_srv_m_tlgrm_message_t *p;

	p = (nxs_chat_srv_m_tlgrm_message_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_m_tlgrm_message_t));

	nxs_chat_srv_c_tlgrm_message_init(p);

	return p;
}

nxs_chat_srv_m_tlgrm_message_t *nxs_chat_srv_c_tlgrm_message_reply_destroy(nxs_chat_srv_m_tlgrm_message_t *message)
{

	if(message == NULL) {

		return NULL;
	}

	nxs_chat_srv_c_tlgrm_message_free(message);

	return nxs_free(message);
}

void nxs_chat_srv_c_tlgrm_chat_init(nxs_chat_srv_m_tlgrm_chat_t *chat)
{

	if(chat == NULL) {

		return;
	}

	chat->_is_used = NXS_NO;
	chat->id       = 0;
	chat->type     = NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_NONE;

	nxs_string_init_empty(&chat->username);
}

void nxs_chat_srv_c_tlgrm_chat_free(nxs_chat_srv_m_tlgrm_chat_t *chat)
{

	if(chat == NULL) {

		return;
	}

	chat->_is_used = NXS_NO;
	chat->id       = 0;
	chat->type     = NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_NONE;

	nxs_string_free(&chat->username);
}

nxs_chat_srv_m_tlgrm_chat_type_t nxs_chat_srv_c_tlgrm_chat_type_map(nxs_string_t *type_name)
{
	size_t i;

	for(i = 0; chat_types[i].type != NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_NONE; i++) {

		if(nxs_string_cmp(&chat_types[i].name, 0, type_name, 0) == NXS_YES) {

			return chat_types[i].type;
		}
	}

	return NXS_CHAT_SRV_M_TLGRM_CHAT_TYPE_NONE;
}

nxs_string_t *nxs_chat_srv_c_tlgrm_parse_mode_map(nxs_chat_srv_m_tlgrm_parse_mode_t mode)
{
	size_t i;

	for(i = 0; parse_mode[i].parse_mode != NXS_CHAT_SRV_M_TLGRM_PARSE_MODE_TYPE_NONE; i++) {

		if(parse_mode[i].parse_mode == mode) {

			return &parse_mode[i].name;
		}
	}

	return &parse_mode[i].name;
}

void nxs_chat_srv_c_tlgrm_user_init(nxs_chat_srv_m_tlgrm_user_t *user)
{

	if(user == NULL) {

		return;
	}

	user->_is_used = NXS_NO;

	user->id = 0;

	nxs_string_init_empty(&user->first_name);
	nxs_string_init_empty(&user->last_name);
	nxs_string_init_empty(&user->username);
	nxs_string_init_empty(&user->language_code);
}

void nxs_chat_srv_c_tlgrm_user_free(nxs_chat_srv_m_tlgrm_user_t *user)
{

	if(user == NULL) {

		return;
	}

	user->_is_used = NXS_NO;

	user->id = 0;

	nxs_string_free(&user->first_name);
	nxs_string_free(&user->last_name);
	nxs_string_free(&user->username);
	nxs_string_free(&user->language_code);
}

void nxs_chat_srv_c_tlgrm_file_init(nxs_chat_srv_m_tlgrm_file_t *file)
{

	if(file == NULL) {

		return;
	}

	file->_is_used = NXS_NO;

	file->file_size = 0;

	nxs_string_init_empty(&file->file_id);
	nxs_string_init_empty(&file->file_path);
}

void nxs_chat_srv_c_tlgrm_file_free(nxs_chat_srv_m_tlgrm_file_t *file)
{

	if(file == NULL) {

		return;
	}

	file->_is_used = NXS_NO;

	file->file_size = 0;

	nxs_string_free(&file->file_id);
	nxs_string_free(&file->file_path);
}

nxs_chat_srv_err_t nxs_chat_srv_c_tlgrm_file_result_pull_json(nxs_chat_srv_m_tlgrm_file_t *file, nxs_bool_t *status, nxs_buf_t *json_buf)
{

	if(file == NULL || status == NULL || json_buf == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	return nxs_chat_srv_c_tlgrm_file_result_pull_json_extract(file, status, json_buf);
}

void nxs_chat_srv_c_tlgrm_inl_keyboard_init(nxs_chat_srv_m_tlgrm_inl_keyboard_t *inl_keyboard)
{

	if(inl_keyboard == NULL) {

		return;
	}

	inl_keyboard->_is_used = NXS_NO;

	nxs_array_init2(&inl_keyboard->inline_keyboard, nxs_array_t);
}

void nxs_chat_srv_c_tlgrm_inl_keyboard_free(nxs_chat_srv_m_tlgrm_inl_keyboard_t *inl_keyboard)
{
	nxs_chat_srv_m_tlgrm_inl_keybutton_t *ikm;
	nxs_array_t *                         a;
	size_t                                i, j;

	if(inl_keyboard == NULL) {

		return;
	}

	inl_keyboard->_is_used = NXS_NO;

	for(i = 0; i < nxs_array_count(&inl_keyboard->inline_keyboard); i++) {

		a = nxs_array_get(&inl_keyboard->inline_keyboard, i);

		for(j = 0; j < nxs_array_count(a); j++) {

			ikm = nxs_array_get(a, j);

			nxs_chat_srv_c_tlgrm_inl_keybutton_free(ikm);
		}

		nxs_array_free(a);
	}

	nxs_array_free(&inl_keyboard->inline_keyboard);
}

nxs_chat_srv_err_t nxs_chat_srv_c_tlgrm_inl_keyboard_add_button(nxs_chat_srv_m_tlgrm_inl_keyboard_t *inl_keyboard,
                                                                size_t                               pos_y,
                                                                size_t                               pos_x,
                                                                nxs_string_t *                       text,
                                                                nxs_string_t *                       url,
                                                                nxs_string_t *                       callback_data)
{
	nxs_chat_srv_m_tlgrm_inl_keybutton_t *ikm, *k;
	nxs_array_t *                         a, *b;
	size_t                                i, c;

	if(inl_keyboard == NULL || text == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((url == NULL && callback_data == NULL) || (url != NULL && callback_data != NULL)) {

		/* see: https://core.telegram.org/bots/api#inlinekeyboardbutton
		 * "You must use exactly one of the optional fields." */

		return NXS_CHAT_SRV_E_ERR;
	}

	inl_keyboard->_is_used = NXS_YES;

	c = nxs_array_count(&inl_keyboard->inline_keyboard);
	a = nxs_array_add_i(&inl_keyboard->inline_keyboard, pos_y);

	/* initialized all allocated elements */
	for(i = c; i < nxs_array_count(&inl_keyboard->inline_keyboard); i++) {

		b = nxs_array_get(&inl_keyboard->inline_keyboard, i);

		nxs_array_init2(b, nxs_chat_srv_m_tlgrm_inl_keybutton_t);
	}

	c   = nxs_array_count(a);
	ikm = nxs_array_add_i(a, pos_x);

	/* initialized all allocated elements */
	for(i = c; i < nxs_array_count(a); i++) {

		k = nxs_array_get(a, i);

		nxs_chat_srv_c_tlgrm_inl_keybutton_init(k);
	}

	/* fill element (pos_y, pos_x) */

	ikm->_is_used = NXS_YES;

	nxs_string_clone(&ikm->text, text);

	if(url != NULL) {

		nxs_string_clone(&ikm->url, url);
	}

	if(callback_data != NULL) {

		nxs_string_clone(&ikm->callback_data, callback_data);
	}

	return NXS_CHAT_SRV_E_OK;
}

void nxs_chat_srv_c_tlgrm_inl_keyboard_serialize(nxs_chat_srv_m_tlgrm_inl_keyboard_t *inl_keyboard, nxs_string_t *out_str)
{
	nxs_chat_srv_m_tlgrm_inl_keybutton_t *ikm;
	nxs_array_t *                         a;
	nxs_string_t                          p, text_serialized, url_serialized, callback_data_serialized;
	nxs_bool_t                            f;
	size_t                                i, j;

	if(inl_keyboard == NULL || out_str == NULL) {

		return;
	}

	nxs_string_clear(out_str);

	if(inl_keyboard->_is_used == NXS_NO) {

		return;
	}

	nxs_string_init(&p);
	nxs_string_init(&text_serialized);
	nxs_string_init(&url_serialized);
	nxs_string_init(&callback_data_serialized);

	for(i = 0; i < nxs_array_count(&inl_keyboard->inline_keyboard); i++) {

		a = nxs_array_get(&inl_keyboard->inline_keyboard, i);

		if(i > 0) {

			nxs_string_char_add_char(&p, (u_char)',');
		}

		nxs_string_printf2_cat(&p, "[");

		for(f = NXS_NO, j = 0; j < nxs_array_count(a); j++) {

			ikm = nxs_array_get(a, j);

			if(ikm->_is_used == NXS_YES) {

				if(f == NXS_YES) {

					nxs_string_char_add_char(&p, (u_char)',');
				}

				f = NXS_YES;

				nxs_string_escape(&text_serialized, &ikm->text, NXS_STRING_ESCAPE_TYPE_JSON);

				if(nxs_string_len(&ikm->url) > 0) {

					nxs_string_escape(&url_serialized, &ikm->url, NXS_STRING_ESCAPE_TYPE_JSON);

					nxs_string_printf2_cat(&p, "{\"text\":\"%r\",\"url\":\"%r\"}", &text_serialized, &url_serialized);
				}
				else {

					nxs_string_escape(&callback_data_serialized, &ikm->callback_data, NXS_STRING_ESCAPE_TYPE_JSON);

					nxs_string_printf2_cat(&p,
					                       "{\"text\":\"%r\",\"callback_data\":\"%r\"}",
					                       &text_serialized,
					                       &callback_data_serialized);
				}
			}
		}

		nxs_string_printf2_cat(&p, "]");
	}

	nxs_string_printf2_cat(out_str, ",\"reply_markup\": {\"inline_keyboard\":[%r]}", &p);

	nxs_string_free(&p);
	nxs_string_free(&text_serialized);
	nxs_string_free(&url_serialized);
	nxs_string_free(&callback_data_serialized);
}

void nxs_chat_srv_c_tlgrm_inl_keybutton_init(nxs_chat_srv_m_tlgrm_inl_keybutton_t *inl_keybutton)
{

	if(inl_keybutton == NULL) {

		return;
	}

	inl_keybutton->_is_used = NXS_NO;

	nxs_string_init_empty(&inl_keybutton->text);
	nxs_string_init_empty(&inl_keybutton->url);
	nxs_string_init_empty(&inl_keybutton->callback_data);
}

void nxs_chat_srv_c_tlgrm_inl_keybutton_free(nxs_chat_srv_m_tlgrm_inl_keybutton_t *inl_keybutton)
{

	if(inl_keybutton == NULL) {

		return;
	}

	inl_keybutton->_is_used = NXS_NO;

	nxs_string_free(&inl_keybutton->text);
	nxs_string_free(&inl_keybutton->url);
	nxs_string_free(&inl_keybutton->callback_data);
}

void nxs_chat_srv_c_tlgrm_force_reply_init(nxs_chat_srv_m_tlgrm_force_reply_t *force_reply)
{

	if(force_reply == NULL) {

		return;
	}

	force_reply->_is_used    = NXS_NO;
	force_reply->force_reply = NXS_NO;
}

void nxs_chat_srv_c_tlgrm_force_reply_free(nxs_chat_srv_m_tlgrm_force_reply_t *force_reply)
{

	if(force_reply == NULL) {

		return;
	}

	force_reply->_is_used    = NXS_NO;
	force_reply->force_reply = NXS_NO;
}

void nxs_chat_srv_c_tlgrm_force_reply_set(nxs_chat_srv_m_tlgrm_force_reply_t *force_reply)
{

	if(force_reply == NULL) {

		return;
	}

	force_reply->_is_used    = NXS_YES;
	force_reply->force_reply = NXS_YES;
}

void nxs_chat_srv_c_tlgrm_force_reply_serialize(nxs_chat_srv_m_tlgrm_force_reply_t *force_reply, nxs_string_t *out_str)
{

	if(force_reply == NULL || out_str == NULL) {

		return;
	}

	nxs_string_clear(out_str);

	if(force_reply->_is_used && force_reply->force_reply == NXS_YES) {

		nxs_string_printf2_cat(out_str, ",\"reply_markup\": {\"force_reply\":true}");
	}
}

void nxs_chat_srv_c_tlgrm_bttn_callback_serialize(nxs_chat_srv_m_tlgrm_bttn_callback_t callback, nxs_string_t *callback_str)
{

	if(callback_str == NULL) {

		return;
	}

	nxs_string_printf(callback_str, "{\"%r\":%d,\"%r\":%zu}", &_s_par_t, callback.type, &_s_par_o_id, callback.object_id);
}

nxs_chat_srv_err_t nxs_chat_srv_c_tlgrm_bttn_callback_deserialize(nxs_chat_srv_m_tlgrm_bttn_callback_t *callback,
                                                                  nxs_string_t *                        callback_str)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	callback->type      = NXS_CHAT_SRV_M_TLGRM_BTTN_CALLBACK_TYPE_NONE;
	callback->object_id = 0;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_t,	&callback->type,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_o_id,	&callback->object_id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, (nxs_buf_t *)callback_str) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(
		        &process, "[%s]: tlgrm update rest api error: parse callback_bttn data error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

/* Module internal (static) functions */

static nxs_chat_srv_err_t nxs_chat_srv_c_tlgrm_update_pull_json_extract(nxs_chat_srv_m_tlgrm_update_t *update, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	update->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_update_id,	&update->update_id,		NULL,							NULL,	NXS_CFG_JSON_TYPE_INT,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_message,	&update->message,		&nxs_chat_srv_c_tlgrm_extract_json_message,		NULL,	NXS_CFG_JSON_TYPE_VOID,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_callback_query,	&update->callback_query,	&nxs_chat_srv_c_tlgrm_extract_json_callback_query,	NULL,	NXS_CFG_JSON_TYPE_VOID,	0,	0,	NXS_NO,		NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: tlgrm update pull rest api error: parse json_buf error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_chat_srv_err_t nxs_chat_srv_c_tlgrm_message_result_pull_json_extract(nxs_chat_srv_m_tlgrm_message_t *message,
                                                                                nxs_bool_t *                    status,
                                                                                nxs_buf_t *                     json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_result,		message,	&nxs_chat_srv_c_tlgrm_extract_json_message,	NULL,	NXS_CFG_JSON_TYPE_VOID,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_ok,		status,		NULL,						NULL,	NXS_CFG_JSON_TYPE_BOOL,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(
		        &process, "[%s]: tlgrm message result json pull error: parse json_buf error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_chat_srv_err_t
        nxs_chat_srv_c_tlgrm_file_result_pull_json_extract(nxs_chat_srv_m_tlgrm_file_t *file, nxs_bool_t *status, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_result,		file,		&nxs_chat_srv_c_tlgrm_extract_json_file,	NULL,	NXS_CFG_JSON_TYPE_VOID,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_ok,		status,		NULL,						NULL,	NXS_CFG_JSON_TYPE_BOOL,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: tlgrm file result json pull error: parse json_buf error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_message(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_tlgrm_message_t *var = nxs_cfg_json_get_val(cfg_json_par_el), *m;
	nxs_cfg_json_t                  cfg_json;
	nxs_array_t                     cfg_arr;
	nxs_cfg_json_state_t            rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	m = nxs_chat_srv_c_tlgrm_message_reply_alloc();

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_message_id,		&var->message_id,	NULL,						NULL,						NXS_CFG_JSON_TYPE_INT,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_from,			&var->from,		&nxs_chat_srv_c_tlgrm_extract_json_user,	NULL,						NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_chat,			&var->chat,		&nxs_chat_srv_c_tlgrm_extract_json_chat,	NULL,						NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_text,			&var->text,		NULL,						NULL,						NXS_CFG_JSON_TYPE_STRING,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_caption,		&var->caption,		NULL,						NULL,						NXS_CFG_JSON_TYPE_STRING,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_reply_to_message,	m,			&nxs_chat_srv_c_tlgrm_extract_json_message,	NULL,						NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_entities,		&var->entities,		NULL,						&nxs_chat_srv_c_tlgrm_extract_json_entity,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_photo,			&var->photo,		NULL,						&nxs_chat_srv_c_tlgrm_extract_json_photo_sizes,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_document,		&var->document,		&nxs_chat_srv_c_tlgrm_extract_json_document,	NULL,						NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_sticker,		&var->sticker,		&nxs_chat_srv_c_tlgrm_extract_json_sticker,	NULL,						NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_NO,		NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "json read error: 'message' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

	if(m->_is_used == NXS_YES) {

		var->reply_to_message = m;

		m = NULL;
	}

error:

	m = nxs_chat_srv_c_tlgrm_message_reply_destroy(m);

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_callback_query(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_tlgrm_callback_query_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                         cfg_json;
	nxs_array_t                            cfg_arr;
	nxs_cfg_json_state_t                   rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&var->id,		NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_from,		&var->from,		&nxs_chat_srv_c_tlgrm_extract_json_user,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_message,	&var->message,		&nxs_chat_srv_c_tlgrm_extract_json_message,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_chat_instance,	&var->chat_instance,	NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_data,		&var->data,		NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "json read error: 'callback_query' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_chat(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_tlgrm_chat_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t               cfg_json;
	nxs_array_t                  cfg_arr;
	nxs_string_t                 chat_type;
	nxs_cfg_json_state_t         rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_string_init(&chat_type);

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&var->id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_username,	&var->username,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_type,		&chat_type,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "json read error: 'message.chat' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

	var->type = nxs_chat_srv_c_tlgrm_chat_type_map(&chat_type);

error:

	nxs_string_free(&chat_type);

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_tlgrm_user_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t               cfg_json;
	nxs_array_t                  cfg_arr;
	nxs_cfg_json_state_t         rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,			&var->id,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_first_name,		&var->first_name,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_last_name,		&var->last_name,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_username,		&var->username,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_language_code,		&var->language_code,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "json read error: 'message' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_c_tlgrm_extract_json_entity(nxs_process_t *     proc,
                                                                     nxs_json_t *        json,
                                                                     nxs_cfg_json_par_t *cfg_json_par_el,
                                                                     nxs_array_t *       cfg_arr)
{
	nxs_array_t *                          entities = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_m_tlgrm_message_entity_t *e;

	e = nxs_array_add(entities);

	nxs_chat_srv_c_tlgrm_message_entity_init(e);

	e->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_skip_undef(cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_type,		&e->type,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_offset,		&e->offset,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_length,		&e->length,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_url,		&e->url,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t nxs_chat_srv_c_tlgrm_extract_json_photo_sizes(nxs_process_t *     proc,
                                                                          nxs_json_t *        json,
                                                                          nxs_cfg_json_par_t *cfg_json_par_el,
                                                                          nxs_array_t *       cfg_arr)
{
	nxs_array_t *                      photo_sizes = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_m_tlgrm_photo_size_t *p;

	p = nxs_array_add(photo_sizes);

	nxs_chat_srv_c_tlgrm_photo_size_init(p);

	p->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_skip_undef(cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_file_id,	&p->file_id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_width,		&p->width,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_height,		&p->height,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_file_size,	&p->file_size,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_NO,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_photo_size(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_tlgrm_photo_size_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                     cfg_json;
	nxs_array_t                        cfg_arr;
	nxs_cfg_json_state_t               rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_file_id,	&var->file_id,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_width,		&var->width,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_height,		&var->height,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_file_size,	&var->file_size,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_NO,		NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "json read error: 'photo_size' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_file(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_tlgrm_file_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t               cfg_json;
	nxs_array_t                  cfg_arr;
	nxs_cfg_json_state_t         rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_file_id,	&var->file_id,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_file_size,	&var->file_size,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_file_path,	&var->file_path,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "json read error: 'file' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_document(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_tlgrm_document_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                   cfg_json;
	nxs_array_t                      cfg_arr;
	nxs_string_t                     chat_type;
	nxs_cfg_json_state_t             rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_string_init(&chat_type);

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_file_id,	&var->file_id,		NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_thumb,		&var->thumb,		&nxs_chat_srv_c_tlgrm_extract_json_photo_size,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_file_name,	&var->file_name,	NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_mime_type,	&var->mime_type,	NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_file_size,	&var->file_size,	NULL,						NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_NO,		NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "json read error: 'document' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_string_free(&chat_type);

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_tlgrm_extract_json_sticker(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_tlgrm_sticker_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                  cfg_json;
	nxs_array_t                     cfg_arr;
	nxs_string_t                    chat_type;
	nxs_cfg_json_state_t            rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_string_init(&chat_type);

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_file_id,	&var->file_id,		NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_width,		&var->width,		NULL,						NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_height,		&var->height,		NULL,						NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_thumb,		&var->thumb,		&nxs_chat_srv_c_tlgrm_extract_json_photo_size,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_emoji,		&var->emoji,		NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_set_name,	&var->set_name,		NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_file_size,	&var->file_size,	NULL,						NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_NO,		NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "json read error: 'sticker' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_string_free(&chat_type);

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}
