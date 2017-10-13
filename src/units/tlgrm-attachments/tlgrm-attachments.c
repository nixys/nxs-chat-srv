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

struct nxs_chat_srv_u_tlgrm_attachments_s
{
	nxs_buf_t		response_buf;
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

	return u_ctx;
}

nxs_chat_srv_u_tlgrm_attachments_t *nxs_chat_srv_u_tlgrm_attachments_free(nxs_chat_srv_u_tlgrm_attachments_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

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

/* Module internal (static) functions */
