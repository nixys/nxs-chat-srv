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
extern		nxs_process_t				process;
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
	{nxs_string("pdf"),			nxs_string("application/pdf")},

	{NXS_STRING_NULL_STR,			NXS_STRING_NULL_STR},
};

/* Module global functions */

// clang-format on

nxs_string_t *nxs_chat_srv_c_mime_get(nxs_string_t *file_name)
{
	size_t  i;
	u_char *c;

	if(file_name == NULL || nxs_string_len(file_name) == 0) {

		return &_s_mime_default;
	}

	if((c = nxs_string_find_char_last(file_name, 0, (u_char)'.')) == NULL) {

		return &_s_mime_default;
	}

	c++;

	if((size_t)(c - nxs_string_str(file_name)) >= nxs_string_len(file_name)) {

		return &_s_mime_default;
	}

	for(i = 0; nxs_string_len(&mimes[i].ext_name) > 0; i++) {

		if(nxs_string_char_cmp(&mimes[i].ext_name, 0, c) == NXS_YES) {

			return &mimes[i].content_type;
		}
	}

	return &_s_mime_default;
}

/* Module internal (static) functions */
