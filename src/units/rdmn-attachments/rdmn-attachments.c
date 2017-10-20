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
	nxs_string_t		content_type;
} nxs_chat_srv_u_rdmn_attachments_download_t;

struct nxs_chat_srv_u_rdmn_attachments_s
{
	nxs_buf_t		response_buf;
	nxs_array_t		downloads;		/* type: nxs_chat_srv_u_rdmn_attachments_download_t */
	nxs_array_t		uploads;		/* type: nxs_chat_srv_d_rdmn_issues_upload_t (defined in dal 'rdmn-issues' ) */
};

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_attachments_upload_extract(nxs_string_t *upload_token, nxs_buf_t *json_buf);
static nxs_cfg_json_state_t
        nxs_chat_srv_u_rdmn_attachments_upload_token_extract(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

// clang-format off

/* Module initializations */

static nxs_string_t _s_par_upload	= nxs_string("upload");
static nxs_string_t _s_par_token	= nxs_string("token");

static nxs_string_t _s_proto_http	= nxs_string("http://");
static nxs_string_t _s_proto_https	= nxs_string("https://");

/* Module global functions */

// clang-format on

nxs_chat_srv_u_rdmn_attachments_t *nxs_chat_srv_u_rdmn_attachments_init(void)
{
	nxs_chat_srv_u_rdmn_attachments_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_rdmn_attachments_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_rdmn_attachments_t));

	nxs_buf_init2(&u_ctx->response_buf);

	nxs_array_init2(&u_ctx->downloads, nxs_chat_srv_u_rdmn_attachments_download_t);

	nxs_chat_srv_d_rdmn_issues_uploads_init(&u_ctx->uploads);

	return u_ctx;
}

nxs_chat_srv_u_rdmn_attachments_t *nxs_chat_srv_u_rdmn_attachments_free(nxs_chat_srv_u_rdmn_attachments_t *u_ctx)
{
	nxs_chat_srv_u_rdmn_attachments_download_t *d;
	nxs_string_t                                tmp_dir;
	size_t                                      i;

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
		nxs_string_free(&d->content_type);
	}

	nxs_array_free(&u_ctx->downloads);

	nxs_chat_srv_d_rdmn_issues_uploads_free(&u_ctx->uploads);

	nxs_buf_free(&u_ctx->response_buf);

	nxs_string_free(&tmp_dir);

	return (nxs_chat_srv_u_rdmn_attachments_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_attachments_download(nxs_chat_srv_u_rdmn_attachments_t *u_ctx,
                                                            size_t                             attachment_id,
                                                            nxs_string_t *                     file_name,
                                                            nxs_string_t *                     file_path,
                                                            nxs_string_t *                     description,
                                                            nxs_string_t *                     content_type)
{
	nxs_chat_srv_m_rdmn_attachment_t            attachment;
	nxs_chat_srv_u_rdmn_attachments_download_t *d;
	nxs_string_t                                content_url;
	nxs_chat_srv_err_t                          rc;
	size_t                                      i;
	u_char *                                    c;

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
			nxs_string_clone(content_type, &d->content_type);

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
	nxs_string_clone(content_type, &attachment.content_type);

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
	nxs_string_init3(&d->content_type, content_type);

error:

	nxs_string_free(&content_url);

	nxs_chat_srv_c_rdmn_attachment_free(&attachment);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_attachments_upload(nxs_chat_srv_u_rdmn_attachments_t *u_ctx,
                                                          nxs_string_t *                     api_key,
                                                          nxs_string_t *                     file_name,
                                                          nxs_string_t *                     file_path,
                                                          nxs_string_t *                     mime_type)
{
	nxs_buf_t          out_buf;
	nxs_string_t       upload_token, *m, file_name_mime;
	nxs_chat_srv_err_t rc;

	if(u_ctx == NULL || api_key == NULL || file_name == NULL || file_path == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_buf_init2(&out_buf);

	nxs_string_init(&upload_token);
	nxs_string_init(&file_name_mime);

	switch(nxs_chat_srv_d_rdmn_issues_uploads_push(api_key, file_path, NULL, &out_buf)) {

		case NXS_CHAT_SRV_E_OK:

			if(nxs_chat_srv_u_rdmn_attachments_upload_extract(&upload_token, &out_buf) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_string_len(mime_type) == 0) {

				m = nxs_chat_srv_c_mime_get(file_path);
			}
			else {

				m = mime_type;
			}

			nxs_chat_srv_c_mime_add_file_extension(&file_name_mime, file_name, m);

			nxs_chat_srv_d_rdmn_issues_uploads_add(&u_ctx->uploads, &upload_token, &file_name_mime, file_path, m);

			break;

		case NXS_CHAT_SRV_E_ATTR:

			break;

		default:

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_buf_free(&out_buf);

	nxs_string_free(&upload_token);
	nxs_string_free(&file_name_mime);

	return rc;
}

nxs_array_t *nxs_chat_srv_u_rdmn_attachments_uploads_get(nxs_chat_srv_u_rdmn_attachments_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	return &u_ctx->uploads;
}

/* Module internal (static) functions */

static nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_attachments_upload_extract(nxs_string_t *upload_token, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_upload,		upload_token,	&nxs_chat_srv_u_rdmn_attachments_upload_token_extract,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(
		        &process, "[%s]: rdmn upload response extract error: parse rdmn upload response", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_u_rdmn_attachments_upload_token_extract(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_string_t *       var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t       cfg_json;
	nxs_array_t          cfg_arr;
	nxs_cfg_json_state_t rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_token,	var,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "[%s]: rdmn upload response extract error: 'upload' block", nxs_proc_get_name(&process));

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}
