#ifndef _INCLUDE_NXS_CHAT_SRV_U_TLGRM_ATTACHMENTS_C
#define _INCLUDE_NXS_CHAT_SRV_U_TLGRM_ATTACHMENTS_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/tlgrm-attachments/tlgrm-attachments-typedefs.h>
#include <units/tlgrm-attachments/tlgrm-attachments.h>
#include <units/tlgrm-attachments/tlgrm-attachments-subunits.h>

/* Definitions */

#define DOWNLOAD_TMP_DIR_MODE		0700

/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	nxs_string_t		file_path;
	nxs_string_t		file_id;
} nxs_chat_srv_u_tlgrm_attachments_upload_t;

typedef struct
{
	size_t			tlgrm_userid;
	nxs_string_t		file_name;
	nxs_string_t		file_path;
	nxs_string_t		file_id;
} nxs_chat_srv_u_tlgrm_attachments_download_t;

struct nxs_chat_srv_u_tlgrm_attachments_s
{
	nxs_buf_t		response_buf;
	nxs_array_t		uploads;		/* type: nxs_chat_srv_u_tlgrm_attachments_upload_t */
	nxs_array_t		downloads;		/* type: nxs_chat_srv_u_tlgrm_attachments_download_t */
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_u_tlgrm_attachments_t *nxs_chat_srv_u_tlgrm_attachments_init(void)
{
	nxs_chat_srv_u_tlgrm_attachments_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_tlgrm_attachments_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_tlgrm_attachments_t));

	nxs_buf_init2(&u_ctx->response_buf);

	nxs_array_init2(&u_ctx->uploads, nxs_chat_srv_u_tlgrm_attachments_upload_t);
	nxs_array_init2(&u_ctx->downloads, nxs_chat_srv_u_tlgrm_attachments_download_t);

	return u_ctx;
}

nxs_chat_srv_u_tlgrm_attachments_t *nxs_chat_srv_u_tlgrm_attachments_free(nxs_chat_srv_u_tlgrm_attachments_t *u_ctx)
{
	nxs_chat_srv_u_tlgrm_attachments_upload_t *  u;
	nxs_chat_srv_u_tlgrm_attachments_download_t *d;
	size_t                                       i;

	if(u_ctx == NULL) {

		return NULL;
	}

	for(i = 0; i < nxs_array_count(&u_ctx->uploads); i++) {

		u = nxs_array_get(&u_ctx->uploads, i);

		nxs_string_free(&u->file_id);
		nxs_string_free(&u->file_path);
	}

	for(i = 0; i < nxs_array_count(&u_ctx->downloads); i++) {

		d = nxs_array_get(&u_ctx->downloads, i);

		if(nxs_fs_unlink(&d->file_path) < 0) {

			nxs_log_write_warn(&process,
			                   "[%s]: can't remove downloaded tlgrm file: %s (file path: %r)",
			                   nxs_proc_get_name(&process),
			                   strerror(errno),
			                   &d->file_path);
		}

		d->tlgrm_userid = 0;

		nxs_string_free(&d->file_id);
		nxs_string_free(&d->file_name);
		nxs_string_free(&d->file_path);
	}

	nxs_array_free(&u_ctx->uploads);
	nxs_array_free(&u_ctx->downloads);

	nxs_buf_free(&u_ctx->response_buf);

	return (nxs_chat_srv_u_tlgrm_attachments_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_attachments_download(nxs_chat_srv_u_tlgrm_attachments_t *u_ctx,
                                                             size_t                              tlgrm_userid,
                                                             nxs_chat_srv_m_db_sess_file_t *     file,
                                                             nxs_string_t *                      file_name,
                                                             nxs_string_t *                      file_path)
{
	nxs_chat_srv_u_tlgrm_attachments_download_t *d;
	nxs_string_t                                 message, tmp_file_name;
	nxs_chat_srv_m_tlgrm_file_t                  tlgrm_file;
	nxs_chat_srv_err_t                           rc;
	nxs_bool_t                                   response_status;
	size_t                                       i;

	if(u_ctx == NULL || file == NULL || file_name == NULL || file_path == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	for(i = 0; i < nxs_array_count(&u_ctx->downloads); i++) {

		d = nxs_array_get(&u_ctx->downloads, i);

		if(d->tlgrm_userid == tlgrm_userid && nxs_string_cmp(&d->file_id, 0, &file->file_id, 0) == NXS_YES) {

			/* if file already has been downloaded */

			if(nxs_string_len(&file->file_name) == 0) {

				nxs_string_clone(file_name, &d->file_name);
			}
			else {

				nxs_string_clone(file_name, &file->file_name);
			}

			nxs_string_clone(file_path, &d->file_path);

			return NXS_CHAT_SRV_E_OK;
		}
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_chat_srv_c_tlgrm_file_init(&tlgrm_file);

	nxs_string_init(&message);
	nxs_string_init(&tmp_file_name);

	nxs_string_printf(&message, "{\"file_id\":\"%r\"}", &file->file_id);

	if(nxs_chat_srv_d_tlgrm_request(NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_GET_FILE, &message, NULL, &u_ctx->response_buf) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: can't get file download link from tlgrm (file id: \"%r\")",
		                    nxs_proc_get_name(&process),
		                    &file->file_id);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_c_tlgrm_file_result_pull_json(&tlgrm_file, &response_status, &u_ctx->response_buf) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_string_basename(&tmp_file_name, &tlgrm_file.file_path);

	nxs_string_printf(file_path, "%r/%zu", &nxs_chat_srv_cfg.attachments.tlgrm_download_tmp_dir, tlgrm_userid);

	if(nxs_fs_mkdir(file_path, DOWNLOAD_TMP_DIR_MODE) < 0) {

		if(errno != EEXIST) {

			nxs_log_write_error(&process,
			                    "[%s]: can't download file from tlgrm, mkdir error: %s (dir path: %r, file id: \"%r\")",
			                    nxs_proc_get_name(&process),
			                    strerror(errno),
			                    file_path,
			                    &file->file_id);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}

	nxs_string_printf2_cat(file_path, "/%r", &tmp_file_name);

	if(nxs_chat_srv_d_tlgrm_download(&tlgrm_file.file_path, file_path, NULL) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_string_len(&file->file_name) == 0) {

		nxs_string_clone(file_name, &tmp_file_name);
	}
	else {

		nxs_string_clone(file_name, &file->file_name);
	}

	d = nxs_array_add(&u_ctx->downloads);

	d->tlgrm_userid = tlgrm_userid;

	nxs_string_init3(&d->file_id, &file->file_id);
	nxs_string_init3(&d->file_name, &tmp_file_name);
	nxs_string_init3(&d->file_path, file_path);

error:

	nxs_chat_srv_c_tlgrm_file_free(&tlgrm_file);

	nxs_string_free(&message);
	nxs_string_free(&tmp_file_name);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_attachments_send_photo(nxs_chat_srv_u_tlgrm_attachments_t *u_ctx,
                                                               size_t                              chat_id,
                                                               nxs_string_t *                      file_path,
                                                               nxs_string_t *                      caption,
                                                               size_t *                            message_id)
{
	nxs_string_t                               message;
	nxs_chat_srv_u_tlgrm_attachments_upload_t *u;
	nxs_chat_srv_m_tlgrm_message_t             tlgrm_message;
	nxs_chat_srv_m_tlgrm_photo_size_t *        p;
	nxs_chat_srv_err_t                         rc;
	nxs_bool_t                                 response_status;
	size_t                                     i;

	if(u_ctx == NULL || file_path == NULL || message_id == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&message);

	nxs_chat_srv_c_tlgrm_message_init(&tlgrm_message);

	for(i = 0; i < nxs_array_count(&u_ctx->uploads); i++) {

		u = nxs_array_get(&u_ctx->uploads, i);

		if(nxs_string_cmp(&u->file_path, 0, file_path, 0) == NXS_YES) {

			/* file has been uploaded into Telegram */

			nxs_string_printf(&message, "{\"chat_id\":%zu,\"photo\":\"%r\",\"caption\":\"%r\"}", chat_id, &u->file_id, caption);

			if(nxs_chat_srv_d_tlgrm_request(NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_PHOTO, &message, NULL, &u_ctx->response_buf) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_log_write_error(&process,
				                    "[%s]: can't upload file to tlgrm (chat_id: %zu, file path: %r)",
				                    nxs_proc_get_name(&process),
				                    chat_id,
				                    file_path);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_c_tlgrm_message_result_pull_json(&tlgrm_message, &response_status, &u_ctx->response_buf) ==
			   NXS_CHAT_SRV_E_OK) {

				*message_id = tlgrm_message.message_id;
			}

			nxs_error(rc, NXS_CHAT_SRV_E_OK, error);
		}
	}

	/* if file not yet uploaded into tlgrm */

	if(nxs_chat_srv_d_tlgrm_upload(
	           NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_PHOTO, file_path, chat_id, caption, NULL, &u_ctx->response_buf) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: can't upload file to tlgrm (chat_id: %zu, file path: %r)",
		                    nxs_proc_get_name(&process),
		                    chat_id,
		                    file_path);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_c_tlgrm_message_result_pull_json(&tlgrm_message, &response_status, &u_ctx->response_buf) == NXS_CHAT_SRV_E_OK) {

		*message_id = tlgrm_message.message_id;

		if((p = nxs_array_get(&tlgrm_message.photo, nxs_array_count(&tlgrm_message.photo) - 1)) != NULL) {

			u = nxs_array_add(&u_ctx->uploads);

			nxs_string_init3(&u->file_path, file_path);
			nxs_string_init3(&u->file_id, &p->file_id);
		}
	}

error:

	nxs_string_free(&message);

	nxs_chat_srv_c_tlgrm_message_free(&tlgrm_message);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_attachments_send_document(nxs_chat_srv_u_tlgrm_attachments_t *u_ctx,
                                                                  size_t                              chat_id,
                                                                  nxs_string_t *                      file_path,
                                                                  nxs_string_t *                      caption,
                                                                  size_t *                            message_id)
{
	nxs_string_t                               message;
	nxs_chat_srv_u_tlgrm_attachments_upload_t *u;
	nxs_chat_srv_m_tlgrm_message_t             tlgrm_message;
	nxs_chat_srv_err_t                         rc;
	nxs_bool_t                                 response_status;
	size_t                                     i;

	if(u_ctx == NULL || file_path == NULL || message_id == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&message);

	nxs_chat_srv_c_tlgrm_message_init(&tlgrm_message);

	for(i = 0; i < nxs_array_count(&u_ctx->uploads); i++) {

		u = nxs_array_get(&u_ctx->uploads, i);

		if(nxs_string_cmp(&u->file_path, 0, file_path, 0) == NXS_YES) {

			/* file has been uploaded into Telegram */

			nxs_string_printf(
			        &message, "{\"chat_id\":%zu,\"document\":\"%r\",\"caption\":\"%r\"}", chat_id, &u->file_id, caption);

			if(nxs_chat_srv_d_tlgrm_request(
			           NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_DOCUMENT, &message, NULL, &u_ctx->response_buf) !=
			   NXS_CHAT_SRV_E_OK) {

				nxs_log_write_error(&process,
				                    "[%s]: can't upload file to tlgrm (chat_id: %zu, file path: %r)",
				                    nxs_proc_get_name(&process),
				                    chat_id,
				                    file_path);

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			if(nxs_chat_srv_c_tlgrm_message_result_pull_json(&tlgrm_message, &response_status, &u_ctx->response_buf) ==
			   NXS_CHAT_SRV_E_OK) {

				*message_id = tlgrm_message.message_id;
			}

			nxs_error(rc, NXS_CHAT_SRV_E_OK, error);
		}
	}

	/* if file not yet uploaded into tlgrm */

	if(nxs_chat_srv_d_tlgrm_upload(
	           NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_SEND_DOCUMENT, file_path, chat_id, caption, NULL, &u_ctx->response_buf) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: can't upload file to tlgrm (chat_id: %zu, file path: %r)",
		                    nxs_proc_get_name(&process),
		                    chat_id,
		                    file_path);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_c_tlgrm_message_result_pull_json(&tlgrm_message, &response_status, &u_ctx->response_buf) == NXS_CHAT_SRV_E_OK) {

		*message_id = tlgrm_message.message_id;

		u = nxs_array_add(&u_ctx->uploads);

		nxs_string_init3(&u->file_path, file_path);
		nxs_string_init3(&u->file_id, &tlgrm_message.document.file_id);
	}

error:

	nxs_string_free(&message);

	nxs_chat_srv_c_tlgrm_message_free(&tlgrm_message);

	return rc;
}

/* Module internal (static) functions */
