#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_USERS_C
#define _INCLUDE_NXS_CHAT_SRV_D_DB_USERS_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/db-users/db-users-typedefs.h>
#include <dal/db-users/db-users.h>
#include <dal/db-users/db-users-subdals.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef enum
{
	NXS_CHAT_SRV_D_DB_USERS_FIELD_ID		= 0,
	NXS_CHAT_SRV_D_DB_USERS_FIELD_T_USERNAME	= 1,
	NXS_CHAT_SRV_D_DB_USERS_FIELD_T_USERID		= 2,
	NXS_CHAT_SRV_D_DB_USERS_FIELD_T_USERLANG	= 3,
	NXS_CHAT_SRV_D_DB_USERS_FIELD_R_USERID		= 4,
	NXS_CHAT_SRV_D_DB_USERS_FIELD_R_USERNAME	= 5,
	NXS_CHAT_SRV_D_DB_USERS_FIELD_R_USERFNAME	= 6,
	NXS_CHAT_SRV_D_DB_USERS_FIELD_R_USERLNAME	= 7,
	NXS_CHAT_SRV_D_DB_USERS_FIELD_R_USERAPIKEY	= 8,
} nxs_staff_srv_d_db_users_fields_t;

typedef struct
{
	size_t			id;
	size_t			tlgrm_userid;
	size_t			rdmn_userid;
	nxs_string_t		tlgrm_username;
	nxs_string_t		tlgrm_userlang;
	nxs_string_t		rdmn_username;
	nxs_string_t		rdmn_userfname;
	nxs_string_t		rdmn_userlname;
	nxs_string_t		rdmn_user_api_key;
} nxs_chat_srv_d_db_users_el_t;

struct nxs_chat_srv_d_db_users_s
{
	nxs_array_t		users;			/* type: nxs_chat_srv_d_db_users_el_t */
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_d_db_users_t *nxs_chat_srv_d_db_users_init(void)
{
	nxs_chat_srv_d_db_users_t *d_ctx;

	d_ctx = (nxs_chat_srv_d_db_users_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_d_db_users_t));

	nxs_array_init2(&d_ctx->users, nxs_chat_srv_d_db_users_el_t);

	return d_ctx;
}

nxs_chat_srv_d_db_users_t *nxs_chat_srv_d_db_users_free(nxs_chat_srv_d_db_users_t *d_ctx)
{
	nxs_chat_srv_d_db_users_el_t *u;
	size_t                        i;

	if(d_ctx == NULL) {

		return NULL;
	}

	for(i = 0; i < nxs_array_count(&d_ctx->users); i++) {

		u = nxs_array_get(&d_ctx->users, i);

		u->id           = 0;
		u->tlgrm_userid = 0;
		u->rdmn_userid  = 0;

		nxs_string_free(&u->tlgrm_username);
		nxs_string_free(&u->tlgrm_userlang);
		nxs_string_free(&u->rdmn_username);
		nxs_string_free(&u->rdmn_userfname);
		nxs_string_free(&u->rdmn_userlname);
		nxs_string_free(&u->rdmn_user_api_key);
	}

	nxs_array_free(&d_ctx->users);

	return (nxs_chat_srv_d_db_users_t *)nxs_free(d_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_users_select(nxs_chat_srv_d_db_users_t *d_ctx, nxs_mysql_t *mysql_ctx)
{
	nxs_mysql_res_t               res;
	nxs_chat_srv_d_db_users_el_t *u;
	nxs_chat_srv_err_t            rc;
	size_t                        i, count;

	if(d_ctx == NULL || mysql_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_mysql_res_init(&res);

	if(nxs_mysql_query(mysql_ctx,
	                   NXS_MYSQL_QUERY_TYPE_SELECT,
	                   &res,
	                   "SELECT "
	                   "	`id`, "
	                   "	`t_username`, "
	                   "	`t_userid`, "
	                   "	`t_userlang`, "
	                   "	`r_userid`, "
	                   "	`r_username`, "
	                   "	`r_userfname`, "
	                   "	`r_userlname`, "
	                   "	`r_userapikey` "
	                   "FROM "
	                   "	`users`") != NXS_MYSQL_E_OK) {

		nxs_log_write_error(
		        &process, "[%s]: MySQL query select users error: \"%s\"", nxs_proc_get_name(&process), (char *)mysql_ctx->err_str);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if((count = nxs_mysql_res_rows(&res)) == 0) {

		nxs_error(rc, NXS_CHAT_SRV_E_OK, error);
	}

	for(i = 0; i < count; i++) {

		u = nxs_array_add(&d_ctx->users);

		u->id           = nxs_string_atoi(nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_USERS_FIELD_ID));
		u->tlgrm_userid = nxs_string_atoi(nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_USERS_FIELD_T_USERID));
		u->rdmn_userid  = nxs_string_atoi(nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_USERS_FIELD_R_USERID));

		nxs_string_init3(&u->tlgrm_username, nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_USERS_FIELD_T_USERNAME));
		nxs_string_init3(&u->tlgrm_userlang, nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_USERS_FIELD_T_USERLANG));
		nxs_string_init3(&u->rdmn_username, nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_USERS_FIELD_R_USERNAME));
		nxs_string_init3(&u->rdmn_userfname, nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_USERS_FIELD_R_USERFNAME));
		nxs_string_init3(&u->rdmn_userlname, nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_USERS_FIELD_R_USERLNAME));
		nxs_string_init3(&u->rdmn_user_api_key, nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_USERS_FIELD_R_USERAPIKEY));
	}

error:

	nxs_mysql_res_free(&res);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_users_get(nxs_chat_srv_d_db_users_t *d_ctx, size_t i, nxs_chat_srv_m_db_users_t *user)
{
	nxs_chat_srv_d_db_users_el_t *u;

	if(d_ctx == NULL || user == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((u = nxs_array_get(&d_ctx->users, i)) == NULL) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	user->id                = u->id;
	user->tlgrm_userid      = u->tlgrm_userid;
	user->rdmn_userid       = u->rdmn_userid;
	user->tlgrm_username    = &u->tlgrm_username;
	user->tlgrm_userlang    = &u->tlgrm_userlang;
	user->rdmn_username     = &u->rdmn_username;
	user->rdmn_userfname    = &u->rdmn_userfname;
	user->rdmn_userlname    = &u->rdmn_userlname;
	user->rdmn_user_api_key = &u->rdmn_user_api_key;

	return NXS_CHAT_SRV_E_OK;
}

/* Module internal (static) functions */
