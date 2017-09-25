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

static u_char cmd_private[]	= {NXS_CHAT_SRV_UTF8_PRIVATE_MESSAGE};

/* Module global functions */

// clang-format on

nxs_bool_t nxs_chat_srv_c_notes_check_private(nxs_string_t *note_src, nxs_string_t *note_dst)
{
	nxs_bool_t rc;

	if(note_src == NULL) {

		return NXS_NO;
	}

	rc = NXS_NO;

	if(nxs_string_char_ncmp(note_src, 0, cmd_private, strlen((char *)cmd_private)) == NXS_YES) {

		/* if message begin with '/private' command */

		rc = NXS_YES;

		if(note_dst != NULL) {

			nxs_string_cpy(note_dst, 0, note_src, strlen((char *)cmd_private));
		}
	}
	else {

		if(note_dst != NULL) {

			nxs_string_clone(note_dst, note_src);
		}
	}

	return rc;
}

/* Module internal (static) functions */
