#ifndef _INCLUDE_NXS_CHAT_SRV_U_RDMN_ATTACHMENTS_C
#define _INCLUDE_NXS_CHAT_SRV_U_RDMN_ATTACHMENTS_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/rdmn-attachments/rdmn-attachments-typedefs.h>
#include <units/rdmn-attachments/rdmn-attachments.h>
#include <units/rdmn-attachments/rdmn-attachments-subunits.h>

/* Definitions */

#define DOWNLOAD_TMP_DIR_MODE		0700

/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	size_t			attachment_id;
	nxs_string_t		file_name;
	nxs_string_t		file_path;
	nxs_string_t		description;
} nxs_chat_srv_u_rdmn_attachments_downloads_t;

struct nxs_chat_srv_u_rdmn_attachments_s
{
	nxs_buf_t		response_buf;
	nxs_array_t		downloads;		/* type: nxs_chat_srv_u_rdmn_attachments_downloads_t */
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */

static nxs_string_t _s_proto_http = nxs_string("http://");
static nxs_string_t _s_proto_https = nxs_string("https://");

/* Module global functions */

// clang-format on

nxs_chat_srv_u_rdmn_attachments_t *nxs_chat_srv_u_rdmn_attachments_init(void)
{
	nxs_chat_srv_u_rdmn_attachments_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_rdmn_attachments_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_rdmn_attachments_t));

	nxs_buf_init2(&u_ctx->response_buf);

	nxs_array_init2(&u_ctx->downloads, nxs_chat_srv_u_rdmn_attachments_downloads_t);

	return u_ctx;
}

nxs_chat_srv_u_rdmn_attachments_t *nxs_chat_srv_u_rdmn_attachments_free(nxs_chat_srv_u_rdmn_attachments_t *u_ctx)
{
	nxs_chat_srv_u_rdmn_attachments_downloads_t *d;
	nxs_string_t                                 tmp_dir;
	size_t                                       i;

	if(u_ctx == NULL) {

		return NULL;
	}

	nxs_string_init(&tmp_dir);

	for(i = 0; i < nxs_array_count(&u_ctx->downloads); i++) {

		d = nxs_array_get(&u_ctx->downloads, i);

		nxs_string_printf(&tmp_dir, "%r/%zu", &nxs_chat_srv_cfg.attachments.rdmn_download_tmp_dir, d->attachment_id);

		nxs_fs_unlink(&d->file_path);
		nxs_fs_rmdir(&tmp_dir);

		d->attachment_id = 0;

		nxs_string_free(&d->file_name);
		nxs_string_free(&d->file_path);
		nxs_string_free(&d->description);
	}

	nxs_array_free(&u_ctx->downloads);

	nxs_buf_free(&u_ctx->response_buf);

	nxs_string_free(&tmp_dir);

	return (nxs_chat_srv_u_rdmn_attachments_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_attachments_download(nxs_chat_srv_u_rdmn_attachments_t *u_ctx,
                                                            size_t                             attachment_id,
                                                            nxs_string_t *                     file_name,
                                                            nxs_string_t *                     file_path,
                                                            nxs_string_t *                     description)
{
	nxs_chat_srv_m_rdmn_attachment_t             attachment;
	nxs_chat_srv_u_rdmn_attachments_downloads_t *d;
	nxs_string_t                                 content_url;
	nxs_chat_srv_err_t                           rc;
	size_t                                       i;
	u_char *                                     c;

	if(u_ctx == NULL || file_path == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	for(i = 0; i < nxs_array_count(&u_ctx->downloads); i++) {

		d = nxs_array_get(&u_ctx->downloads, i);

		if(d->attachment_id == attachment_id) {

			/* if file already has been downloaded */

			nxs_string_clone(file_name, &d->file_name);
			nxs_string_clone(file_path, &d->file_path);
			nxs_string_clone(description, &d->description);

			return NXS_CHAT_SRV_E_OK;
		}
	}

	/* if file not downloaded yet */

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&content_url);

	nxs_chat_srv_c_rdmn_attachment_init(&attachment);

	if(nxs_chat_srv_d_rdmn_attachments_info(attachment_id, NULL, &u_ctx->response_buf) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: can't get file download link from rdmn (attachment id: \"%r\")",
		                    nxs_proc_get_name(&process),
		                    attachment_id);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_c_rdmn_attachment_pull_json(&attachment, &u_ctx->response_buf) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_string_clone(file_name, &attachment.filename);
	nxs_string_clone(description, &attachment.description);

	if((c = nxs_string_find_substr_first(&attachment.content_url, 0, nxs_string_str(&_s_proto_http), nxs_string_len(&_s_proto_http))) ==
	   NULL) {

		nxs_string_clone(&content_url, &attachment.content_url);
	}
	else {

		nxs_string_printf(&content_url, "%r%s", &_s_proto_https, c + nxs_string_len(&_s_proto_http));
	}

	nxs_string_printf(file_path, "%r/%zu", &nxs_chat_srv_cfg.attachments.rdmn_download_tmp_dir, attachment_id);

	if(nxs_fs_mkdir(file_path, DOWNLOAD_TMP_DIR_MODE) < 0) {

		if(errno != EEXIST) {

			nxs_log_write_error(&process,
			                    "[%s]: can't download file from rdmn, mkdir error: %s (dir path: %r, attachment id: \"%r\")",
			                    nxs_proc_get_name(&process),
			                    strerror(errno),
			                    file_path,
			                    attachment_id);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}

	nxs_string_printf2_cat(file_path, "/%r", file_name);

	if(nxs_chat_srv_d_rdmn_attachments_download(&content_url, file_path, NULL) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	d = nxs_array_add(&u_ctx->downloads);

	d->attachment_id = attachment_id;

	nxs_string_init3(&d->file_name, file_name);
	nxs_string_init3(&d->file_path, file_path);
	nxs_string_init3(&d->description, description);

error:

	nxs_string_free(&content_url);

	nxs_chat_srv_c_rdmn_attachment_free(&attachment);

	return rc;
}

/* Module internal (static) functions */
