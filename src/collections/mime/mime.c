#ifndef _INCLUDE_NXS_CHAT_SRV_C_MIME_C
#define _INCLUDE_NXS_CHAT_SRV_C_MIME_C
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
	nxs_string_t		ext_name;
	nxs_string_t		content_type;
} nxs_chat_srv_c_mime_types_t;

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */

static nxs_string_t _s_mime_default		= nxs_string("application/octet-stream");

nxs_chat_srv_c_mime_types_t mimes[] =
{
	{nxs_string("jpeg"),			nxs_string("image/jpeg")},
	{nxs_string("jpg"),			nxs_string("image/jpeg")},
	{nxs_string("png"),			nxs_string("image/png")},
	{nxs_string("webp"),			nxs_string("image/webp")},
	{nxs_string("mp4"),			nxs_string("video/mp4")},
	{nxs_string("ogg"),			nxs_string("audio/ogg")},

	{NXS_STRING_NULL_STR,			NXS_STRING_NULL_STR},
};

/* Module global functions */

// clang-format on

nxs_string_t *nxs_chat_srv_c_mime_get(nxs_string_t *file_name)
{
	size_t  i;
	u_char *c;

	if((c = nxs_chat_srv_c_mime_get_file_extension(file_name)) == NULL) {

		return &_s_mime_default;
	}

	for(i = 0; nxs_string_len(&mimes[i].ext_name) > 0; i++) {

		if(nxs_string_char_cmp(&mimes[i].ext_name, 0, c) == NXS_YES) {

			return &mimes[i].content_type;
		}
	}

	return &_s_mime_default;
}

u_char *nxs_chat_srv_c_mime_get_file_extension(nxs_string_t *file_name)
{
	u_char *c;

	if(file_name == NULL || nxs_string_len(file_name) == 0) {

		return NULL;
	}

	if((c = nxs_string_find_char_last(file_name, 0, (u_char)'.')) == NULL) {

		return NULL;
	}

	c++;

	if((size_t)(c - nxs_string_str(file_name)) >= nxs_string_len(file_name)) {

		return NULL;
	}

	return c;
}

void nxs_chat_srv_c_mime_add_file_extension(nxs_string_t *file_name_dst, nxs_string_t *file_name_src, nxs_string_t *mime_type)
{
	u_char *c;
	size_t  i;
	ssize_t mime_index;

	if(file_name_dst == NULL || nxs_string_len(file_name_src) == 0 || nxs_string_len(mime_type) == 0) {

		return;
	}

	for(mime_index = -1, i = 0; nxs_string_len(&mimes[i].content_type) > 0; i++) {

		if(nxs_string_cmp(&mimes[i].content_type, 0, mime_type, 0) == NXS_YES) {

			/* mime type matched */

			if(mime_index == -1) {

				mime_index = i;
			}

			if((c = nxs_chat_srv_c_mime_get_file_extension(file_name_src)) != NULL &&
			   nxs_string_char_cmp(&mimes[i].ext_name, 0, c) == NXS_YES) {

				/* file extansion matched */

				nxs_string_cpy(file_name_dst, 0, file_name_src, 0);

				return;
			}
		}
	}

	if(mime_index == -1) {

		/* mime not found */

		nxs_string_cpy(file_name_dst, 0, file_name_src, 0);

		return;
	}

	/* file has no extension or if file extension and mime type do not match */

	nxs_string_printf(file_name_dst, "%r.%r", file_name_src, &mimes[mime_index].ext_name);
}

/* Module internal (static) functions */
