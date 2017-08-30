#ifndef _INCLUDE_NXS_CHAT_SRV_U_QUEUE_CACHE_C
#define _INCLUDE_NXS_CHAT_SRV_U_QUEUE_CACHE_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/queue-cache/queue-cache-typedefs.h>
#include <units/queue-cache/queue-cache.h>
#include <units/queue-cache/queue-cache-subunits.h>

/* Definitions */

#define	DEFAULT_CACHE_TTL	300

/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	size_t			id;
	nxs_string_t		subject;
} issue_t;

typedef struct
{
	size_t			id;
	nxs_string_t		name;
	int			status;
} membership_t;

typedef struct
{
	time_t			memberships_ttl;
	time_t			last_issue_ttl;
	time_t			watch_issues_ttl;

	size_t			id;
	nxs_string_t		login;
	nxs_string_t		firstname;
	nxs_string_t		lastname;
	nxs_string_t		mail;
	nxs_string_t		api_key;

	nxs_list_t		memberships;			/* type: membership_t */

	issue_t			last_issue;
	nxs_list_t		watch_issues;			/* type: issue_t */
} user_t;

typedef struct
{
	size_t			id;
	nxs_string_t		name;
	nxs_bool_t		is_default;
} issue_priority_t;

typedef struct
{
	time_t			ttl;
	nxs_array_t		els;				/* type: issue_priority_t */
} issue_priorities_t;

struct nxs_chat_srv_u_queue_cache_s
{
	nxs_list_t		users;				/* type: user_t */
	issue_priorities_t	priorities;
};

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_err_t pull_user(user_t *user, size_t user_id);
static nxs_chat_srv_err_t pull_user_extract(user_t *user, nxs_buf_t *json_buf);
static nxs_cfg_json_state_t pull_user_extract_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t pull_user_extract_user_memberships(nxs_process_t *     proc,
                                                               nxs_json_t *        json,
                                                               nxs_cfg_json_par_t *cfg_json_par_el,
                                                               nxs_array_t *       cfg_arr);
static nxs_cfg_json_state_t
        pull_user_extract_user_memberships_project(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

static void nxs_chat_srv_u_queue_cache_init_user(user_t *user);
static void nxs_chat_srv_u_queue_cache_free_user(user_t *user);

// clang-format off

/* Module initializations */

static nxs_string_t	_s_par_user		= nxs_string("user");
static nxs_string_t	_s_par_id		= nxs_string("id");
static nxs_string_t	_s_par_login		= nxs_string("login");
static nxs_string_t	_s_par_firstname	= nxs_string("firstname");
static nxs_string_t	_s_par_lastname		= nxs_string("lastname");
static nxs_string_t	_s_par_mail		= nxs_string("mail");
static nxs_string_t	_s_par_api_key		= nxs_string("api_key");
static nxs_string_t	_s_par_memberships	= nxs_string("memberships");
static nxs_string_t	_s_par_project		= nxs_string("project");
static nxs_string_t	_s_par_name		= nxs_string("name");


/* Module global functions */

// clang-format on

nxs_chat_srv_u_queue_cache_t *nxs_chat_srv_u_queue_cache_init(void)
{
	nxs_chat_srv_u_queue_cache_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_queue_cache_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_queue_cache_t));

	u_ctx->priorities.ttl = 0;

	nxs_list_init2(&u_ctx->users, user_t);

	nxs_array_init2(&u_ctx->priorities.els, issue_priorities_t);

	return u_ctx;
}

nxs_chat_srv_u_queue_cache_t *nxs_chat_srv_u_queue_cache_free(nxs_chat_srv_u_queue_cache_t *u_ctx)
{
	user_t *          u;
	issue_priority_t *p;
	size_t            i;

	if(u_ctx == NULL) {

		return NULL;
	}

	u_ctx->priorities.ttl = 0;

	for(i = 0; i < nxs_array_count(&u_ctx->priorities.els); i++) {

		p = nxs_array_get(&u_ctx->priorities.els, i);

		p->id         = 0;
		p->is_default = NXS_NO;

		nxs_string_free(&p->name);
	}

	nxs_array_free(&u_ctx->priorities.els);

	for(u = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, &u_ctx->users); u != NULL; u = nxs_list_del(&u_ctx->users, NXS_LIST_MOVE_NEXT)) {

		nxs_chat_srv_u_queue_cache_free_user(u);
	}

	nxs_list_free(&u_ctx->users);

	return (nxs_chat_srv_u_queue_cache_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_queue_cache_get_user_memberships(nxs_chat_srv_u_queue_cache_t *u_ctx,
                                                                   size_t                        user_id,
                                                                   nxs_array_t *                 memberships,
                                                                   size_t                        offset,
                                                                   size_t                        limit)
{
	user_t *                                 u;
	membership_t *                           m;
	nxs_chat_srv_err_t                       rc;
	nxs_chat_srv_m_queue_cache_membership_t *meta;
	size_t                                   i;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	for(u = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, &u_ctx->users); u != NULL; u = nxs_list_ptr_next(&u_ctx->users)) {

		if(u->id == user_id) {

			/* user found in cache, checking cache ttl */

			if(u->memberships_ttl < time(NULL)) {

				/* user memberships cache is out of date, need to update */

				nxs_chat_srv_u_queue_cache_free_user(u);
				nxs_chat_srv_u_queue_cache_init_user(u);

				pull_user(u, user_id);

				u->memberships_ttl = time(NULL) + DEFAULT_CACHE_TTL;

				if(offset >= (size_t)nxs_list_count(&u->memberships)) {

					return NXS_CHAT_SRV_E_EXIST;
				}

				for(i = 0, m = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, &u->memberships); m != NULL;
				    m = nxs_list_ptr_next(&u->memberships), i++) {

					if(i >= offset + limit) {

						break;
					}

					if(i >= offset) {

						meta = nxs_array_add(memberships);

						meta->project_id = m->id;

						nxs_string_init3(&meta->project_name, &m->name);
					}
				}

				return NXS_CHAT_SRV_E_OK;
			}
		}
	}

	/* if user not found in cache */

	u = nxs_list_add_tail(&u_ctx->users);

	nxs_chat_srv_u_queue_cache_init_user(u);

	pull_user(u, user_id);

	u->memberships_ttl = time(NULL) + DEFAULT_CACHE_TTL;

	if(offset >= (size_t)nxs_list_count(&u->memberships)) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	for(i = 0, m = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, &u->memberships); m != NULL; m = nxs_list_ptr_next(&u->memberships), i++) {

		if(i >= offset + limit) {

			break;
		}

		if(i >= offset) {

			meta = nxs_array_add(memberships);

			meta->project_id = m->id;

			nxs_string_init3(&meta->project_name, &m->name);
		}
	}

	return rc;
}

/* Module internal (static) functions */

static nxs_chat_srv_err_t pull_user(user_t *user, size_t user_id)
{
	nxs_buf_t          out_buf;
	nxs_chat_srv_err_t rc;

	if(user == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_buf_init(&out_buf, 1);

	nxs_chat_srv_d_rdmn_users_get(user_id, &nxs_chat_srv_cfg.rdmn.api_key, &out_buf, NULL, 0, 0);

	pull_user_extract(user, &out_buf);

	nxs_buf_free(&out_buf);

	return rc;
}

static nxs_chat_srv_err_t pull_user_extract(user_t *user, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_user,	user,	&pull_user_extract_user,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: tlgrm update rest api error: parse callback data error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t pull_user_extract_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	user_t *             user = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t       cfg_json;
	nxs_array_t          cfg_arr;
	nxs_cfg_json_state_t rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&user->id,		NULL,	NULL,					NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_login,		&user->login,		NULL,	NULL,					NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_firstname,	&user->firstname,	NULL,	NULL,					NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_lastname,	&user->lastname,	NULL,	NULL,					NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_mail,		&user->mail,		NULL,	NULL,					NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_api_key,	&user->api_key,		NULL,	NULL,					NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_memberships,	&user->memberships,	NULL,	&pull_user_extract_user_memberships,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(
		        &process, "[%s]: rdmn user get error: parse out_buf error (object: \"user\")", nxs_proc_get_name(&process));

		rc = NXS_CFG_JSON_CONF_ERROR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        pull_user_extract_user_memberships(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el, nxs_array_t *cfg_arr)
{
	nxs_list_t *  memberships = nxs_cfg_json_get_val(cfg_json_par_el);
	membership_t *m;

	m = nxs_list_add_tail(memberships);

	m->id     = 0;
	m->status = 0;

	nxs_string_init(&m->name);

	nxs_cfg_json_conf_array_skip_undef(cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_project,	m,	&pull_user_extract_user_memberships_project,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t
        pull_user_extract_user_memberships_project(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	membership_t *       m = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t       cfg_json;
	nxs_array_t          cfg_arr;
	nxs_cfg_json_state_t rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&m->id,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_name,		&m->name,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: rdmn user get error: parse out_buf error (object: \"user.memberships[].project\")",
		                    nxs_proc_get_name(&process));

		rc = NXS_CFG_JSON_CONF_ERROR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static void nxs_chat_srv_u_queue_cache_init_user(user_t *user)
{

	if(user == NULL) {

		return;
	}

	user->id               = 0;
	user->memberships_ttl  = 0;
	user->last_issue_ttl   = 0;
	user->watch_issues_ttl = 0;
	user->last_issue.id    = 0;

	nxs_string_init(&user->login);
	nxs_string_init(&user->firstname);
	nxs_string_init(&user->lastname);
	nxs_string_init(&user->mail);
	nxs_string_init(&user->api_key);
	nxs_string_init(&user->last_issue.subject);

	nxs_list_init2(&user->memberships, membership_t);
	nxs_list_init2(&user->watch_issues, issue_t);
}

static void nxs_chat_srv_u_queue_cache_free_user(user_t *user)
{
	membership_t *m;
	issue_t *     iss;

	if(user == NULL) {

		return;
	}

	user->id               = 0;
	user->memberships_ttl  = 0;
	user->last_issue_ttl   = 0;
	user->watch_issues_ttl = 0;
	user->last_issue.id    = 0;

	nxs_string_free(&user->login);
	nxs_string_free(&user->firstname);
	nxs_string_free(&user->lastname);
	nxs_string_free(&user->mail);
	nxs_string_free(&user->api_key);
	nxs_string_free(&user->last_issue.subject);

	for(m = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, &user->memberships); m != NULL;
	    m = nxs_list_del(&user->memberships, NXS_LIST_MOVE_NEXT)) {

		m->id     = 0;
		m->status = 0;

		nxs_string_free(&m->name);
	}

	for(iss = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, &user->watch_issues); iss != NULL;
	    iss = nxs_list_del(&user->watch_issues, NXS_LIST_MOVE_NEXT)) {

		iss->id = 0;

		nxs_string_free(&iss->subject);
	}

	nxs_list_free(&user->memberships);
	nxs_list_free(&user->watch_issues);
}
