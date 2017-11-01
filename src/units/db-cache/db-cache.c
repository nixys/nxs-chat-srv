#ifndef _INCLUDE_NXS_CHAT_SRV_U_DB_CACHE_C
#define _INCLUDE_NXS_CHAT_SRV_U_DB_CACHE_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/db-cache/db-cache-typedefs.h>
#include <units/db-cache/db-cache.h>
#include <units/db-cache/db-cache-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef enum
{
	NXS_CHAT_SRV_U_DB_CACHE_PROJECT_STATUS_NONE	= 0,
	NXS_CHAT_SRV_U_DB_CACHE_PROJECT_STATUS_ACTIVE	= 1,
	NXS_CHAT_SRV_U_DB_CACHE_PROJECT_STATUS_CLOSED	= 5,
	NXS_CHAT_SRV_U_DB_CACHE_PROJECT_STATUS_ARCHIVED	= 9
} nxs_chat_srv_u_db_cache_project_status_t;

typedef struct
{
	size_t						id;
	nxs_chat_srv_u_db_cache_project_status_t	status;
	nxs_string_t					name;
} nxs_chat_srv_u_db_cache_project_t;

typedef struct
{
	size_t						r_id;
	nxs_string_t					r_login;
	nxs_string_t					r_userfname;
	nxs_string_t					r_userlname;
	nxs_string_t					r_tlgrm_username;
} nxs_chat_srv_u_db_cache_user_t;

typedef struct
{
	size_t						id;
	nxs_bool_t					is_default;
	nxs_string_t					name;
} nxs_chat_srv_u_db_cache_issue_priority_t;

struct nxs_chat_srv_u_db_cache_s
{
	nxs_list_t					projects;		/* type: nxs_chat_srv_u_db_cache_project_t */
	nxs_list_t					users;			/* type: nxs_chat_srv_u_db_cache_user_t */
	nxs_array_t					issue_priorities;	/* type: nxs_chat_srv_u_db_cache_issue_priority_t */
};

/* Module internal (static) functions prototypes */

// clang-format on

static void nxs_chat_srv_u_db_cache_projects_init(nxs_list_t *projects);
static void nxs_chat_srv_u_db_cache_projects_free(nxs_list_t *projects);
static void nxs_chat_srv_u_db_cache_projects_clear(nxs_list_t *projects);
static void nxs_chat_srv_u_db_cache_projects_serialize(nxs_list_t *projects, nxs_string_t *out_str);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_projects_deserialize(nxs_list_t *projects, nxs_string_t *json_str);
static nxs_cfg_json_state_t nxs_chat_srv_u_db_cache_projects_deserialize_projects(nxs_process_t *     proc,
                                                                                  nxs_json_t *        json,
                                                                                  nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                  nxs_array_t *       cfg_arr);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_projects_extract(nxs_list_t *projects, nxs_buf_t *json_buf, size_t *total_count);
static nxs_cfg_json_state_t nxs_chat_srv_u_db_cache_projects_extract_projects(nxs_process_t *     proc,
                                                                              nxs_json_t *        json,
                                                                              nxs_cfg_json_par_t *cfg_json_par_el,
                                                                              nxs_array_t *       cfg_arr);
static void nxs_chat_srv_u_db_cache_projects_el_init(nxs_chat_srv_u_db_cache_project_t *project);
static void nxs_chat_srv_u_db_cache_projects_el_free(nxs_chat_srv_u_db_cache_project_t *project);

static void nxs_chat_srv_u_db_cache_users_init(nxs_list_t *users);
static void nxs_chat_srv_u_db_cache_users_free(nxs_list_t *users);
static void nxs_chat_srv_u_db_cache_users_el_init(nxs_chat_srv_u_db_cache_user_t *user);
static void nxs_chat_srv_u_db_cache_users_el_free(nxs_chat_srv_u_db_cache_user_t *user);
static void nxs_chat_srv_u_db_cache_users_clear(nxs_list_t *users);
static void nxs_chat_srv_u_db_cache_users_serialize(nxs_list_t *users, nxs_string_t *out_str);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_users_deserialize(nxs_list_t *users, nxs_string_t *json_str);
static nxs_cfg_json_state_t nxs_chat_srv_u_db_cache_users_deserialize_users(nxs_process_t *     proc,
                                                                            nxs_json_t *        json,
                                                                            nxs_cfg_json_par_t *cfg_json_par_el,
                                                                            nxs_array_t *       cfg_arr);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_users_extract(nxs_list_t *users, nxs_buf_t *json_buf, size_t *total_count);
static nxs_cfg_json_state_t nxs_chat_srv_u_db_cache_users_extract_users(nxs_process_t *     proc,
                                                                        nxs_json_t *        json,
                                                                        nxs_cfg_json_par_t *cfg_json_par_el,
                                                                        nxs_array_t *       cfg_arr);
static nxs_cfg_json_state_t nxs_chat_srv_u_db_cache_users_extract_users_custom_fields(nxs_process_t *     proc,
                                                                                      nxs_json_t *        json,
                                                                                      nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                      nxs_array_t *       cfg_arr);

static void nxs_chat_srv_u_db_cache_issue_priorities_init(nxs_array_t *issue_priorities);
static void nxs_chat_srv_u_db_cache_issue_priorities_free(nxs_array_t *issue_priorities);
static void nxs_chat_srv_u_db_cache_issue_priorities_el_init(nxs_chat_srv_u_db_cache_issue_priority_t *ip);
static void nxs_chat_srv_u_db_cache_issue_priorities_el_free(nxs_chat_srv_u_db_cache_issue_priority_t *ip);
static void nxs_chat_srv_u_db_cache_issue_priorities_clear(nxs_array_t *issue_priorities);
static void nxs_chat_srv_u_db_cache_issue_priorities_serialize(nxs_array_t *issue_priorities, nxs_string_t *out_str);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_issue_priorities_deserialize(nxs_array_t *issue_priorities, nxs_string_t *json_str);
static nxs_cfg_json_state_t nxs_chat_srv_u_db_cache_issue_priorities_deserialize_issue_priorities(nxs_process_t *     proc,
                                                                                                  nxs_json_t *        json,
                                                                                                  nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                                  nxs_array_t *       cfg_arr);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_issue_priorities_extract(nxs_array_t *issue_priorities, nxs_buf_t *json_buf);
static nxs_cfg_json_state_t nxs_chat_srv_u_db_cache_issue_priorities_extract_issue_priorities(nxs_process_t *     proc,
                                                                                              nxs_json_t *        json,
                                                                                              nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                              nxs_array_t *       cfg_arr);

static nxs_cfg_json_state_t
        nxs_chat_srv_u_db_cache_deserialize_base64(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

// clang-format off

/* Module initializations */

static nxs_string_t _s_par_users		= nxs_string("users");
static nxs_string_t _s_par_id			= nxs_string("id");
static nxs_string_t _s_par_login		= nxs_string("login");
static nxs_string_t _s_par_firstname		= nxs_string("firstname");
static nxs_string_t _s_par_lastname		= nxs_string("lastname");
static nxs_string_t _s_par_custom_fields	= nxs_string("custom_fields");
static nxs_string_t _s_par_value		= nxs_string("value");
static nxs_string_t _s_par_total_count		= nxs_string("total_count");
static nxs_string_t _s_par_projects		= nxs_string("projects");
static nxs_string_t _s_par_name			= nxs_string("name");
static nxs_string_t _s_par_status		= nxs_string("status");
static nxs_string_t _s_par_issue_priorities	= nxs_string("issue_priorities");
static nxs_string_t _s_par_r_login		= nxs_string("r_login");
static nxs_string_t _s_par_r_tlgrm_username	= nxs_string("r_tlgrm_username");
static nxs_string_t _s_par_r_userfname		= nxs_string("r_userfname");
static nxs_string_t _s_par_r_userlname		= nxs_string("r_userlname");
static nxs_string_t _s_par_is_default		= nxs_string("is_default");

/* Module global functions */

// clang-format on

nxs_chat_srv_u_db_cache_t *nxs_chat_srv_u_db_cache_init(void)
{
	nxs_chat_srv_u_db_cache_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_db_cache_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_db_cache_t));

	nxs_chat_srv_u_db_cache_projects_init(&u_ctx->projects);
	nxs_chat_srv_u_db_cache_users_init(&u_ctx->users);
	nxs_chat_srv_u_db_cache_issue_priorities_init(&u_ctx->issue_priorities);

	return u_ctx;
}

nxs_chat_srv_u_db_cache_t *nxs_chat_srv_u_db_cache_free(nxs_chat_srv_u_db_cache_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	nxs_chat_srv_u_db_cache_projects_free(&u_ctx->projects);
	nxs_chat_srv_u_db_cache_users_free(&u_ctx->users);
	nxs_chat_srv_u_db_cache_issue_priorities_free(&u_ctx->issue_priorities);

	return (nxs_chat_srv_u_db_cache_t *)nxs_free(u_ctx);
}

/* pull cache from Redis */
nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_pull(nxs_chat_srv_u_db_cache_t *u_ctx)
{
	nxs_chat_srv_err_t         rc;
	nxs_chat_srv_d_db_cache_t *db_cache_ctx;
	nxs_string_t               value;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	db_cache_ctx = nxs_chat_srv_d_db_cache_init();

	nxs_string_init(&value);

	/* projects */
	if(nxs_chat_srv_d_db_cache_get(db_cache_ctx, NXS_CHAT_SRV_D_DB_CACHE_TYPE_PROJECTS, &value) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_db_cache_projects_deserialize(&u_ctx->projects, &value) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	/* users */
	if(nxs_chat_srv_d_db_cache_get(db_cache_ctx, NXS_CHAT_SRV_D_DB_CACHE_TYPE_USERS, &value) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_db_cache_users_deserialize(&u_ctx->users, &value) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	/* issue_priorities */
	if(nxs_chat_srv_d_db_cache_get(db_cache_ctx, NXS_CHAT_SRV_D_DB_CACHE_TYPE_ISSUE_PRIORITIES, &value) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_db_cache_issue_priorities_deserialize(&u_ctx->issue_priorities, &value) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	db_cache_ctx = nxs_chat_srv_d_db_cache_free(db_cache_ctx);

	nxs_string_free(&value);

	return rc;
}

/* update 'projects' Redis cache from Redmine */
nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_update_projects(nxs_chat_srv_u_db_cache_t *u_ctx)
{
	nxs_chat_srv_err_t                 rc;
	nxs_buf_t                          out_buf;
	nxs_string_t                       serialized_str;
	nxs_chat_srv_u_db_cache_project_t *p;
	nxs_chat_srv_d_db_cache_t *        db_cache_ctx;
	size_t                             total_count, offset;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_buf_init2(&out_buf);

	nxs_string_init(&serialized_str);

	db_cache_ctx = nxs_chat_srv_d_db_cache_init();

	/* clean up context */
	nxs_chat_srv_u_db_cache_projects_clear(&u_ctx->projects);

	/*
	 * get data from Redmine
	 */

	/* Redmine projects */
	offset = 0;

	do {

		if(nxs_chat_srv_d_rdmn_projects_get(
		           &nxs_chat_srv_cfg.rdmn.api_key, offset, NXS_CHAT_SRV_RDMN_QUERY_LIMIT, NULL, &out_buf) != NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		if(nxs_chat_srv_u_db_cache_projects_extract(&u_ctx->projects, &out_buf, &total_count)) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		offset += NXS_CHAT_SRV_RDMN_QUERY_LIMIT;
	} while(total_count > offset);

	/* remove from lists inactive values */

	for(p = nxs_list_ptr_init(&u_ctx->projects, NXS_LIST_PTR_INIT_HEAD); p != NULL;) {

		if(p->status != NXS_CHAT_SRV_U_DB_CACHE_PROJECT_STATUS_ACTIVE) {

			nxs_chat_srv_u_db_cache_projects_el_free(p);

			p = nxs_list_del(&u_ctx->projects, NXS_LIST_MOVE_NEXT);
		}
		else {

			p = nxs_list_ptr_next(&u_ctx->projects);
		}
	}

	/*
	 * push context into Redis
	 */

	/* projects */
	nxs_chat_srv_u_db_cache_projects_serialize(&u_ctx->projects, &serialized_str);

	if(nxs_chat_srv_d_db_cache_put(db_cache_ctx, NXS_CHAT_SRV_D_DB_CACHE_TYPE_PROJECTS, &serialized_str) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_string_free(&serialized_str);

	nxs_buf_free(&out_buf);

	db_cache_ctx = nxs_chat_srv_d_db_cache_free(db_cache_ctx);

	return rc;
}

/* update 'users' Redis cache from Redmine */
nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_update_users(nxs_chat_srv_u_db_cache_t *u_ctx)
{
	nxs_chat_srv_err_t rc;
	nxs_buf_t          out_buf;
	nxs_string_t       serialized_str;
	//	nxs_chat_srv_u_db_cache_user_t *u;
	nxs_chat_srv_d_db_cache_t *db_cache_ctx;
	size_t                     total_count, offset;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_buf_init2(&out_buf);

	nxs_string_init(&serialized_str);

	db_cache_ctx = nxs_chat_srv_d_db_cache_init();

	/* clean up context */
	nxs_chat_srv_u_db_cache_users_clear(&u_ctx->users);

	/*
	 * get data from Redmine
	 */

	/* Redmine users */
	offset = 0;

	do {

		if(nxs_chat_srv_d_rdmn_users_get(
		           0, NXS_NO, &nxs_chat_srv_cfg.rdmn.api_key, offset, NXS_CHAT_SRV_RDMN_QUERY_LIMIT, NULL, &out_buf) !=
		   NXS_CHAT_SRV_E_OK) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		if(nxs_chat_srv_u_db_cache_users_extract(&u_ctx->users, &out_buf, &total_count)) {

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
		}

		offset += NXS_CHAT_SRV_RDMN_QUERY_LIMIT;
	} while(total_count > offset);

	/* remove inactive values from list */

	/* TODO: add remove from list inactive users
	for(u = nxs_list_ptr_init(&u_ctx->users, NXS_LIST_PTR_INIT_HEAD); u != NULL;) {

	        if(nxs_string_len(&u->r_tlgrm_username) == 0) {

	                nxs_chat_srv_u_db_cache_users_el_free(u);

	                u = nxs_list_del(&u_ctx->users, NXS_LIST_MOVE_NEXT);
	        }
	        else {

	                u = nxs_list_ptr_next(&u_ctx->users);
	        }
	}
	*/

	/*
	 * push context into Redis
	 */

	/* users */
	nxs_chat_srv_u_db_cache_users_serialize(&u_ctx->users, &serialized_str);

	if(nxs_chat_srv_d_db_cache_put(db_cache_ctx, NXS_CHAT_SRV_D_DB_CACHE_TYPE_USERS, &serialized_str) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_string_free(&serialized_str);

	nxs_buf_free(&out_buf);

	db_cache_ctx = nxs_chat_srv_d_db_cache_free(db_cache_ctx);

	return rc;
}

/* update 'issue priorities' Redis cache from Redmine */
nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_update_issue_priorities(nxs_chat_srv_u_db_cache_t *u_ctx)
{
	nxs_chat_srv_err_t         rc;
	nxs_buf_t                  out_buf;
	nxs_string_t               serialized_str;
	nxs_chat_srv_d_db_cache_t *db_cache_ctx;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_buf_init2(&out_buf);

	nxs_string_init(&serialized_str);

	db_cache_ctx = nxs_chat_srv_d_db_cache_init();

	/* clean up context */
	nxs_chat_srv_u_db_cache_issue_priorities_clear(&u_ctx->issue_priorities);

	/*
	 * get data from Redmine
	 */

	/* Redmine enums issue priorities */
	if(nxs_chat_srv_d_rdmn_enums_issue_priorities_get(&nxs_chat_srv_cfg.rdmn.api_key, NULL, &out_buf) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_db_cache_issue_priorities_extract(&u_ctx->issue_priorities, &out_buf)) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	/*
	 * push context into Redis
	 */

	/* issue priorities */
	nxs_chat_srv_u_db_cache_issue_priorities_serialize(&u_ctx->issue_priorities, &serialized_str);

	if(nxs_chat_srv_d_db_cache_put(db_cache_ctx, NXS_CHAT_SRV_D_DB_CACHE_TYPE_ISSUE_PRIORITIES, &serialized_str) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_string_free(&serialized_str);

	nxs_buf_free(&out_buf);

	db_cache_ctx = nxs_chat_srv_d_db_cache_free(db_cache_ctx);

	return rc;
}

/* check active projects */
nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_projects_get(nxs_chat_srv_u_db_cache_t *u_ctx, nxs_array_t *projects)
{
	nxs_chat_srv_u_db_cache_project_t *p;
	nxs_chat_srv_m_db_cache_project_t *mp;

	if(u_ctx == NULL || projects == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	for(p = nxs_list_ptr_init(&u_ctx->projects, NXS_LIST_PTR_INIT_HEAD); p != NULL; p = nxs_list_ptr_next(&u_ctx->projects)) {

		mp = nxs_array_add(projects);

		mp->id   = p->id;
		mp->name = &p->name;
	}

	return NXS_CHAT_SRV_E_OK;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_user_get(nxs_chat_srv_u_db_cache_t *u_ctx,
                                                    nxs_string_t *             tlgrm_username,
                                                    nxs_chat_srv_m_user_ctx_t *user_ctx)
{
	nxs_chat_srv_u_db_cache_user_t *u;

	if(u_ctx == NULL || tlgrm_username == NULL || user_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	for(u = nxs_list_ptr_init(&u_ctx->users, NXS_LIST_PTR_INIT_HEAD); u != NULL; u = nxs_list_ptr_next(&u_ctx->users)) {

		if(nxs_string_cmp(&u->r_tlgrm_username, 0, tlgrm_username, 0) == NXS_YES) {

			user_ctx->r_userid = u->r_id;

			nxs_string_clone(&user_ctx->t_username, &u->r_tlgrm_username);
			nxs_string_clone(&user_ctx->r_username, &u->r_login);
			nxs_string_clone(&user_ctx->r_userfname, &u->r_userfname);
			nxs_string_clone(&user_ctx->r_userlname, &u->r_userlname);

			return NXS_CHAT_SRV_E_OK;
		}
	}

	return NXS_CHAT_SRV_E_EXIST;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_user_get_by_rdmn_id(nxs_chat_srv_u_db_cache_t *u_ctx,
                                                               size_t                     rdmn_userid,
                                                               nxs_chat_srv_m_user_ctx_t *user_ctx)
{
	nxs_chat_srv_u_db_cache_user_t *u;

	if(u_ctx == NULL || user_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	for(u = nxs_list_ptr_init(&u_ctx->users, NXS_LIST_PTR_INIT_HEAD); u != NULL; u = nxs_list_ptr_next(&u_ctx->users)) {

		if(u->r_id == rdmn_userid) {

			user_ctx->r_userid = u->r_id;

			nxs_string_clone(&user_ctx->t_username, &u->r_tlgrm_username);
			nxs_string_clone(&user_ctx->r_username, &u->r_login);
			nxs_string_clone(&user_ctx->r_userfname, &u->r_userfname);
			nxs_string_clone(&user_ctx->r_userlname, &u->r_userlname);

			return NXS_CHAT_SRV_E_OK;
		}
	}

	return NXS_CHAT_SRV_E_EXIST;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_prio_get(nxs_chat_srv_u_db_cache_t *u_ctx, nxs_array_t *priorities)
{
	nxs_chat_srv_u_db_cache_issue_priority_t *ip;
	nxs_chat_srv_m_db_cache_issue_priority_t *issue_priority;
	size_t                                    i;

	if(u_ctx == NULL || priorities == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	for(i = 0; i < nxs_array_count(&u_ctx->issue_priorities); i++) {

		ip = nxs_array_get(&u_ctx->issue_priorities, i);

		issue_priority = nxs_array_add(priorities);

		issue_priority->id         = ip->id;
		issue_priority->name       = &ip->name;
		issue_priority->is_default = ip->is_default;
	}

	return NXS_CHAT_SRV_E_OK;
}

/* get default issue priority */
nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_prio_get_default(nxs_chat_srv_u_db_cache_t *               u_ctx,
                                                            nxs_chat_srv_m_db_cache_issue_priority_t *issue_priority)
{
	nxs_chat_srv_u_db_cache_issue_priority_t *ip;
	size_t                                    i;

	if(u_ctx == NULL || issue_priority == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	for(i = 0; i < nxs_array_count(&u_ctx->issue_priorities); i++) {

		ip = nxs_array_get(&u_ctx->issue_priorities, i);

		if(ip->is_default == NXS_YES) {

			issue_priority->id         = ip->id;
			issue_priority->name       = &ip->name;
			issue_priority->is_default = ip->is_default;

			return NXS_CHAT_SRV_E_OK;
		}
	}

	return NXS_CHAT_SRV_E_EXIST;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_prio_get_by_id(nxs_chat_srv_u_db_cache_t *               u_ctx,
                                                          size_t                                    prio_id,
                                                          nxs_chat_srv_m_db_cache_issue_priority_t *issue_priority)
{
	nxs_chat_srv_u_db_cache_issue_priority_t *ip;
	size_t                                    i;

	if(u_ctx == NULL || issue_priority == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	for(i = 0; i < nxs_array_count(&u_ctx->issue_priorities); i++) {

		ip = nxs_array_get(&u_ctx->issue_priorities, i);

		if(ip->id == prio_id) {

			issue_priority->id         = ip->id;
			issue_priority->name       = &ip->name;
			issue_priority->is_default = ip->is_default;

			return NXS_CHAT_SRV_E_OK;
		}
	}

	return NXS_CHAT_SRV_E_EXIST;
}

/* Module internal (static) functions */

static void nxs_chat_srv_u_db_cache_projects_init(nxs_list_t *projects)
{

	if(projects == NULL) {

		return;
	}

	nxs_list_init2(projects, nxs_chat_srv_u_db_cache_project_t);
}

static void nxs_chat_srv_u_db_cache_projects_free(nxs_list_t *projects)
{
	nxs_chat_srv_u_db_cache_project_t *p;

	if(projects == NULL) {

		return;
	}

	for(p = nxs_list_ptr_init(projects, NXS_LIST_PTR_INIT_HEAD); p != NULL; p = nxs_list_del(projects, NXS_LIST_MOVE_NEXT)) {

		nxs_chat_srv_u_db_cache_projects_el_free(p);
	}

	nxs_list_free(projects);
}

static void nxs_chat_srv_u_db_cache_projects_el_init(nxs_chat_srv_u_db_cache_project_t *project)
{

	if(project == NULL) {

		return;
	}

	project->id     = 0;
	project->status = NXS_CHAT_SRV_U_DB_CACHE_PROJECT_STATUS_NONE;

	nxs_string_init(&project->name);
}

static void nxs_chat_srv_u_db_cache_projects_el_free(nxs_chat_srv_u_db_cache_project_t *project)
{

	if(project == NULL) {

		return;
	}

	project->id     = 0;
	project->status = NXS_CHAT_SRV_U_DB_CACHE_PROJECT_STATUS_NONE;

	nxs_string_free(&project->name);
}

static void nxs_chat_srv_u_db_cache_projects_clear(nxs_list_t *projects)
{
	nxs_chat_srv_u_db_cache_project_t *p;

	if(projects == NULL) {

		return;
	}

	for(p = nxs_list_ptr_init(projects, NXS_LIST_PTR_INIT_HEAD); p != NULL; p = nxs_list_del(projects, NXS_LIST_MOVE_NEXT)) {

		nxs_chat_srv_u_db_cache_projects_el_free(p);
	}
}

static void nxs_chat_srv_u_db_cache_projects_serialize(nxs_list_t *projects, nxs_string_t *out_str)
{
	nxs_chat_srv_u_db_cache_project_t *p;
	nxs_string_t                       str, serialized_name;
	nxs_bool_t                         f;

	if(projects == NULL || out_str == NULL) {

		return;
	}

	nxs_string_init(&str);
	nxs_string_init(&serialized_name);

	for(f = NXS_NO, p = nxs_list_ptr_init(projects, NXS_LIST_PTR_INIT_HEAD); p != NULL; p = nxs_list_ptr_next(projects)) {

		if(f == NXS_YES) {

			nxs_string_char_add_char(&str, (u_char)',');
		}
		else {

			f = NXS_YES;
		}

		nxs_base64_encode_string(&serialized_name, &p->name);

		nxs_string_printf2_cat(&str, "{\"%r\":%zu,\"%r\":\"%r\"}", &_s_par_id, p->id, &_s_par_name, &serialized_name);
	}

	nxs_string_printf(out_str, "{\"%r\":[%r]}", &_s_par_projects, &str);

	nxs_string_free(&str);
	nxs_string_free(&serialized_name);
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_projects_deserialize(nxs_list_t *projects, nxs_string_t *json_str)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	if(projects == NULL || json_str == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_projects,	projects,	NULL,	&nxs_chat_srv_u_db_cache_projects_deserialize_projects,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, (nxs_buf_t *)json_str) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: db-cache unit error: parse Redis projects error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_u_db_cache_projects_deserialize_projects(nxs_process_t *     proc,
                                                                                  nxs_json_t *        json,
                                                                                  nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                  nxs_array_t *       cfg_arr)
{
	nxs_list_t *                       projects = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_u_db_cache_project_t *p;

	p = nxs_list_add_tail(projects);

	nxs_chat_srv_u_db_cache_projects_el_init(p);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_id,		&p->id,		NULL,						NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_name,		&p->name,	&nxs_chat_srv_u_db_cache_deserialize_base64,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_projects_extract(nxs_list_t *projects, nxs_buf_t *json_buf, size_t *total_count)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_projects,	projects,	NULL,	&nxs_chat_srv_u_db_cache_projects_extract_projects,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_total_count,	total_count,	NULL,	NULL,							NXS_CFG_JSON_TYPE_INT,			0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: db-cache unit error: parse rdmn projects error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_u_db_cache_projects_extract_projects(nxs_process_t *     proc,
                                                                              nxs_json_t *        json,
                                                                              nxs_cfg_json_par_t *cfg_json_par_el,
                                                                              nxs_array_t *       cfg_arr)
{
	nxs_list_t *                       projects = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_u_db_cache_project_t *p;

	p = nxs_list_add_tail(projects);

	nxs_chat_srv_u_db_cache_projects_el_init(p);

	nxs_cfg_json_conf_array_skip_undef(cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_id,		&p->id,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_name,		&p->name,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_status,		&p->status,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT_32,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static void nxs_chat_srv_u_db_cache_users_init(nxs_list_t *users)
{

	if(users == NULL) {

		return;
	}

	nxs_list_init2(users, nxs_chat_srv_u_db_cache_user_t);
}

static void nxs_chat_srv_u_db_cache_users_free(nxs_list_t *users)
{
	nxs_chat_srv_u_db_cache_user_t *u;

	if(users == NULL) {

		return;
	}

	for(u = nxs_list_ptr_init(users, NXS_LIST_PTR_INIT_HEAD); u != NULL; u = nxs_list_del(users, NXS_LIST_MOVE_NEXT)) {

		nxs_chat_srv_u_db_cache_users_el_free(u);
	}

	nxs_list_free(users);
}

static void nxs_chat_srv_u_db_cache_users_el_init(nxs_chat_srv_u_db_cache_user_t *user)
{

	if(user == NULL) {

		return;
	}

	user->r_id = 0;

	nxs_string_init(&user->r_login);
	nxs_string_init(&user->r_userfname);
	nxs_string_init(&user->r_userlname);
	nxs_string_init(&user->r_tlgrm_username);
}

static void nxs_chat_srv_u_db_cache_users_el_free(nxs_chat_srv_u_db_cache_user_t *user)
{

	if(user == NULL) {

		return;
	}

	user->r_id = 0;

	nxs_string_free(&user->r_login);
	nxs_string_free(&user->r_userfname);
	nxs_string_free(&user->r_userlname);
	nxs_string_free(&user->r_tlgrm_username);
}

static void nxs_chat_srv_u_db_cache_users_clear(nxs_list_t *users)
{
	nxs_chat_srv_u_db_cache_user_t *u;

	if(users == NULL) {

		return;
	}

	for(u = nxs_list_ptr_init(users, NXS_LIST_PTR_INIT_HEAD); u != NULL; u = nxs_list_del(users, NXS_LIST_MOVE_NEXT)) {

		nxs_chat_srv_u_db_cache_users_el_free(u);
	}
}

static void nxs_chat_srv_u_db_cache_users_serialize(nxs_list_t *users, nxs_string_t *out_str)
{
	nxs_chat_srv_u_db_cache_user_t *u;
	nxs_string_t str, serialized_r_login, serialized_r_tlgrm_username, serialized_r_userfname, serialized_r_userlname;
	nxs_bool_t   f;

	if(users == NULL || out_str == NULL) {

		return;
	}

	nxs_string_init(&str);
	nxs_string_init(&serialized_r_login);
	nxs_string_init(&serialized_r_tlgrm_username);
	nxs_string_init(&serialized_r_userfname);
	nxs_string_init(&serialized_r_userlname);

	for(f = NXS_NO, u = nxs_list_ptr_init(users, NXS_LIST_PTR_INIT_HEAD); u != NULL; u = nxs_list_ptr_next(users)) {

		if(f == NXS_YES) {

			nxs_string_char_add_char(&str, (u_char)',');
		}
		else {

			f = NXS_YES;
		}

		nxs_base64_encode_string(&serialized_r_login, &u->r_login);
		nxs_base64_encode_string(&serialized_r_tlgrm_username, &u->r_tlgrm_username);
		nxs_base64_encode_string(&serialized_r_userfname, &u->r_userfname);
		nxs_base64_encode_string(&serialized_r_userlname, &u->r_userlname);

		nxs_string_printf2_cat(&str,
		                       "{\"%r\":%zu,\"%r\":\"%r\",\"%r\":\"%r\",\"%r\":\"%r\",\"%r\":\"%r\"}",
		                       &_s_par_id,
		                       u->r_id,
		                       &_s_par_r_login,
		                       &serialized_r_login,
		                       &_s_par_r_tlgrm_username,
		                       &serialized_r_tlgrm_username,
		                       &_s_par_r_userfname,
		                       &serialized_r_userfname,
		                       &_s_par_r_userlname,
		                       &serialized_r_userlname);
	}

	nxs_string_printf(out_str, "{\"%r\":[%r]}", &_s_par_users, &str);

	nxs_string_free(&str);
	nxs_string_free(&serialized_r_login);
	nxs_string_free(&serialized_r_tlgrm_username);
	nxs_string_free(&serialized_r_userfname);
	nxs_string_free(&serialized_r_userlname);
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_users_deserialize(nxs_list_t *users, nxs_string_t *json_str)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	if(users == NULL || json_str == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_users,	users,	NULL,	&nxs_chat_srv_u_db_cache_users_deserialize_users,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, (nxs_buf_t *)json_str) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: db-cache unit error: parse Redis users error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_u_db_cache_users_deserialize_users(nxs_process_t *     proc,
                                                                            nxs_json_t *        json,
                                                                            nxs_cfg_json_par_t *cfg_json_par_el,
                                                                            nxs_array_t *       cfg_arr)
{
	nxs_list_t *                    users = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_u_db_cache_user_t *u;

	u = nxs_list_add_tail(users);

	nxs_chat_srv_u_db_cache_users_el_init(u);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_id,			&u->r_id,		NULL,						NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_r_login,		&u->r_login,		&nxs_chat_srv_u_db_cache_deserialize_base64,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_r_tlgrm_username,	&u->r_tlgrm_username,	&nxs_chat_srv_u_db_cache_deserialize_base64,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_r_userfname,		&u->r_userfname,	&nxs_chat_srv_u_db_cache_deserialize_base64,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_r_userlname,		&u->r_userlname,	&nxs_chat_srv_u_db_cache_deserialize_base64,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_users_extract(nxs_list_t *users, nxs_buf_t *json_buf, size_t *total_count)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_users,		users,		NULL,	&nxs_chat_srv_u_db_cache_users_extract_users,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_total_count,	total_count,	NULL,	NULL,						NXS_CFG_JSON_TYPE_INT,			0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: db-cache unit error: parse rdmn users error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_u_db_cache_users_extract_users(nxs_process_t *     proc,
                                                                        nxs_json_t *        json,
                                                                        nxs_cfg_json_par_t *cfg_json_par_el,
                                                                        nxs_array_t *       cfg_arr)
{
	nxs_list_t *                    users = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_u_db_cache_user_t *u;

	u = nxs_list_add_tail(users);

	nxs_chat_srv_u_db_cache_users_el_init(u);

	nxs_cfg_json_conf_array_skip_undef(cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_id,		&u->r_id,		NULL,	NULL,								NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_login,		&u->r_login,		NULL,	NULL,								NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_firstname,	&u->r_userfname,	NULL,	NULL,								NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_lastname,	&u->r_userlname,	NULL,	NULL,								NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_custom_fields,	&u->r_tlgrm_username,	NULL,	&nxs_chat_srv_u_db_cache_users_extract_users_custom_fields,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t nxs_chat_srv_u_db_cache_users_extract_users_custom_fields(nxs_process_t *     proc,
                                                                                      nxs_json_t *        json,
                                                                                      nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                      nxs_array_t *       cfg_arr)
{
	nxs_string_t *r_tlgrm_username = nxs_cfg_json_get_val(cfg_json_par_el), *s;
	nxs_json_t *  j;

	if((j = nxs_json_child_get_by_key(json, nxs_string_str(&_s_par_id))) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: db-cache unit error: parse rdmn users custom fields error, missing filed \"%r\"",
		                    nxs_proc_get_name(&process),
		                    &_s_par_id);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(nxs_json_type_get(j) != NXS_JSON_TYPE_INTEGER) {

		nxs_log_write_error(
		        &process,
		        "[%s]: db-cache unit error: parse rdmn users custom fields error, expected integer type for filed \"%r\"",
		        nxs_proc_get_name(&process),
		        &_s_par_id);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if((size_t)nxs_json_integer_val(j) == nxs_chat_srv_cfg.cache.rdmn_cf_telegram &&
	   (j = nxs_json_child_get_by_key(json, nxs_string_str(&_s_par_value))) != NULL) {

		/* needed custom field with value */

		if(nxs_json_type_get(j) != NXS_JSON_TYPE_STRING) {

			nxs_log_write_error(
			        &process,
			        "[%s]: db-cache unit error: parse rdmn users custom fields error, expected string type for filed \"%r\"",
			        nxs_proc_get_name(&process),
			        &_s_par_value);

			return NXS_CFG_JSON_CONF_ERROR;
		}

		s = nxs_json_string_val(j);

		if(nxs_string_get_char(s, 0) == (u_char)'@') {

			nxs_string_cpy(r_tlgrm_username, 0, s, 1);
		}
		else {

			nxs_string_clone(r_tlgrm_username, s);
		}
	}

	return NXS_CFG_JSON_CONF_OK;
}

static void nxs_chat_srv_u_db_cache_issue_priorities_init(nxs_array_t *issue_priorities)
{

	if(issue_priorities == NULL) {

		return;
	}

	nxs_array_init2(issue_priorities, nxs_chat_srv_u_db_cache_issue_priority_t);
}

static void nxs_chat_srv_u_db_cache_issue_priorities_free(nxs_array_t *issue_priorities)
{
	nxs_chat_srv_u_db_cache_issue_priority_t *ip;
	size_t                                    i;

	if(issue_priorities == NULL) {

		return;
	}

	for(i = 0; i < nxs_array_count(issue_priorities); i++) {

		ip = nxs_array_get(issue_priorities, i);

		nxs_chat_srv_u_db_cache_issue_priorities_el_free(ip);
	}

	nxs_array_free(issue_priorities);
}

static void nxs_chat_srv_u_db_cache_issue_priorities_el_init(nxs_chat_srv_u_db_cache_issue_priority_t *ip)
{

	if(ip == NULL) {

		return;
	}

	ip->id         = 0;
	ip->is_default = NXS_NO;

	nxs_string_init(&ip->name);
}

static void nxs_chat_srv_u_db_cache_issue_priorities_el_free(nxs_chat_srv_u_db_cache_issue_priority_t *ip)
{

	if(ip == NULL) {

		return;
	}

	ip->id         = 0;
	ip->is_default = NXS_NO;

	nxs_string_free(&ip->name);
}

static void nxs_chat_srv_u_db_cache_issue_priorities_clear(nxs_array_t *issue_priorities)
{
	nxs_chat_srv_u_db_cache_issue_priority_t *ip;
	size_t                                    i;

	if(issue_priorities == NULL) {

		return;
	}

	for(i = 0; i < nxs_array_count(issue_priorities); i++) {

		ip = nxs_array_get(issue_priorities, i);

		nxs_chat_srv_u_db_cache_issue_priorities_el_free(ip);
	}

	nxs_array_clear(issue_priorities);
}

static void nxs_chat_srv_u_db_cache_issue_priorities_serialize(nxs_array_t *issue_priorities, nxs_string_t *out_str)
{
	nxs_chat_srv_u_db_cache_issue_priority_t *ip;
	nxs_string_t                              str, serialized_name;
	size_t                                    i;

	if(issue_priorities == NULL || out_str == NULL) {

		return;
	}

	nxs_string_init(&str);
	nxs_string_init(&serialized_name);

	for(i = 0; i < nxs_array_count(issue_priorities); i++) {

		ip = nxs_array_get(issue_priorities, i);

		if(i > 0) {

			nxs_string_char_add_char(&str, (u_char)',');
		}

		nxs_base64_encode_string(&serialized_name, &ip->name);

		nxs_string_printf2_cat(&str,
		                       "{\"%r\":%zu,\"%r\":\"%r\",\"%r\":%s}",
		                       &_s_par_id,
		                       ip->id,
		                       &_s_par_name,
		                       &serialized_name,
		                       &_s_par_is_default,
		                       ip->is_default == NXS_YES ? "true" : "false");
	}

	nxs_string_printf(out_str, "{\"%r\":[%r]}", &_s_par_issue_priorities, &str);

	nxs_string_free(&str);
	nxs_string_free(&serialized_name);
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_issue_priorities_deserialize(nxs_array_t *issue_priorities, nxs_string_t *json_str)
{
	nxs_chat_srv_err_t                        rc;
	nxs_cfg_json_t                            cfg_json;
	nxs_array_t                               cfg_arr;
	nxs_chat_srv_u_db_cache_issue_priority_t *ip;
	nxs_bool_t                                f;
	size_t                                    i;

	if(issue_priorities == NULL || json_str == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_issue_priorities,	issue_priorities,	NULL,	&nxs_chat_srv_u_db_cache_issue_priorities_deserialize_issue_priorities,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, (nxs_buf_t *)json_str) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: db-cache unit error: parse Redis issue priorities error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	if(rc == NXS_CHAT_SRV_E_OK) {

		for(i = 0; i < nxs_array_count(issue_priorities); i++) {

			ip = nxs_array_get(issue_priorities, i);

			if(ip->is_default == NXS_YES) {

				f = NXS_YES;

				break;
			}
		}

		if(f == NXS_NO) {

			nxs_log_write_error(&process,
			                    "[%s]: db-cache unit error: does not exist default issue priorities in Redis",
			                    nxs_proc_get_name(&process));

			rc = NXS_CHAT_SRV_E_ERR;
		}
	}

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_u_db_cache_issue_priorities_deserialize_issue_priorities(nxs_process_t *     proc,
                                                                                                  nxs_json_t *        json,
                                                                                                  nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                                  nxs_array_t *       cfg_arr)
{
	nxs_array_t *                             issue_priorities = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_u_db_cache_issue_priority_t *ip;

	ip = nxs_array_add(issue_priorities);

	nxs_chat_srv_u_db_cache_issue_priorities_el_init(ip);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_id,		&ip->id,		NULL,						NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_is_default,	&ip->is_default,	NULL,						NULL,	NXS_CFG_JSON_TYPE_BOOL,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_name,		&ip->name,		&nxs_chat_srv_u_db_cache_deserialize_base64,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_cache_issue_priorities_extract(nxs_array_t *issue_priorities, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t                        rc;
	nxs_cfg_json_t                            cfg_json;
	nxs_array_t                               cfg_arr;
	nxs_chat_srv_u_db_cache_issue_priority_t *ip;
	size_t                                    i;
	nxs_bool_t                                f;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_issue_priorities,	issue_priorities,	NULL,	&nxs_chat_srv_u_db_cache_issue_priorities_extract_issue_priorities,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: db-cache unit error: parse rdmn issue priorities error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	if(rc == NXS_CHAT_SRV_E_OK) {

		for(i = 0; i < nxs_array_count(issue_priorities); i++) {

			ip = nxs_array_get(issue_priorities, i);

			if(ip->is_default == NXS_YES) {

				f = NXS_YES;

				break;
			}
		}

		if(f == NXS_NO) {

			nxs_log_write_error(&process,
			                    "[%s]: db-cache unit error: does not exist default issue priorities in Redmine",
			                    nxs_proc_get_name(&process));

			rc = NXS_CHAT_SRV_E_ERR;
		}
	}

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_u_db_cache_issue_priorities_extract_issue_priorities(nxs_process_t *     proc,
                                                                                              nxs_json_t *        json,
                                                                                              nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                              nxs_array_t *       cfg_arr)
{
	nxs_array_t *                             issue_priorities = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_u_db_cache_issue_priority_t *ip;

	ip = nxs_array_add(issue_priorities);

	nxs_chat_srv_u_db_cache_issue_priorities_el_init(ip);

	nxs_cfg_json_conf_array_skip_undef(cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_id,		&ip->id,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_is_default,	&ip->is_default,	NULL,	NULL,	NXS_CFG_JSON_TYPE_BOOL,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_name,		&ip->name,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_u_db_cache_deserialize_base64(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_string_t *str = nxs_cfg_json_get_val(cfg_json_par_el);

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_STRING) {

		nxs_log_write_error(&process,
		                    "[%s]: db-cache unit error: parse Redis projects error, wrong type for value",
		                    nxs_proc_get_name(&process));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	nxs_base64_decode_string(str, nxs_json_string_val(json));

	return NXS_CFG_JSON_CONF_OK;
}
