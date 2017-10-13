#ifndef _INCLUDE_NXS_CHAT_SRV_C_NOTES_C
#define _INCLUDE_NXS_CHAT_SRV_C_NOTES_C
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



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */

static u_char	cmd_private[]	= {NXS_CHAT_SRV_UTF8_PRIVATE_MESSAGE};
static u_char	cmd_ext[]	= "/ext";

/* Module global functions */

// clang-format on

/*
 * if note_dst == NULL		- only check note for 'private'
 * if note_dst != NULL		- check note for 'private' and write modified string to note_dst
 *
 * note_dst and note_src may be the same string
 */
nxs_bool_t nxs_chat_srv_c_notes_check_private(nxs_string_t *note_src, nxs_string_t *note_dst)
{
	nxs_string_t tmp_str;
	nxs_bool_t   rc;
	size_t       l;

	if(note_src == NULL) {

		return NXS_NO;
	}

	rc = NXS_NO;

	l = strlen((char *)cmd_private);

	if(nxs_string_char_ncmp(note_src, 0, cmd_private, l) == NXS_YES) {

		/* if message begin with '/private' command */

		rc = NXS_YES;
	}

	if(note_dst != NULL) {

		if(rc == NXS_YES) {

			if(note_dst == note_src) {

				nxs_string_init(&tmp_str);

				nxs_string_cpy(&tmp_str, 0, note_src, l);

				nxs_string_clone(note_src, &tmp_str);

				nxs_string_free(&tmp_str);
			}
			else {

				nxs_string_cpy(note_dst, 0, note_src, l);
			}
		}
		else {

			if(note_dst != note_src) {

				nxs_string_clone(note_dst, note_src);
			}
		}
	}

	return rc;
}

/*
 * if note_dst == NULL		- only check note for 'extended'
 * if note_dst != NULL		- check note for 'extended' and write modified string to note_dst
 *
 * note_dst and note_src may be the same string
 */
nxs_bool_t nxs_chat_srv_c_notes_check_ext(nxs_string_t *note_src, nxs_string_t *note_dst)
{
	nxs_string_t tmp_str;
	nxs_bool_t   rc;
	size_t       l;

	if(note_src == NULL) {

		return NXS_NO;
	}

	rc = NXS_NO;

	l = strlen((char *)cmd_ext);

	if(nxs_string_char_ncmp(note_src, 0, cmd_ext, l) == NXS_YES) {

		if(nxs_string_len(note_src) == l) {

			nxs_error(rc, NXS_YES, error);
		}

		if(nxs_string_len(note_src) > l) {

			switch(nxs_string_get_char(note_src, l)) {

				case(u_char)' ':
				case(u_char)'\r':
				case(u_char)'\n':
				case(u_char)'\t':

					nxs_error(rc, NXS_YES, error);
			}
		}
	}

error:

	if(note_dst != NULL) {

		if(rc == NXS_YES) {

			if(note_dst == note_src) {

				nxs_string_init(&tmp_str);

				nxs_string_cpy(&tmp_str, 0, note_src, l);

				nxs_string_clone(note_src, &tmp_str);

				nxs_string_free(&tmp_str);
			}
			else {

				nxs_string_cpy(note_dst, 0, note_src, l);
			}
		}
		else {

			if(note_dst != note_src) {

				nxs_string_clone(note_dst, note_src);
			}
		}
	}

	return rc;
}

/* Module internal (static) functions */
