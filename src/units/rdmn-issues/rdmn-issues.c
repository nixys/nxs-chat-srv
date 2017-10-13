#ifndef _INCLUDE_NXS_CHAT_SRV_U_RDMN_ISSUES_C
#define _INCLUDE_NXS_CHAT_SRV_U_RDMN_ISSUES_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/rdmn-issues/rdmn-issues-typedefs.h>
#include <units/rdmn-issues/rdmn-issues.h>
#include <units/rdmn-issues/rdmn-issues-subunits.h>

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
} nxs_chat_srv_u_rdmn_issues_mine_t;

typedef struct
{
	nxs_string_t		file_path;
	nxs_string_t		file_name;
} nxs_chat_srv_u_rdmn_issues_upload_t;

typedef struct
{
	size_t			id;
	nxs_string_t		subject;
} nxs_chat_srv_u_rdmn_issues_short_t;

struct nxs_chat_srv_u_rdmn_issues_s
{
	nxs_array_t		shorts;			/* type: nxs_chat_srv_u_rdmn_issues_short_t */
	nxs_array_t		custom_fields;		/* type: nxs_chat_srv_d_rdmn_issues_cf_t */
	nxs_array_t		uploads;		/* type: nxs_chat_srv_u_rdmn_issues_upload_t */
};

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_string_t *nxs_chat_srv_u_rdmn_issues_mime_get(nxs_string_t *file_name);

static nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_issues_create_extract(size_t *new_issue_id, nxs_buf_t *json_buf);
static nxs_cfg_json_state_t
        nxs_chat_srv_u_rdmn_issues_create_extract_issue(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

static nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_issues_get_query_extract(nxs_array_t *issues, nxs_buf_t *json_buf, size_t *total_count);
static nxs_cfg_json_state_t nxs_chat_srv_u_rdmn_issues_get_query_extract_issues(nxs_process_t *     proc,
                                                                                nxs_json_t *        json,
                                                                                nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                nxs_array_t *       cfg_arr);

void nxs_chat_srv_u_rdmn_issues_shorts_init(nxs_array_t *shorts);
void nxs_chat_srv_u_rdmn_issues_shorts_free(nxs_array_t *shorts);
static void nxs_chat_srv_u_rdmn_issues_uploads_init(nxs_array_t *uploads);
static void nxs_chat_srv_u_rdmn_issues_uploads_free(nxs_array_t *uploads);
static nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_issues_file_upload_extract(nxs_string_t *upload_token, nxs_buf_t *json_buf);
static nxs_cfg_json_state_t
        nxs_chat_srv_u_rdmn_issues_file_upload_token_extract(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

// clang-format off

/* Module initializations */

static nxs_string_t _s_par_issue		= nxs_string("issue");
static nxs_string_t _s_par_issues		= nxs_string("issues");
static nxs_string_t _s_par_total_count		= nxs_string("total_count");
static nxs_string_t _s_par_id			= nxs_string("id");
static nxs_string_t _s_par_subject		= nxs_string("subject");
static nxs_string_t _s_par_upload		= nxs_string("upload");
static nxs_string_t _s_par_token		= nxs_string("token");

static nxs_string_t _s_mime_default		= nxs_string("application/octet-stream");

nxs_chat_srv_u_rdmn_issues_mine_t mimes[] =
{
	{nxs_string("jpeg"),			nxs_string("image/jpeg")},
	{nxs_string("jpg"),			nxs_string("image/jpeg")},
	{nxs_string("png"),			nxs_string("image/png")},

	{NXS_STRING_NULL_STR,			NXS_STRING_NULL_STR},
};

/* Module global functions */

// clang-format on

nxs_chat_srv_u_rdmn_issues_t *nxs_chat_srv_u_rdmn_issues_init(void)
{
	nxs_chat_srv_u_rdmn_issues_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_rdmn_issues_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_rdmn_issues_t));

	nxs_chat_srv_u_rdmn_issues_shorts_init(&u_ctx->shorts);
	nxs_chat_srv_u_rdmn_issues_uploads_init(&u_ctx->uploads);

	nxs_chat_srv_d_rdmn_issues_cf_init(&u_ctx->custom_fields);

	return u_ctx;
}

nxs_chat_srv_u_rdmn_issues_t *nxs_chat_srv_u_rdmn_issues_free(nxs_chat_srv_u_rdmn_issues_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	nxs_chat_srv_u_rdmn_issues_shorts_free(&u_ctx->shorts);
	nxs_chat_srv_u_rdmn_issues_uploads_free(&u_ctx->uploads);

	nxs_chat_srv_d_rdmn_issues_cf_free(&u_ctx->custom_fields);

	return (nxs_chat_srv_u_rdmn_issues_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_issues_create(nxs_chat_srv_u_rdmn_issues_t *u_ctx,
                                                     size_t                        project_id,
                                                     size_t                        priority_id,
                                                     nxs_string_t *                subject,
                                                     nxs_string_t *                description,
                                                     nxs_bool_t                    is_private,
                                                     size_t *                      new_issue_id,
                                                     nxs_string_t *                api_key)
{
	nxs_chat_srv_u_rdmn_issues_upload_t *u;
	nxs_buf_t                            out_buf;
	nxs_array_t                          d_uploads;
	nxs_string_t                         upload_token;
	nxs_chat_srv_err_t                   rc, ec;
	nxs_http_code_t                      http_code;
	size_t                               i;

	if(u_ctx == NULL || subject == NULL || description == NULL || new_issue_id == NULL || api_key == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_buf_init2(&out_buf);

	nxs_string_init(&upload_token);

	nxs_chat_srv_d_rdmn_issues_uploads_init(&d_uploads);

	if(nxs_array_count(&u_ctx->uploads) > 0) {

		for(i = 0; i < nxs_array_count(&u_ctx->uploads); i++) {

			u = nxs_array_get(&u_ctx->uploads, i);

			ec = nxs_chat_srv_d_rdmn_issues_file_upload(api_key, &u->file_path, NULL, &out_buf);

			if(nxs_fs_unlink(&u->file_path) < 0) {

				nxs_log_write_warn(&process,
				                   "[%s]: rdmn add file to issue note warn: %s (file path: %r)",
				                   nxs_proc_get_name(&process),
				                   strerror(errno),
				                   &u->file_path);
			}

			switch(ec) {

				case NXS_CHAT_SRV_E_OK:

					if(nxs_chat_srv_u_rdmn_issues_file_upload_extract(&upload_token, &out_buf) != NXS_CHAT_SRV_E_OK) {

						nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
					}

					nxs_chat_srv_d_rdmn_issues_uploads_add(&d_uploads,
					                                       &upload_token,
					                                       &u->file_name,
					                                       nxs_chat_srv_u_rdmn_issues_mime_get(&u->file_path));

					break;

				case NXS_CHAT_SRV_E_ATTR:

					break;

				default:

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}
		}
	}

	if(nxs_chat_srv_d_rdmn_issues_create(
	           project_id, priority_id, subject, description, is_private, &d_uploads, api_key, &http_code, &out_buf) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_rdmn_issues_create_extract(new_issue_id, &out_buf) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_buf_free(&out_buf);

	nxs_string_free(&upload_token);

	nxs_chat_srv_d_rdmn_issues_uploads_free(&d_uploads);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_issues_add_note(nxs_chat_srv_u_rdmn_issues_t *u_ctx,
                                                       size_t                        issue_id,
                                                       size_t                        assigned_to_id,
                                                       nxs_string_t *                note,
                                                       nxs_bool_t                    private_notes,
                                                       size_t                        status_id,
                                                       nxs_string_t *                api_key)
{
	nxs_chat_srv_u_rdmn_issues_upload_t *u;
	nxs_string_t                         upload_token;
	nxs_buf_t                            out_buf;
	nxs_array_t                          d_uploads;
	nxs_chat_srv_err_t                   rc, ec;
	size_t                               i;

	if(u_ctx == NULL || note == NULL || api_key == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_buf_init2(&out_buf);

	nxs_string_init(&upload_token);

	nxs_chat_srv_d_rdmn_issues_uploads_init(&d_uploads);

	if(nxs_array_count(&u_ctx->uploads) > 0) {

		for(i = 0; i < nxs_array_count(&u_ctx->uploads); i++) {

			u = nxs_array_get(&u_ctx->uploads, i);

			ec = nxs_chat_srv_d_rdmn_issues_file_upload(api_key, &u->file_path, NULL, &out_buf);

			if(nxs_fs_unlink(&u->file_path) < 0) {

				nxs_log_write_warn(&process,
				                   "[%s]: rdmn add file to issue note warn: %s (file path: %r)",
				                   nxs_proc_get_name(&process),
				                   strerror(errno),
				                   &u->file_path);
			}

			switch(ec) {

				case NXS_CHAT_SRV_E_OK:

					if(nxs_chat_srv_u_rdmn_issues_file_upload_extract(&upload_token, &out_buf) != NXS_CHAT_SRV_E_OK) {

						nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
					}

					nxs_chat_srv_d_rdmn_issues_uploads_add(&d_uploads,
					                                       &upload_token,
					                                       &u->file_name,
					                                       nxs_chat_srv_u_rdmn_issues_mime_get(&u->file_path));

					break;

				case NXS_CHAT_SRV_E_ATTR:

					break;

				default:

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}
		}
	}

	switch(nxs_chat_srv_d_rdmn_issues_add_comment(
	        issue_id, assigned_to_id, note, private_notes, status_id, &d_uploads, &u_ctx->custom_fields, api_key, NULL, NULL)) {

		case NXS_CHAT_SRV_E_OK:

			rc = NXS_CHAT_SRV_E_OK;

			break;

		case NXS_CHAT_SRV_E_ATTR:

			rc = NXS_CHAT_SRV_E_WARN;

			break;

		default:

			rc = NXS_CHAT_SRV_E_ERR;

			break;
	}

error:

	nxs_buf_free(&out_buf);

	nxs_string_free(&upload_token);

	nxs_chat_srv_d_rdmn_issues_uploads_free(&d_uploads);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_issues_get_query(nxs_chat_srv_u_rdmn_issues_t *u_ctx,
                                                        size_t                        issue_query_id,
                                                        size_t                        offset,
                                                        size_t                        limit,
                                                        nxs_string_t *                api_key,
                                                        size_t *                      total_count)
{
	nxs_buf_t          out_buf;
	nxs_chat_srv_err_t rc;
	nxs_http_code_t    http_code;

	if(u_ctx == NULL || api_key == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_buf_init2(&out_buf);

	if(nxs_chat_srv_d_rdmn_issues_get_query(issue_query_id, api_key, offset, limit, &http_code, &out_buf) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_rdmn_issues_get_query_extract(&u_ctx->shorts, &out_buf, total_count) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_buf_free(&out_buf);

	return rc;
}

nxs_string_t *nxs_chat_srv_u_rdmn_issues_shorts_get(nxs_chat_srv_u_rdmn_issues_t *u_ctx, size_t i, size_t *issue_id)
{
	nxs_chat_srv_u_rdmn_issues_short_t *iss;

	if(u_ctx == NULL) {

		return NULL;
	}

	if((iss = nxs_array_get(&u_ctx->shorts, i)) == NULL) {

		return NULL;
	}

	if(issue_id != NULL) {

		*issue_id = iss->id;
	}

	return &iss->subject;
}

void nxs_chat_srv_u_rdmn_issues_cf_add(nxs_chat_srv_u_rdmn_issues_t *u_ctx, size_t id, nxs_string_t *value)
{

	if(u_ctx == NULL || value == NULL) {

		return;
	}

	nxs_chat_srv_d_rdmn_issues_cf_add(&u_ctx->custom_fields, id, value);
}

void nxs_chat_srv_u_rdmn_issues_uploads_add(nxs_chat_srv_u_rdmn_issues_t *u_ctx, nxs_string_t *file_path, nxs_string_t *file_name)
{
	nxs_chat_srv_u_rdmn_issues_upload_t *u;

	if(u_ctx == NULL || file_path == NULL || file_name == NULL) {

		return;
	}

	u = nxs_array_add(&u_ctx->uploads);

	nxs_string_init3(&u->file_path, file_path);
	nxs_string_init3(&u->file_name, file_name);
}

/* Module internal (static) functions */

static nxs_string_t *nxs_chat_srv_u_rdmn_issues_mime_get(nxs_string_t *file_name)
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

static nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_issues_create_extract(size_t *new_issue_id, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_issue,		new_issue_id,	&nxs_chat_srv_u_rdmn_issues_create_extract_issue,	NULL,	NXS_CFG_JSON_TYPE_VOID,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(
		        &process, "[%s]: rdmn new issue extract error: parse rdmn new issue error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_u_rdmn_issues_create_extract_issue(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	size_t *             var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t       cfg_json;
	nxs_array_t          cfg_arr;
	nxs_cfg_json_state_t rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,	var,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn new issue extract error: 'issue' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_issues_get_query_extract(nxs_array_t *issues, nxs_buf_t *json_buf, size_t *total_count)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_issues,		issues,		NULL,	&nxs_chat_srv_u_rdmn_issues_get_query_extract_issues,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_total_count,	total_count,	NULL,	NULL,							NXS_CFG_JSON_TYPE_INT,			0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(
		        &process, "[%s]: rdmn get issues by query extract error: parse rdmn issues error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_u_rdmn_issues_get_query_extract_issues(nxs_process_t *     proc,
                                                                                nxs_json_t *        json,
                                                                                nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                nxs_array_t *       cfg_arr)
{
	nxs_array_t *                       issues = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_u_rdmn_issues_short_t *iss;

	iss = nxs_array_add(issues);

	iss->id = 0;

	nxs_string_init_empty(&iss->subject);

	nxs_cfg_json_conf_array_skip_undef(cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_id,		&iss->id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_subject,	&iss->subject,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

void nxs_chat_srv_u_rdmn_issues_shorts_init(nxs_array_t *shorts)
{

	if(shorts == NULL) {

		return;
	}

	nxs_array_init2(shorts, nxs_chat_srv_u_rdmn_issues_short_t);
}

void nxs_chat_srv_u_rdmn_issues_shorts_free(nxs_array_t *shorts)
{
	nxs_chat_srv_u_rdmn_issues_short_t *iss;
	size_t                              i;

	if(shorts == NULL) {

		return;
	}

	for(i = 0; i < nxs_array_count(shorts); i++) {

		iss = nxs_array_get(shorts, i);

		iss->id = 0;

		nxs_string_free(&iss->subject);
	}

	nxs_array_free(shorts);
}

static void nxs_chat_srv_u_rdmn_issues_uploads_init(nxs_array_t *uploads)
{

	if(uploads == NULL) {

		return;
	}

	nxs_array_init2(uploads, nxs_chat_srv_u_rdmn_issues_upload_t);
}

static void nxs_chat_srv_u_rdmn_issues_uploads_free(nxs_array_t *uploads)
{
	nxs_chat_srv_u_rdmn_issues_upload_t *u;
	size_t                               i;

	if(uploads == NULL) {

		return;
	}

	for(i = 0; i < nxs_array_count(uploads); i++) {

		u = nxs_array_get(uploads, i);

		nxs_string_free(&u->file_path);
		nxs_string_free(&u->file_name);
	}

	nxs_array_free(uploads);
}

static nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_issues_file_upload_extract(nxs_string_t *upload_token, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_upload,		upload_token,	&nxs_chat_srv_u_rdmn_issues_file_upload_token_extract,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

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
        nxs_chat_srv_u_rdmn_issues_file_upload_token_extract(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
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
