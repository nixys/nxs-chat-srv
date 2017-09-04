#ifndef _INCLUDE_NXS_CHAT_SRV_U_QUEUE_CACHE_C
#define _INCLUDE_NXS_CHAT_SRV_U_QUEUE_CACHE_C
#endif

// clang-format off

#include <regex.h>

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
	nxs_string_t		name;
	int			status;
} project_t;

typedef struct
{
	nxs_string_t		regex_str;			/* regex string, specified for user */
	nxs_array_t		projects;			/* type: project_t. Selected projects (by specified regex) for user */
} project_regex_t;

typedef struct
{
	time_t			ttl;
	nxs_list_t		projects;			/* type: project_t. All available active projects for user*/
	project_regex_t		regex_ctx;
} project_ctx_t;

typedef struct
{
	size_t			rdmn_userid;

	project_ctx_t		project_ctx;			/* user projects context */
} user_t;

struct nxs_chat_srv_u_queue_cache_s
{
	nxs_list_t		users;				/* type: user_t */
};

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_err_t pull_user(user_t *user, size_t rdmn_userid);
static nxs_chat_srv_err_t pull_user_extract(user_t *user, nxs_buf_t *json_buf);
static nxs_cfg_json_state_t pull_user_extract_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t pull_user_extract_user_memberships(nxs_process_t *     proc,
                                                               nxs_json_t *        json,
                                                               nxs_cfg_json_par_t *cfg_json_par_el,
                                                               nxs_array_t *       cfg_arr);
static nxs_cfg_json_state_t
        pull_user_extract_user_memberships_project(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

static void user_init(user_t *user);
static void user_free(user_t *user);
static void user_del(nxs_list_t *users, user_t *user);
static user_t *user_find_by_id(nxs_list_t *users, size_t rdmn_userid);
static user_t *user_touch(nxs_list_t *users, size_t rdmn_userid, nxs_string_t *regex_str);

static void project_ctx_init(project_ctx_t *p_ctx);
static void project_ctx_free(project_ctx_t *p_ctx);
static void project_regex_init(project_regex_t *r_ctx);
static void project_regex_free(project_regex_t *r_ctx);
static nxs_chat_srv_err_t project_regex_set(project_regex_t *r_ctx, nxs_string_t *regex_str, nxs_list_t *projects);

// clang-format off

/* Module initializations */

static nxs_string_t	_s_par_user		= nxs_string("user");
static nxs_string_t	_s_par_id		= nxs_string("id");
static nxs_string_t	_s_par_memberships	= nxs_string("memberships");
static nxs_string_t	_s_par_project		= nxs_string("project");
static nxs_string_t	_s_par_name		= nxs_string("name");

/* Module global functions */

// clang-format on

nxs_chat_srv_u_queue_cache_t *nxs_chat_srv_u_queue_cache_init(void)
{
	nxs_chat_srv_u_queue_cache_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_queue_cache_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_queue_cache_t));

	nxs_list_init2(&u_ctx->users, user_t);

	return u_ctx;
}

nxs_chat_srv_u_queue_cache_t *nxs_chat_srv_u_queue_cache_free(nxs_chat_srv_u_queue_cache_t *u_ctx)
{
	user_t *u;

	if(u_ctx == NULL) {

		return NULL;
	}

	for(u = nxs_list_ptr_init(&u_ctx->users, NXS_LIST_PTR_INIT_HEAD); u != NULL; u = nxs_list_del(&u_ctx->users, NXS_LIST_MOVE_NEXT)) {

		user_free(u);
	}

	nxs_list_free(&u_ctx->users);

	return (nxs_chat_srv_u_queue_cache_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_queue_cache_get_user_projects(nxs_chat_srv_u_queue_cache_t *u_ctx,
                                                                size_t                        user_id,
                                                                nxs_array_t *                 projects,
                                                                nxs_string_t *                project_name_regex,
                                                                size_t                        offset,
                                                                size_t                        limit,
                                                                size_t *                      count)
{
	user_t *                              u;
	project_t *                           p;
	nxs_chat_srv_m_queue_cache_project_t *project;
	nxs_chat_srv_err_t                    rc;
	size_t                                i, c;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((u = user_touch(&u_ctx->users, user_id, project_name_regex)) == NULL) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	c = nxs_array_count(&u->project_ctx.regex_ctx.projects);

	if(offset >= c) {

		nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);
	}

	if(count != NULL) {

		*count = c;
	}

	for(i = offset; i < c; i++) {

		if(i >= offset + limit) {

			break;
		}

		p = nxs_array_get(&u->project_ctx.regex_ctx.projects, i);

		project = nxs_array_add(projects);

		project->id   = p->id;
		project->name = &p->name;
	}

error:

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_queue_cache_get_user_project_first(nxs_chat_srv_u_queue_cache_t *u_ctx,
                                                                     size_t                        user_id,
                                                                     size_t *                      project_id,
                                                                     nxs_string_t *                project_name)
{
	user_t *           u;
	project_t *        p;
	nxs_chat_srv_err_t rc;

	if(u_ctx == NULL || project_id == NULL || project_name == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if((u = user_touch(&u_ctx->users, user_id, NULL)) == NULL) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if((p = nxs_array_get(&u->project_ctx.regex_ctx.projects, 0)) == NULL) {

		nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);
	}

	*project_id = p->id;

	nxs_string_clone(project_name, &p->name);

error:

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_queue_cache_get_user_project_by_id(nxs_chat_srv_u_queue_cache_t *u_ctx,
                                                                     size_t                        user_id,
                                                                     size_t                        project_id,
                                                                     nxs_string_t *                project_name)
{
	user_t *           u;
	project_t *        p;
	nxs_chat_srv_err_t rc;

	if(u_ctx == NULL || project_name == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_EXIST;

	if((u = user_touch(&u_ctx->users, user_id, NULL)) == NULL) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	for(p = nxs_list_ptr_init(&u->project_ctx.projects, NXS_LIST_PTR_INIT_HEAD); p != NULL;
	    p = nxs_list_ptr_next(&u->project_ctx.projects)) {

		if(p->id == project_id) {

			nxs_string_clone(project_name, &p->name);

			nxs_error(rc, NXS_CHAT_SRV_E_OK, error);
		}
	}

error:

	return rc;
}

/* Module internal (static) functions */

static nxs_chat_srv_err_t pull_user(user_t *user, size_t rdmn_userid)
{
	nxs_buf_t          out_buf;
	nxs_chat_srv_err_t rc;

	if(user == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_buf_init(&out_buf, 1);

	if(nxs_chat_srv_d_rdmn_users_get(rdmn_userid, &nxs_chat_srv_cfg.rdmn.api_key, &out_buf, NULL, 0, 0) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(pull_user_extract(user, &out_buf)) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

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

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&user->rdmn_userid,	NULL,	NULL,					NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_memberships,	&user->project_ctx,	NULL,	&pull_user_extract_user_memberships,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,	0,	0,	NXS_YES,	NULL);

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
	project_ctx_t *p_ctx = nxs_cfg_json_get_val(cfg_json_par_el);
	project_t *    p;

	p = nxs_list_add_tail(&p_ctx->projects);

	p->id     = 0;
	p->status = 0;

	nxs_string_init(&p->name);

	nxs_cfg_json_conf_array_skip_undef(cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_project,	p,	&pull_user_extract_user_memberships_project,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t
        pull_user_extract_user_memberships_project(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	project_t *          p = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t       cfg_json;
	nxs_array_t          cfg_arr;
	nxs_cfg_json_state_t rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&p->id,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_name,		&p->name,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

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

static void user_init(user_t *user)
{

	if(user == NULL) {

		return;
	}

	user->rdmn_userid = 0;

	project_ctx_init(&user->project_ctx);
}

static void user_free(user_t *user)
{

	if(user == NULL) {

		return;
	}

	user->rdmn_userid = 0;

	project_ctx_free(&user->project_ctx);
}

static void user_del(nxs_list_t *users, user_t *user)
{
	user_t *u;

	if(users == NULL) {

		return;
	}

	for(u = nxs_list_ptr_init(users, NXS_LIST_PTR_INIT_HEAD); u != NULL; u = nxs_list_ptr_next(users)) {

		if(u == user) {

			user_free(u);

			nxs_list_del(users, NXS_LIST_MOVE_NEXT);

			return;
		}
	}

	return;
}

static user_t *user_find_by_id(nxs_list_t *users, size_t rdmn_userid)
{
	user_t *u;

	if(users == NULL) {

		return NULL;
	}

	for(u = nxs_list_ptr_init(users, NXS_LIST_PTR_INIT_HEAD); u != NULL; u = nxs_list_ptr_next(users)) {

		if(u->rdmn_userid == rdmn_userid) {

			return u;
		}
	}

	return NULL;
}

/*
 * Check user existence and update cache if it needed. If user not exist in cache it will be added.
 */
static user_t *user_touch(nxs_list_t *users, size_t rdmn_userid, nxs_string_t *regex_str)
{
	user_t *     u;
	nxs_string_t r_str;

	if(users == NULL) {

		return NULL;
	}

	nxs_string_init_empty(&r_str);

	if((u = user_find_by_id(users, rdmn_userid)) == NULL) {

		/* if user not found in cache */

		u = nxs_list_add_tail(users);

		user_init(u);

		if(pull_user(u, rdmn_userid) != NXS_CHAT_SRV_E_OK) {

			user_del(users, u);

			nxs_error(u, NULL, error);
		}

		u->project_ctx.ttl = time(NULL) + DEFAULT_CACHE_TTL;

		if(regex_str != NULL) {

			project_regex_set(&u->project_ctx.regex_ctx, regex_str, &u->project_ctx.projects);
		}
		else {

			project_regex_set(&u->project_ctx.regex_ctx, &r_str, &u->project_ctx.projects);
		}
	}
	else {

		/* user found in cache, checking cache ttl */

		if(u->project_ctx.ttl < time(NULL)) {

			/* user projects cache is out of date, need to update */

			nxs_string_clone(&r_str, &u->project_ctx.regex_ctx.regex_str);

			user_free(u);
			user_init(u);

			if(pull_user(u, rdmn_userid) != NXS_CHAT_SRV_E_OK) {

				user_del(users, u);

				nxs_error(u, NULL, error);
			}

			u->project_ctx.ttl = time(NULL) + DEFAULT_CACHE_TTL;

			if(regex_str != NULL) {

				project_regex_set(&u->project_ctx.regex_ctx, regex_str, &u->project_ctx.projects);
			}
			else {

				project_regex_set(&u->project_ctx.regex_ctx, &r_str, &u->project_ctx.projects);
			}
		}
		else {

			if(regex_str != NULL && nxs_string_cmp(regex_str, 0, &u->project_ctx.regex_ctx.regex_str, 0) == NXS_NO) {

				project_regex_set(&u->project_ctx.regex_ctx, regex_str, &u->project_ctx.projects);
			}
		}
	}

error:

	nxs_string_free(&r_str);

	return u;
}

static void project_ctx_init(project_ctx_t *p_ctx)
{

	p_ctx->ttl = 0;

	nxs_list_init2(&p_ctx->projects, project_t);

	project_regex_init(&p_ctx->regex_ctx);
}

static void project_ctx_free(project_ctx_t *p_ctx)
{
	project_t *p;

	p_ctx->ttl = 0;

	for(p = nxs_list_ptr_init(&p_ctx->projects, NXS_LIST_PTR_INIT_HEAD); p != NULL;
	    p = nxs_list_del(&p_ctx->projects, NXS_LIST_MOVE_NEXT)) {

		p->id     = 0;
		p->status = 0;

		nxs_string_free(&p->name);
	}

	nxs_list_free(&p_ctx->projects);

	project_regex_free(&p_ctx->regex_ctx);
}

static void project_regex_init(project_regex_t *r_ctx)
{

	if(r_ctx == NULL) {

		return;
	}

	nxs_string_init_empty(&r_ctx->regex_str);

	nxs_array_init2(&r_ctx->projects, project_t);
}

static void project_regex_free(project_regex_t *r_ctx)
{
	size_t     i;
	project_t *p;

	if(r_ctx == NULL) {

		return;
	}

	nxs_string_free(&r_ctx->regex_str);

	for(i = 0; i < nxs_array_count(&r_ctx->projects); i++) {

		p = nxs_array_get(&r_ctx->projects, i);

		p->id     = 0;
		p->status = 0;

		nxs_string_free(&p->name);
	}

	nxs_array_free(&r_ctx->projects);
}

static nxs_chat_srv_err_t project_regex_set(project_regex_t *r_ctx, nxs_string_t *regex_str, nxs_list_t *projects)
{
	nxs_chat_srv_err_t rc;
	project_t *        p1, *p2;
	regex_t            regex;
	size_t             i;
	int                ec;
	char               msgbuf[100];

	if(r_ctx == NULL || regex_str == NULL || projects == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	/* clean up regex context before filling it with new data */

	nxs_string_clone(&r_ctx->regex_str, regex_str);

	for(i = 0; i < nxs_array_count(&r_ctx->projects); i++) {

		p1 = nxs_array_get(&r_ctx->projects, i);

		p1->id     = 0;
		p1->status = 0;

		nxs_string_free(&p1->name);
	}

	nxs_array_clear(&r_ctx->projects);

	if(nxs_string_len(&r_ctx->regex_str) == 0) {

		/* when no regex */

		for(p1 = nxs_list_ptr_init(projects, NXS_LIST_PTR_INIT_HEAD); p1 != NULL; p1 = nxs_list_ptr_next(projects)) {

			p2 = nxs_array_add(&r_ctx->projects);

			p2->id = p1->id;

			nxs_string_init3(&p2->name, &p1->name);
		}
	}
	else {

		/* when using regex */

		if(regcomp(&regex, (char *)nxs_string_str(&r_ctx->regex_str), REG_ICASE) != 0) {

			nxs_log_write_error(&process,
			                    "set user regex ctx error: could not compile regex (regex string: \"%s\")",
			                    nxs_string_str(&r_ctx->regex_str));

			return NXS_CHAT_SRV_E_ERR;
		}

		for(p1 = nxs_list_ptr_init(projects, NXS_LIST_PTR_INIT_HEAD); p1 != NULL; p1 = nxs_list_ptr_next(projects)) {

			if((ec = regexec(&regex, (char *)nxs_string_str(&p1->name), 0, NULL, 0)) == 0) {

				/* regex match */

				p2 = nxs_array_add(&r_ctx->projects);

				p2->id = p1->id;

				nxs_string_init3(&p2->name, &p1->name);
			}
			else {

				if(ec != REG_NOMATCH) {

					regerror(ec, &regex, msgbuf, sizeof(msgbuf));

					nxs_log_write_error(
					        &process,
					        "get user projects from cache error: regex failed (regex string: \"%s\", error: %s)",
					        nxs_string_str(&r_ctx->regex_str),
					        msgbuf);

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}
			}
		}

		regfree(&regex);
	}

error:

	return rc;
}
