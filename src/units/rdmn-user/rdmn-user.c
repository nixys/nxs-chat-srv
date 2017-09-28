#ifndef _INCLUDE_NXS_CHAT_SRV_U_RDMN_USER_C
#define _INCLUDE_NXS_CHAT_SRV_U_RDMN_USER_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/rdmn-user/rdmn-user-typedefs.h>
#include <units/rdmn-user/rdmn-user.h>
#include <units/rdmn-user/rdmn-user-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef enum
{
	STATUS_ANONYMOUS			= 0,
	STATUS_ACTIVE				= 1,
	STATUS_REGISTERED			= 2,
	STATUS_LOCKED				= 3
}
nxs_chat_srv_u_rdmn_user_status_t;

struct nxs_chat_srv_u_rdmn_user_s
{
	size_t					id;
	nxs_string_t				login;
	nxs_string_t				firstname;
	nxs_string_t				lastname;
	nxs_string_t				mail;
	nxs_string_t				api_key;
	nxs_string_t				tlgrm_username;
	nxs_chat_srv_u_rdmn_user_status_t	status;
};

/* Module internal (static) functions prototypes */

// clang-format on

static void nxs_chat_srv_u_rdmn_user_clear(nxs_chat_srv_u_rdmn_user_t *u_ctx);
static nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_user_extract(nxs_chat_srv_u_rdmn_user_t *u_ctx, nxs_buf_t *json_buf);
static nxs_cfg_json_state_t
        nxs_chat_srv_u_rdmn_user_extract_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_chat_srv_u_rdmn_user_extract_user_custom_fields(nxs_process_t *     proc,
                                                                                nxs_json_t *        json,
                                                                                nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                nxs_array_t *       cfg_arr);

// clang-format off

/* Module initializations */

static nxs_string_t _s_par_user			= nxs_string("user");
static nxs_string_t _s_par_id			= nxs_string("id");
static nxs_string_t _s_par_login		= nxs_string("login");
static nxs_string_t _s_par_firstname		= nxs_string("firstname");
static nxs_string_t _s_par_lastname		= nxs_string("lastname");
static nxs_string_t _s_par_mail			= nxs_string("mail");
static nxs_string_t _s_par_api_key		= nxs_string("api_key");
static nxs_string_t _s_par_custom_fields	= nxs_string("custom_fields");
static nxs_string_t _s_par_value		= nxs_string("value");
static nxs_string_t _s_par_status		= nxs_string("status");

/* Module global functions */

// clang-format on

nxs_chat_srv_u_rdmn_user_t *nxs_chat_srv_u_rdmn_user_init(void)
{
	nxs_chat_srv_u_rdmn_user_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_rdmn_user_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_rdmn_user_t));

	u_ctx->id     = 0;
	u_ctx->status = STATUS_ANONYMOUS;

	nxs_string_init_empty(&u_ctx->login);
	nxs_string_init_empty(&u_ctx->firstname);
	nxs_string_init_empty(&u_ctx->lastname);
	nxs_string_init_empty(&u_ctx->mail);
	nxs_string_init_empty(&u_ctx->api_key);
	nxs_string_init_empty(&u_ctx->tlgrm_username);

	return u_ctx;
}

nxs_chat_srv_u_rdmn_user_t *nxs_chat_srv_u_rdmn_user_free(nxs_chat_srv_u_rdmn_user_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	u_ctx->id     = 0;
	u_ctx->status = STATUS_ANONYMOUS;

	nxs_string_free(&u_ctx->login);
	nxs_string_free(&u_ctx->firstname);
	nxs_string_free(&u_ctx->lastname);
	nxs_string_free(&u_ctx->mail);
	nxs_string_free(&u_ctx->api_key);
	nxs_string_free(&u_ctx->tlgrm_username);

	return (nxs_chat_srv_u_rdmn_user_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_user_pull(nxs_chat_srv_u_rdmn_user_t *u_ctx, size_t user_id)
{
	nxs_chat_srv_err_t rc;
	nxs_buf_t          out_buf;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(user_id == 0) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_buf_init2(&out_buf);

	/*
	 * get data from Redmine
	 */

	if(nxs_chat_srv_d_rdmn_users_get(user_id, NXS_NO, &nxs_chat_srv_cfg.rdmn.api_key, 0, 0, NULL, &out_buf) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_rdmn_user_extract(u_ctx, &out_buf)) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(u_ctx->status != STATUS_ACTIVE) {

		nxs_chat_srv_u_rdmn_user_clear(u_ctx);

		nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);
	}

error:

	nxs_buf_free(&out_buf);

	return rc;
}

nxs_string_t *nxs_chat_srv_u_rdmn_user_get_api_key(nxs_chat_srv_u_rdmn_user_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	if(u_ctx->id == 0) {

		return NULL;
	}

	return &u_ctx->api_key;
}

/* Module internal (static) functions */

static void nxs_chat_srv_u_rdmn_user_clear(nxs_chat_srv_u_rdmn_user_t *u_ctx)
{

	if(u_ctx == NULL) {

		return;
	}

	u_ctx->id     = 0;
	u_ctx->status = STATUS_ANONYMOUS;

	nxs_string_clear(&u_ctx->login);
	nxs_string_clear(&u_ctx->firstname);
	nxs_string_clear(&u_ctx->lastname);
	nxs_string_clear(&u_ctx->mail);
	nxs_string_clear(&u_ctx->api_key);
	nxs_string_clear(&u_ctx->tlgrm_username);

	return;
}

static nxs_chat_srv_err_t nxs_chat_srv_u_rdmn_user_extract(nxs_chat_srv_u_rdmn_user_t *u_ctx, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_user,		u_ctx,	&nxs_chat_srv_u_rdmn_user_extract_user,	NULL,	NXS_CFG_JSON_TYPE_VOID,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: rdmn user extract error: parse rdmn users error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_u_rdmn_user_extract_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_u_rdmn_user_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t              cfg_json;
	nxs_array_t                 cfg_arr;
	nxs_cfg_json_state_t        rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,			&var->id,			NULL,	NULL,							NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_status,			&var->status,			NULL,	NULL,							NXS_CFG_JSON_TYPE_INT_32,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_login,			&var->login,			NULL,	NULL,							NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_firstname,		&var->firstname,		NULL,	NULL,							NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_lastname,		&var->lastname,			NULL,	NULL,							NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_mail,			&var->mail,			NULL,	NULL,							NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_api_key,		&var->api_key,			NULL,	NULL,							NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_custom_fields,		&var->tlgrm_username,		NULL,	&nxs_chat_srv_u_rdmn_user_extract_user_custom_fields,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn user extract error: 'user' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_u_rdmn_user_extract_user_custom_fields(nxs_process_t *     proc,
                                                                                nxs_json_t *        json,
                                                                                nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                nxs_array_t *       cfg_arr)
{
	nxs_string_t *tlgrm_username = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_json_t *  j;

	if((j = nxs_json_child_get_by_key(json, nxs_string_str(&_s_par_id))) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: rdmn user extract error: parse rdmn users custom fields error, missing filed \"%r\"",
		                    nxs_proc_get_name(&process),
		                    &_s_par_id);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(nxs_json_type_get(j) != NXS_JSON_TYPE_INTEGER) {

		nxs_log_write_error(
		        &process,
		        "[%s]: rdmn user extract error: parse rdmn users custom fields error, expected integer type for filed \"%r\"",
		        nxs_proc_get_name(&process),
		        &_s_par_id);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if((size_t)nxs_json_integer_val(j) == nxs_chat_srv_cfg.cache.rdmn_cf_telegram &&
	   (j = nxs_json_child_get_by_key(json, nxs_string_str(&_s_par_value))) != NULL) {

		/* needed custom field with value */

		if(nxs_json_type_get(j) != NXS_JSON_TYPE_STRING) {

			nxs_log_write_error(&process,
			                    "[%s]: rdmn user extract error: parse rdmn users custom fields error, expected string type for "
			                    "filed \"%r\"",
			                    nxs_proc_get_name(&process),
			                    &_s_par_value);

			return NXS_CFG_JSON_CONF_ERROR;
		}

		nxs_string_clone(tlgrm_username, nxs_json_string_val(j));
	}

	return NXS_CFG_JSON_CONF_OK;
}
