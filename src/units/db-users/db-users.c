#ifndef _INCLUDE_NXS_CHAT_SRV_U_DB_USERS_C
#define _INCLUDE_NXS_CHAT_SRV_U_DB_USERS_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/db-users/db-users-typedefs.h>
#include <units/db-users/db-users.h>
#include <units/db-users/db-users-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

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
} nxs_chat_srv_u_db_users_el_t;

struct nxs_chat_srv_u_db_users_s
{
	nxs_array_t		users;			/* type: nxs_chat_srv_u_db_users_el_t */
};

/* Module internal (static) functions prototypes */

// clang-format on

static void nxs_chat_srv_u_db_users_fill(nxs_chat_srv_m_db_users_t *user, nxs_chat_srv_u_db_users_el_t *u);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_u_db_users_t *nxs_chat_srv_u_db_users_init(void)
{
	nxs_chat_srv_u_db_users_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_db_users_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_db_users_t));

	nxs_array_init2(&u_ctx->users, nxs_chat_srv_u_db_users_el_t);

	return u_ctx;
}

nxs_chat_srv_u_db_users_t *nxs_chat_srv_u_db_users_free(nxs_chat_srv_u_db_users_t *u_ctx)
{
	nxs_chat_srv_u_db_users_el_t *u;
	size_t                        i;

	if(u_ctx == NULL) {

		return NULL;
	}

	for(i = 0; i < nxs_array_count(&u_ctx->users); i++) {

		u = nxs_array_get(&u_ctx->users, i);

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

	nxs_array_free(&u_ctx->users);

	return (nxs_chat_srv_u_db_users_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_users_pull(nxs_chat_srv_u_db_users_t *u_ctx)
{
	nxs_mysql_t                   mysql;
	nxs_chat_srv_d_db_users_t *   users_db_ctx;
	nxs_chat_srv_u_db_users_el_t *u;
	nxs_chat_srv_m_db_users_t     user;
	nxs_chat_srv_err_t            rc;
	size_t                        i;

	if(nxs_chat_srv_c_mysql_connect(&mysql) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process, "[%s]: MySQL connect error: \"%s\"", nxs_proc_get_name(&process), mysql.err_str);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	users_db_ctx = nxs_chat_srv_d_db_users_init();

	if(nxs_chat_srv_d_db_users_select(users_db_ctx, &mysql) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	for(i = 0; nxs_chat_srv_d_db_users_get(users_db_ctx, i, &user) == NXS_CHAT_SRV_E_OK; i++) {

		u = nxs_array_add(&u_ctx->users);

		u->id           = user.id;
		u->tlgrm_userid = user.tlgrm_userid;
		u->rdmn_userid  = user.rdmn_userid;

		nxs_string_init3(&u->tlgrm_username, user.tlgrm_username);
		nxs_string_init3(&u->tlgrm_userlang, user.tlgrm_userlang);
		nxs_string_init3(&u->rdmn_username, user.rdmn_username);
		nxs_string_init3(&u->rdmn_userfname, user.rdmn_userfname);
		nxs_string_init3(&u->rdmn_userlname, user.rdmn_userlname);
		nxs_string_init3(&u->rdmn_user_api_key, user.rdmn_user_api_key);
	}

error:

	nxs_chat_srv_c_mysql_disconnect(&mysql);

	users_db_ctx = nxs_chat_srv_d_db_users_free(users_db_ctx);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_users_find_by_tid(nxs_chat_srv_u_db_users_t *u_ctx, size_t tlgrm_id, nxs_chat_srv_m_db_users_t *user)
{
	nxs_chat_srv_u_db_users_el_t *u;
	size_t                        i;

	if(u_ctx == NULL || user == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(tlgrm_id == 0) {

		nxs_chat_srv_u_db_users_fill(user, NULL);

		return NXS_CHAT_SRV_E_EXIST;
	}

	for(i = 0; i < nxs_array_count(&u_ctx->users); i++) {

		u = nxs_array_get(&u_ctx->users, i);

		if(u->tlgrm_userid == tlgrm_id) {

			nxs_chat_srv_u_db_users_fill(user, u);

			return NXS_CHAT_SRV_E_OK;
		}
	}

	nxs_chat_srv_u_db_users_fill(user, NULL);

	return NXS_CHAT_SRV_E_EXIST;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_users_find_by_tusername(nxs_chat_srv_u_db_users_t *u_ctx,
                                                             nxs_string_t *             tlgrm_username,
                                                             nxs_chat_srv_m_db_users_t *user)
{
	nxs_chat_srv_u_db_users_el_t *u;
	size_t                        i;

	if(u_ctx == NULL || user == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(tlgrm_username == NULL || nxs_string_len(tlgrm_username) == 0) {

		nxs_chat_srv_u_db_users_fill(user, NULL);

		return NXS_CHAT_SRV_E_EXIST;
	}

	for(i = 0; i < nxs_array_count(&u_ctx->users); i++) {

		u = nxs_array_get(&u_ctx->users, i);

		if(nxs_string_cmp(&u->tlgrm_username, 0, tlgrm_username, 0) == NXS_STRING_CMP_EQ) {

			nxs_chat_srv_u_db_users_fill(user, u);

			return NXS_CHAT_SRV_E_OK;
		}
	}

	nxs_chat_srv_u_db_users_fill(user, NULL);

	return NXS_CHAT_SRV_E_EXIST;
}

/* Module internal (static) functions */

static void nxs_chat_srv_u_db_users_fill(nxs_chat_srv_m_db_users_t *user, nxs_chat_srv_u_db_users_el_t *u)
{

	if(u == NULL) {

		user->id                = 0;
		user->tlgrm_userid      = 0;
		user->rdmn_userid       = 0;
		user->tlgrm_username    = NULL;
		user->tlgrm_userlang    = NULL;
		user->rdmn_username     = NULL;
		user->rdmn_userfname    = NULL;
		user->rdmn_userlname    = NULL;
		user->rdmn_user_api_key = NULL;
	}
	else {

		user->id                = u->id;
		user->tlgrm_userid      = u->tlgrm_userid;
		user->rdmn_userid       = u->rdmn_userid;
		user->tlgrm_username    = &u->tlgrm_username;
		user->tlgrm_userlang    = &u->tlgrm_userlang;
		user->rdmn_username     = &u->rdmn_username;
		user->rdmn_userfname    = &u->rdmn_userfname;
		user->rdmn_userlname    = &u->rdmn_userlname;
		user->rdmn_user_api_key = &u->rdmn_user_api_key;
	}
}
