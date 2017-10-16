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

struct nxs_chat_srv_u_tlgrm_attachments_s
{
	nxs_buf_t		response_buf;
	nxs_array_t		uploaded_files;		/* type: nxs_chat_srv_u_tlgrm_attachments_upload_t */
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

	nxs_array_init2(&u_ctx->uploaded_files, nxs_chat_srv_u_tlgrm_attachments_upload_t);

	return u_ctx;
}

nxs_chat_srv_u_tlgrm_attachments_t *nxs_chat_srv_u_tlgrm_attachments_free(nxs_chat_srv_u_tlgrm_attachments_t *u_ctx)
{
	nxs_chat_srv_u_tlgrm_attachments_upload_t *u;
	size_t                                     i;

	if(u_ctx == NULL) {

		return NULL;
	}

	for(i = 0; i < nxs_array_count(&u_ctx->uploaded_files); i++) {

		u = nxs_array_get(&u_ctx->uploaded_files, i);

		nxs_string_free(&u->file_id);
		nxs_string_free(&u->file_path);
	}

	nxs_array_free(&u_ctx->uploaded_files);

	nxs_buf_free(&u_ctx->response_buf);

	return (nxs_chat_srv_u_tlgrm_attachments_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_tlgrm_attachments_download(nxs_chat_srv_u_tlgrm_attachments_t *u_ctx,
                                                             size_t                              tlgrm_userid,
                                                             nxs_string_t *                      file_id,
                                                             nxs_string_t *                      file_name,
                                                             nxs_string_t *                      file_path)
{
	nxs_string_t                message;
	nxs_chat_srv_m_tlgrm_file_t file;
	nxs_chat_srv_err_t          rc;
	nxs_bool_t                  response_status;

	if(u_ctx == NULL || file_id == NULL || file_name == NULL || file_path == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_chat_srv_c_tlgrm_file_init(&file);

	nxs_string_init(&message);

	nxs_string_printf(&message, "{\"file_id\":\"%r\"}", file_id);

	if(nxs_chat_srv_d_tlgrm_request(NXS_CHAT_SRV_TLGRM_REQUEST_TYPE_GET_FILE, &message, NULL, &u_ctx->response_buf) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(
		        &process, "[%s]: can't get file download link from tlgrm (file id: \"%r\")", nxs_proc_get_name(&process), file_id);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_c_tlgrm_file_result_pull_json(&file, &response_status, &u_ctx->response_buf) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_string_basename(file_name, &file.file_path);

	nxs_string_printf(file_path, "%r/%zu", &nxs_chat_srv_cfg.attachments.tlgrm_download_tmp_dir, tlgrm_userid);

	if(nxs_fs_mkdir(file_path, DOWNLOAD_TMP_DIR_MODE) < 0) {

		if(errno != EEXIST) {

			nxs_log_write_error(&process,
			                    "[%s]: can't download file from tlgrm, mkdir error: %s (dir path: %r, file id: \"%r\")",
			                    nxs_proc_get_name(&process),
			                    strerror(errno),
			                    file_path,
			                    file_id);

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}
	}

	nxs_string_printf2_cat(file_path, "/%r", file_name);

	if(nxs_chat_srv_d_tlgrm_download(&file.file_path, file_path, NULL) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_chat_srv_c_tlgrm_file_free(&file);

	nxs_string_free(&message);

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

	for(i = 0; i < nxs_array_count(&u_ctx->uploaded_files); i++) {

		u = nxs_array_get(&u_ctx->uploaded_files, i);

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

			u = nxs_array_add(&u_ctx->uploaded_files);

			nxs_string_init3(&u->file_path, file_path);
			nxs_string_init3(&u->file_id, &p->file_id);
		}
	}

error:

	nxs_string_free(&message);

	nxs_chat_srv_c_tlgrm_message_free(&tlgrm_message);

	return rc;
}

/* Module internal (static) functions */
