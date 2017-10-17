#ifndef _INCLUDE_NXS_CHAT_SRV_U_DB_SESS_C
#define _INCLUDE_NXS_CHAT_SRV_U_DB_SESS_C
#endif

// clang-format off

#include <regex.h>

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/db-sess/db-sess-typedefs.h>
#include <units/db-sess/db-sess.h>
#include <units/db-sess/db-sess-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

/*
 * Using in sessions to fill all needed data to create new remine issue
 */

typedef struct
{
	size_t						id;
	nxs_string_t					name;
} nxs_chat_srv_u_db_sess_project_t;

typedef struct
{
	size_t						project_id;		/* redmine project id */
	nxs_string_t					project_name;		/* redmine project name */
	size_t						priority_id;		/* redmine priority id */
	nxs_string_t					priority_name;		/* redmine priority name */
	nxs_string_t					subject;		/* redmine issue subject */
	nxs_string_t					description;		/* redmine issue description */
	nxs_bool_t					is_private;		/* if new issue is private */
	nxs_bool_t					is_ext;			/* if new issue need extended actions */
	nxs_string_t					project_name_regex;	/* regex to filter redmine projects name */
	nxs_list_t					available_projects;	/* type: nxs_chat_srv_u_db_sess_project_t. All available active projects for user */
	nxs_array_t					selected_projects;	/* type: nxs_chat_srv_u_db_sess_project_t. Selected projects (by specified regex) for user */
	nxs_array_t					file_ids;		/* type: nxs_string_t. Telegram 'file_id' array */
} nxs_chat_srv_u_db_sess_t_new_issue_t;

/*
 * Using in session to send message to specified redmine issue
 */
typedef struct
{
	nxs_string_t					message;		/* message to append into redmine issue */
	nxs_bool_t					is_private;		/* if message is private */
	nxs_bool_t					is_ext;			/* if message need extended actions */
	nxs_array_t					file_ids;		/* type: nxs_string_t. Telegram 'file_id' array */
} nxs_chat_srv_u_db_sess_t_message_t;

typedef struct
{
	nxs_string_t					rdmn_api_key;		/* user api key in Redmine */
	size_t						chat_id;		/* chat id where session was started */
	size_t						usr_message_id;		/* user message id where session was started */
	size_t						bot_message_id;		/* bot message id where session was started */
	time_t						updated_at;		/* session last update */
	nxs_chat_srv_m_db_sess_type_t			type;			/* type of data stored in `data` field */
	nxs_chat_srv_m_db_sess_wait_for_type_t		wait_for;		/* used to help bot to determine type of expected data received from user (e.g. expected reply from user with new issue subject) */

	nxs_chat_srv_u_db_sess_t_message_t		message;
	nxs_chat_srv_u_db_sess_t_new_issue_t		new_issue;
} nxs_chat_srv_u_db_sess_t_value_t;

struct nxs_chat_srv_u_db_sess_s
{
	nxs_chat_srv_d_db_sess_t			*sess_db_ctx;

	size_t						tlgrm_userid;		/* session owner (it is not bot) */
	nxs_chat_srv_u_db_sess_t_value_t		value;
};

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_err_t
        nxs_chat_srv_u_db_sess_pull_user(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue, size_t rdmn_userid, nxs_array_t *cache_projects);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_pull_user_extract(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue, nxs_buf_t *json_buf);
static nxs_cfg_json_state_t
        nxs_chat_srv_u_db_sess_pull_user_extract_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_chat_srv_u_db_sess_pull_user_extract_user_memberships(nxs_process_t *     proc,
                                                                                      nxs_json_t *        json,
                                                                                      nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                      nxs_array_t *       cfg_arr);
static nxs_cfg_json_state_t nxs_chat_srv_u_db_sess_pull_user_extract_user_memberships_project(nxs_process_t *     proc,
                                                                                              nxs_json_t *        json,
                                                                                              nxs_cfg_json_par_t *cfg_json_par_el);

static void nxs_chat_srv_u_db_sess_s_value_init(nxs_chat_srv_u_db_sess_t_value_t *value);
static void nxs_chat_srv_u_db_sess_s_value_free(nxs_chat_srv_u_db_sess_t_value_t *value);
static void nxs_chat_srv_u_db_sess_s_value_clear(nxs_chat_srv_u_db_sess_t_value_t *value);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_value_get(nxs_chat_srv_u_db_sess_t *u_ctx);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_value_put(nxs_chat_srv_u_db_sess_t *u_ctx);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_value_serialize(nxs_string_t *value_str, nxs_chat_srv_u_db_sess_t_value_t *value);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_value_deserialize(nxs_string_t *value_str, nxs_chat_srv_u_db_sess_t_value_t *value);

static void nxs_chat_srv_u_db_sess_s_message_init(nxs_chat_srv_u_db_sess_t_message_t *message);
static void nxs_chat_srv_u_db_sess_s_message_free(nxs_chat_srv_u_db_sess_t_message_t *message);
static void nxs_chat_srv_u_db_sess_s_message_clear(nxs_chat_srv_u_db_sess_t_message_t *message);
static void nxs_chat_srv_u_db_sess_s_message_serialize(nxs_chat_srv_u_db_sess_t_message_t *message, nxs_string_t *out_str);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_message_deserialize(nxs_chat_srv_u_db_sess_t_message_t *message, nxs_json_t *json_data);

static void nxs_chat_srv_u_db_sess_s_new_issue_init(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue);
static void nxs_chat_srv_u_db_sess_s_new_issue_free(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue);
static void nxs_chat_srv_u_db_sess_s_new_issue_clear(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue);
static void nxs_chat_srv_u_db_sess_s_new_issue_set_selected_project_first(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_new_issue_set_project_by_id(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue,
                                                                               size_t                                project_id);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_new_issue_regex_set(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue,
                                                                       nxs_string_t *                        regex_str);
static void nxs_chat_srv_u_db_sess_s_new_issue_serialize(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue, nxs_string_t *out_str);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_new_issue_deserialize(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue,
                                                                         nxs_json_t *                          json_data);
static nxs_cfg_json_state_t nxs_chat_srv_u_db_sess_s_new_issue_deserialize_projects_available(nxs_process_t *     proc,
                                                                                              nxs_json_t *        json,
                                                                                              nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                              nxs_array_t *       cfg_arr);
static nxs_cfg_json_state_t nxs_chat_srv_u_db_sess_s_new_issue_deserialize_projects_selected(nxs_process_t *     proc,
                                                                                             nxs_json_t *        json,
                                                                                             nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                             nxs_array_t *       cfg_arr);
static nxs_cfg_json_state_t nxs_chat_srv_u_db_sess_s_new_issue_deserialize_projects_name(nxs_process_t *     proc,
                                                                                         nxs_json_t *        json,
                                                                                         nxs_cfg_json_par_t *cfg_json_par_el);

// clang-format off

/* Module initializations */

static nxs_string_t _s_par_rdmn_api_key		= nxs_string("rdmn_api_key");
static nxs_string_t _s_par_chat_id		= nxs_string("chat_id");
static nxs_string_t _s_par_bot_message_id	= nxs_string("bot_message_id");
static nxs_string_t _s_par_usr_message_id	= nxs_string("usr_message_id");
static nxs_string_t _s_par_updated_at		= nxs_string("updated_at");
static nxs_string_t _s_par_type			= nxs_string("type");
static nxs_string_t _s_par_wait_for		= nxs_string("wait_for");
static nxs_string_t _s_par_data			= nxs_string("data");
static nxs_string_t _s_par_message		= nxs_string("message");
static nxs_string_t _s_par_project_id		= nxs_string("project_id");
static nxs_string_t _s_par_project_name		= nxs_string("project_name");
static nxs_string_t _s_par_priority_id		= nxs_string("priority_id");
static nxs_string_t _s_par_priority_name	= nxs_string("priority_name");
static nxs_string_t _s_par_subject		= nxs_string("subject");
static nxs_string_t _s_par_description		= nxs_string("description");
static nxs_string_t _s_par_project_name_regex	= nxs_string("project_name_regex");
static nxs_string_t _s_par_available_projects	= nxs_string("available_projects");
static nxs_string_t _s_par_selected_projects	= nxs_string("selected_projects");
static nxs_string_t _s_par_id			= nxs_string("id");
static nxs_string_t _s_par_name			= nxs_string("name");
static nxs_string_t _s_par_user			= nxs_string("user");
static nxs_string_t _s_par_memberships		= nxs_string("memberships");
static nxs_string_t _s_par_project		= nxs_string("project");
static nxs_string_t _s_par_is_private		= nxs_string("is_private");
static nxs_string_t _s_par_is_ext		= nxs_string("is_ext");
static nxs_string_t _s_par_file_ids		= nxs_string("file_ids");

/* Module global functions */

// clang-format on

nxs_chat_srv_u_db_sess_t *nxs_chat_srv_u_db_sess_init(void)
{
	nxs_chat_srv_u_db_sess_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_db_sess_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_db_sess_t));

	u_ctx->tlgrm_userid = 0;

	u_ctx->sess_db_ctx = NULL;

	nxs_chat_srv_u_db_sess_s_value_init(&u_ctx->value);

	return u_ctx;
}

nxs_chat_srv_u_db_sess_t *nxs_chat_srv_u_db_sess_free(nxs_chat_srv_u_db_sess_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	u_ctx->sess_db_ctx = nxs_chat_srv_d_db_sess_free(u_ctx->sess_db_ctx);

	u_ctx->tlgrm_userid = 0;

	nxs_chat_srv_u_db_sess_s_value_free(&u_ctx->value);

	return (nxs_chat_srv_u_db_sess_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_pull(nxs_chat_srv_u_db_sess_t *u_ctx, size_t tlgrm_userid)
{
	nxs_chat_srv_err_t rc;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	u_ctx->tlgrm_userid = tlgrm_userid;

	if((u_ctx->sess_db_ctx = nxs_chat_srv_d_db_sess_init()) == NULL) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_db_sess_s_value_get(u_ctx) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_start(nxs_chat_srv_u_db_sess_t *             u_ctx,
                                                nxs_string_t *                         rdmn_api_key,
                                                size_t                                 chat_id,
                                                size_t                                 usr_message_id,
                                                size_t                                 bot_message_id,
                                                nxs_chat_srv_m_db_sess_type_t          type,
                                                nxs_chat_srv_m_db_sess_wait_for_type_t wait_for)
{

	if(u_ctx == NULL || rdmn_api_key == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	u_ctx->value.chat_id        = chat_id;
	u_ctx->value.usr_message_id = usr_message_id;
	u_ctx->value.bot_message_id = bot_message_id;
	u_ctx->value.type           = type;
	u_ctx->value.wait_for       = wait_for;

	nxs_string_clone(&u_ctx->value.rdmn_api_key, rdmn_api_key);

	return nxs_chat_srv_u_db_sess_s_value_put(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_destroy(nxs_chat_srv_u_db_sess_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	nxs_chat_srv_u_db_sess_s_value_clear(&u_ctx->value);

	return nxs_chat_srv_d_db_sess_del(u_ctx->sess_db_ctx, u_ctx->tlgrm_userid);
}

nxs_bool_t nxs_chat_srv_u_db_sess_check_exist(nxs_chat_srv_u_db_sess_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NXS_NO;
	}

	if(u_ctx->tlgrm_userid == 0 || u_ctx->value.type == NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE) {

		return NXS_NO;
	}

	return NXS_YES;
}

size_t nxs_chat_srv_u_db_sess_get_tlgrm_userid(nxs_chat_srv_u_db_sess_t *u_ctx)
{

	if(u_ctx == NULL) {

		return 0;
	}

	return u_ctx->tlgrm_userid;
}

nxs_string_t *nxs_chat_srv_u_db_sess_get_rdmn_api_key(nxs_chat_srv_u_db_sess_t *u_ctx)
{

	if(u_ctx == NULL) {

		return 0;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return 0;
	}

	return &u_ctx->value.rdmn_api_key;
}

size_t nxs_chat_srv_u_db_sess_get_chat_id(nxs_chat_srv_u_db_sess_t *u_ctx)
{

	if(u_ctx == NULL) {

		return 0;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return 0;
	}

	return u_ctx->value.chat_id;
}

size_t nxs_chat_srv_u_db_sess_get_usr_message_id(nxs_chat_srv_u_db_sess_t *u_ctx)
{

	if(u_ctx == NULL) {

		return 0;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return 0;
	}

	return u_ctx->value.usr_message_id;
}

size_t nxs_chat_srv_u_db_sess_get_bot_message_id(nxs_chat_srv_u_db_sess_t *u_ctx)
{

	if(u_ctx == NULL) {

		return 0;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return 0;
	}

	return u_ctx->value.bot_message_id;
}

nxs_chat_srv_m_db_sess_type_t nxs_chat_srv_u_db_sess_get_type(nxs_chat_srv_u_db_sess_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE;
	}

	return u_ctx->value.type;
}

nxs_chat_srv_m_db_sess_wait_for_type_t nxs_chat_srv_u_db_sess_get_wait_for(nxs_chat_srv_u_db_sess_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;
	}

	return u_ctx->value.wait_for;
}

nxs_array_t *nxs_chat_srv_u_db_sess_get_files(nxs_chat_srv_u_db_sess_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NULL;
	}

	switch(u_ctx->value.type) {

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE:

			return &u_ctx->value.message.file_ids;

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE:

			return &u_ctx->value.new_issue.file_ids;

		default:

			nxs_log_write_warn(
			        &process,
			        "[%s]: can't get session file_ids: session type incorrect (tlgrm user id: %zu, session type: %d)",
			        nxs_proc_get_name(&process),
			        u_ctx->tlgrm_userid,
			        u_ctx->value.type);

			break;
	}

	return NULL;
}

size_t nxs_chat_srv_u_db_sess_get_files_count(nxs_chat_srv_u_db_sess_t *u_ctx)
{
	if(u_ctx == NULL) {

		return 0;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return 0;
	}

	switch(u_ctx->value.type) {

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE:

			return nxs_array_count(&u_ctx->value.message.file_ids);

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE:

			return nxs_array_count(&u_ctx->value.new_issue.file_ids);

		default:

			nxs_log_write_warn(
			        &process,
			        "[%s]: can't get session files count: session type incorrect (tlgrm user id: %zu, session type: %d)",
			        nxs_proc_get_name(&process),
			        u_ctx->tlgrm_userid,
			        u_ctx->value.type);

			break;
	}

	return 0;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_set_wait_for(nxs_chat_srv_u_db_sess_t *u_ctx, nxs_chat_srv_m_db_sess_wait_for_type_t wait_for)
{
	nxs_chat_srv_err_t rc;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	rc = NXS_CHAT_SRV_E_OK;

	switch(u_ctx->value.type) {

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE:

			/* check available 'wait-for' types for this session type */
			if(wait_for != NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_SUBJECT &&
			   wait_for != NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_DESCRIPTION &&
			   wait_for != NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_PROJECTNAME_REGEX &&
			   wait_for != NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE) {

				nxs_error(rc, NXS_CHAT_SRV_E_TYPE, error);
			}

			u_ctx->value.wait_for = wait_for;

			if(nxs_chat_srv_u_db_sess_s_value_put(u_ctx) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_TYPE, error);
			}

			break;

		default:

			nxs_error(rc, NXS_CHAT_SRV_E_TYPE, error);
	}

error:

	return rc;
}

nxs_chat_srv_err_t
        nxs_chat_srv_u_db_sess_set_ids(nxs_chat_srv_u_db_sess_t *u_ctx, size_t chat_id, size_t usr_message_id, size_t bot_message_id)
{

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(chat_id > 0) {

		u_ctx->value.chat_id = chat_id;
	}

	if(usr_message_id > 0) {

		u_ctx->value.usr_message_id = usr_message_id;
	}

	if(bot_message_id > 0) {

		u_ctx->value.bot_message_id = bot_message_id;
	}

	return nxs_chat_srv_u_db_sess_s_value_put(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_add_file(nxs_chat_srv_u_db_sess_t *u_ctx, nxs_string_t *file_id)
{
	nxs_string_t *s;

	if(u_ctx == NULL || file_id == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	switch(u_ctx->value.type) {

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE:

			s = nxs_array_add(&u_ctx->value.message.file_ids);

			nxs_string_init3(s, file_id);

			return nxs_chat_srv_u_db_sess_s_value_put(u_ctx);

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE:

			s = nxs_array_add(&u_ctx->value.new_issue.file_ids);

			nxs_string_init3(s, file_id);

			return nxs_chat_srv_u_db_sess_s_value_put(u_ctx);

		default:

			nxs_log_write_warn(&process,
			                   "[%s]: can't add session file_id: session type incorrect (tlgrm user id: %zu, session type: %d)",
			                   nxs_proc_get_name(&process),
			                   u_ctx->tlgrm_userid,
			                   u_ctx->value.type);

			break;
	}

	return NXS_CHAT_SRV_E_TYPE;
}

nxs_string_t *nxs_chat_srv_u_db_sess_t_get_message(nxs_chat_srv_u_db_sess_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NULL;
	}

	if(u_ctx->value.type != NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE) {

		nxs_log_write_warn(
		        &process,
		        "[%s]: can't get session message: session type incorrect (tlgrm user id: %zu, session type: %d, expected type: %d)",
		        nxs_proc_get_name(&process),
		        u_ctx->tlgrm_userid,
		        u_ctx->value.type,
		        NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE);

		return NULL;
	}

	return &u_ctx->value.message.message;
}

nxs_bool_t nxs_chat_srv_u_db_sess_t_get_message_is_private(nxs_chat_srv_u_db_sess_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NXS_NO;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NXS_NO;
	}

	if(u_ctx->value.type != NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE) {

		nxs_log_write_warn(&process,
		                   "[%s]: can't get session message is private: session type incorrect (tlgrm user id: %zu, session type: "
		                   "%d, expected type: %d)",
		                   nxs_proc_get_name(&process),
		                   u_ctx->tlgrm_userid,
		                   u_ctx->value.type,
		                   NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE);

		return NXS_NO;
	}

	return u_ctx->value.message.is_private;
}

nxs_bool_t nxs_chat_srv_u_db_sess_t_get_message_is_ext(nxs_chat_srv_u_db_sess_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NXS_NO;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NXS_NO;
	}

	if(u_ctx->value.type != NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE) {

		nxs_log_write_warn(&process,
		                   "[%s]: can't get session message is extended: session type incorrect (tlgrm user id: %zu, session type: "
		                   "%d, expected type: %d)",
		                   nxs_proc_get_name(&process),
		                   u_ctx->tlgrm_userid,
		                   u_ctx->value.type,
		                   NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE);

		return NXS_NO;
	}

	return u_ctx->value.message.is_ext;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_t_set_message(nxs_chat_srv_u_db_sess_t *u_ctx, nxs_string_t *message)
{

	if(u_ctx == NULL || message == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(u_ctx->value.type != NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE) {

		nxs_log_write_warn(
		        &process,
		        "[%s]: can't set session message: session type incorrect (tlgrm user id: %zu, session type: %d, expected type: %d)",
		        nxs_proc_get_name(&process),
		        u_ctx->tlgrm_userid,
		        u_ctx->value.type,
		        NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE);

		return NXS_CHAT_SRV_E_TYPE;
	}

	u_ctx->value.message.is_private = nxs_chat_srv_c_notes_check_private(message, &u_ctx->value.message.message);
	u_ctx->value.message.is_ext     = nxs_chat_srv_c_notes_check_ext(&u_ctx->value.message.message, &u_ctx->value.message.message);

	return nxs_chat_srv_u_db_sess_s_value_put(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_t_cat_message(nxs_chat_srv_u_db_sess_t *u_ctx, nxs_string_t *message)
{

	if(u_ctx == NULL || message == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(u_ctx->value.type != NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE) {

		nxs_log_write_warn(
		        &process,
		        "[%s]: can't set session message: session type incorrect (tlgrm user id: %zu, session type: %d, expected type: %d)",
		        nxs_proc_get_name(&process),
		        u_ctx->tlgrm_userid,
		        u_ctx->value.type,
		        NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE);

		return NXS_CHAT_SRV_E_TYPE;
	}

	if(nxs_string_len(message) == 0) {

		return NXS_CHAT_SRV_E_OK;
	}

	if(nxs_string_len(&u_ctx->value.message.message) == 0) {

		u_ctx->value.message.is_private = nxs_chat_srv_c_notes_check_private(message, &u_ctx->value.message.message);
		u_ctx->value.message.is_ext = nxs_chat_srv_c_notes_check_ext(&u_ctx->value.message.message, &u_ctx->value.message.message);
	}
	else {

		nxs_string_printf2_cat(&u_ctx->value.message.message, "\n%r", message);
	}

	return nxs_chat_srv_u_db_sess_s_value_put(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_t_get_new_issue(nxs_chat_srv_u_db_sess_t *u_ctx,
                                                          size_t *                  project_id,
                                                          nxs_string_t *            project_name,
                                                          size_t *                  priority_id,
                                                          nxs_string_t *            priority_name,
                                                          nxs_string_t *            subject,
                                                          nxs_string_t *            description,
                                                          nxs_bool_t *              is_private,
                                                          nxs_bool_t *              is_ext,
                                                          nxs_string_t *            project_name_regex)
{

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(u_ctx->value.type != NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE) {

		nxs_log_write_warn(&process,
		                   "[%s]: can't get session new issue: session type incorrect (tlgrm user id: %zu, session type: %d, "
		                   "expected type: %d)",
		                   nxs_proc_get_name(&process),
		                   u_ctx->tlgrm_userid,
		                   u_ctx->value.type,
		                   NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE);

		return NXS_CHAT_SRV_E_TYPE;
	}

	if(project_id != NULL) {

		*project_id = u_ctx->value.new_issue.project_id;
	}

	if(priority_id != NULL) {

		*priority_id = u_ctx->value.new_issue.priority_id;
	}

	if(project_name != NULL) {

		nxs_string_clone(project_name, &u_ctx->value.new_issue.project_name);
	}

	if(priority_name != NULL) {

		nxs_string_clone(priority_name, &u_ctx->value.new_issue.priority_name);
	}

	if(subject != NULL) {

		nxs_string_clone(subject, &u_ctx->value.new_issue.subject);
	}

	if(description != NULL) {

		nxs_string_clone(description, &u_ctx->value.new_issue.description);
	}

	if(is_private != NULL) {

		*is_private = u_ctx->value.new_issue.is_private;
	}

	if(is_ext != NULL) {

		*is_ext = u_ctx->value.new_issue.is_ext;
	}

	if(project_name_regex != NULL) {

		nxs_string_clone(project_name_regex, &u_ctx->value.new_issue.project_name_regex);
	}

	return NXS_CHAT_SRV_E_OK;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_t_get_new_issue_projects(nxs_chat_srv_u_db_sess_t *u_ctx,
                                                                   nxs_array_t *             projects,
                                                                   size_t                    offset,
                                                                   size_t                    limit,
                                                                   size_t *                  count)
{
	nxs_chat_srv_u_db_sess_project_t *p;
	nxs_chat_srv_m_db_sess_project_t *project;
	size_t                            i, c;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(u_ctx->value.type != NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE) {

		nxs_log_write_warn(&process,
		                   "[%s]: can't get projects session new issue: session type incorrect (tlgrm user id: %zu, session type: "
		                   "%d, expected type: %d)",
		                   nxs_proc_get_name(&process),
		                   u_ctx->tlgrm_userid,
		                   u_ctx->value.type,
		                   NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE);

		return NXS_CHAT_SRV_E_TYPE;
	}

	c = nxs_array_count(&u_ctx->value.new_issue.selected_projects);

	if(count != NULL) {

		*count = c;
	}

	if(offset < c) {

		for(i = offset; i < c; i++) {

			if(i >= offset + limit) {

				break;
			}

			p = nxs_array_get(&u_ctx->value.new_issue.selected_projects, i);

			project = nxs_array_add(projects);

			project->id   = p->id;
			project->name = &p->name;
		}
	}

	return NXS_CHAT_SRV_E_OK;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_t_set_new_issue(nxs_chat_srv_u_db_sess_t *u_ctx,
                                                          size_t                    project_id,
                                                          size_t                    priority_id,
                                                          nxs_string_t *            priority_name,
                                                          nxs_string_t *            subject,
                                                          nxs_string_t *            description,
                                                          nxs_string_t *            project_name_regex)
{
	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(u_ctx->value.type != NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE) {

		nxs_log_write_warn(&process,
		                   "[%s]: can't set session new issue: session type incorrect (tlgrm user id: %zu, session type: %d, "
		                   "expected type: %d)",
		                   nxs_proc_get_name(&process),
		                   u_ctx->tlgrm_userid,
		                   u_ctx->value.type,
		                   NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE);

		return NXS_CHAT_SRV_E_TYPE;
	}

	if(project_id > 0) {

		if(nxs_chat_srv_u_db_sess_s_new_issue_set_project_by_id(&u_ctx->value.new_issue, project_id) != NXS_CHAT_SRV_E_OK) {

			nxs_log_write_warn(
			        &process,
			        "[%s]: can't set session new issue: can't find project by id (tlgrm user id: %zu, project id: %zu)",
			        nxs_proc_get_name(&process),
			        u_ctx->tlgrm_userid,
			        project_id);

			return NXS_CHAT_SRV_E_EXIST;
		}
	}

	if(priority_id > 0) {

		u_ctx->value.new_issue.priority_id = priority_id;
	}

	if(priority_name != NULL) {

		nxs_string_clone(&u_ctx->value.new_issue.priority_name, priority_name);
	}

	if(subject != NULL) {

		nxs_string_clone(&u_ctx->value.new_issue.subject, subject);
	}

	if(description != NULL) {

		u_ctx->value.new_issue.is_private = nxs_chat_srv_c_notes_check_private(description, &u_ctx->value.new_issue.description);
		u_ctx->value.new_issue.is_ext =
		        nxs_chat_srv_c_notes_check_ext(&u_ctx->value.new_issue.description, &u_ctx->value.new_issue.description);
	}

	if(project_name_regex != NULL) {

		if(nxs_chat_srv_u_db_sess_s_new_issue_regex_set(&u_ctx->value.new_issue, project_name_regex) != NXS_CHAT_SRV_E_OK) {

			return NXS_CHAT_SRV_E_ERR;
		}
	}

	return nxs_chat_srv_u_db_sess_s_value_put(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_t_conv_to_new_issue(nxs_chat_srv_u_db_sess_t *u_ctx,
                                                              nxs_array_t *             cache_projects,
                                                              size_t                    rdmn_userid,
                                                              size_t                    priority_id,
                                                              nxs_string_t *            priority_name,
                                                              nxs_string_t *            subject,
                                                              nxs_string_t *            description,
                                                              nxs_string_t *            project_name_regex)
{
	nxs_chat_srv_err_t rc;
	nxs_string_t *     s_f, *s_t;
	size_t             i;

	if(u_ctx == NULL || cache_projects == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	rc = NXS_CHAT_SRV_E_OK;

	/* u_ctx->value.type - current type ("from" type) */
	switch(u_ctx->value.type) {

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE:

			if(nxs_chat_srv_u_db_sess_pull_user(&u_ctx->value.new_issue, rdmn_userid, cache_projects) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			u_ctx->value.type     = NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE;
			u_ctx->value.wait_for = NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;

			u_ctx->value.new_issue.priority_id = priority_id;

			u_ctx->value.new_issue.is_private = u_ctx->value.message.is_private;
			u_ctx->value.new_issue.is_ext     = u_ctx->value.message.is_ext;

			nxs_string_clone(&u_ctx->value.new_issue.priority_name, priority_name);
			nxs_string_clone(&u_ctx->value.new_issue.subject, subject);
			nxs_string_clone(&u_ctx->value.new_issue.description, &u_ctx->value.message.message);

			if(nxs_chat_srv_u_db_sess_s_new_issue_regex_set(&u_ctx->value.new_issue, project_name_regex) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			for(i = 0; i < nxs_array_count(&u_ctx->value.message.file_ids); i++) {

				s_f = nxs_array_get(&u_ctx->value.message.file_ids, i);

				s_t = nxs_array_add(&u_ctx->value.new_issue.file_ids);

				nxs_string_init3(s_t, s_f);
			}

			nxs_chat_srv_u_db_sess_s_new_issue_set_selected_project_first(&u_ctx->value.new_issue);

			nxs_chat_srv_u_db_sess_s_message_clear(&u_ctx->value.message);

			if(nxs_chat_srv_u_db_sess_s_value_put(u_ctx) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		default:

			nxs_error(rc, NXS_CHAT_SRV_E_TYPE, error);
	}

error:

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_t_conv_to_message(nxs_chat_srv_u_db_sess_t *u_ctx, nxs_string_t *message)
{
	nxs_chat_srv_err_t rc;
	nxs_string_t *     s_f, *s_t;
	size_t             i;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	rc = NXS_CHAT_SRV_E_OK;

	/* u_ctx->value.type - current type ("from" type) */
	switch(u_ctx->value.type) {

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE:

			u_ctx->value.type     = NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE;
			u_ctx->value.wait_for = NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;

			u_ctx->value.message.is_private = u_ctx->value.new_issue.is_private;
			u_ctx->value.message.is_ext     = u_ctx->value.new_issue.is_ext;

			nxs_string_clone(&u_ctx->value.message.message, &u_ctx->value.new_issue.description);

			for(i = 0; i < nxs_array_count(&u_ctx->value.new_issue.file_ids); i++) {

				s_f = nxs_array_get(&u_ctx->value.new_issue.file_ids, i);

				s_t = nxs_array_add(&u_ctx->value.message.file_ids);

				nxs_string_init3(s_t, s_f);
			}

			nxs_chat_srv_u_db_sess_s_new_issue_clear(&u_ctx->value.new_issue);

			if(nxs_chat_srv_u_db_sess_s_value_put(u_ctx) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		default:

			nxs_error(rc, NXS_CHAT_SRV_E_TYPE, error);
	}

error:

	return rc;
}

/* Module internal (static) functions */

/* get user memberships from Redmine */
static nxs_chat_srv_err_t
        nxs_chat_srv_u_db_sess_pull_user(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue, size_t rdmn_userid, nxs_array_t *cache_projects)
{
	nxs_buf_t                          out_buf;
	nxs_chat_srv_err_t                 rc;
	nxs_chat_srv_u_db_sess_project_t * p;
	nxs_chat_srv_m_db_cache_project_t *cp;
	nxs_bool_t                         f;
	size_t                             i;

	if(new_issue == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_buf_init2(&out_buf);

	if(nxs_chat_srv_d_rdmn_users_get(rdmn_userid, NXS_YES, &nxs_chat_srv_cfg.rdmn.api_key, 0, 0, NULL, &out_buf) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_u_db_sess_pull_user_extract(new_issue, &out_buf)) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	/* remove inactive projects from user available projects list */
	for(p = nxs_list_ptr_init(&new_issue->available_projects, NXS_LIST_PTR_INIT_HEAD); p != NULL;) {

		for(f = NXS_NO, i = 0; i < nxs_array_count(cache_projects); i++) {

			cp = nxs_array_get(cache_projects, i);

			if(p->id == cp->id) {

				f = NXS_YES;

				break;
			}
		}

		if(f == NXS_YES) {

			p = nxs_list_ptr_next(&new_issue->available_projects);
		}
		else {

			p->id = 0;

			nxs_string_free(&p->name);

			p = nxs_list_del(&new_issue->available_projects, NXS_LIST_MOVE_NEXT);
		}
	}

error:

	nxs_buf_free(&out_buf);

	return rc;
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_pull_user_extract(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_user,	new_issue,	&nxs_chat_srv_u_db_sess_pull_user_extract_user,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: tlgrm db-sess unit error: parse rdmn users error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_u_db_sess_pull_user_extract_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                        cfg_json;
	nxs_array_t                           cfg_arr;
	nxs_cfg_json_state_t                  rc;

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_memberships,	&new_issue->available_projects,	NULL,	&nxs_chat_srv_u_db_sess_pull_user_extract_user_memberships,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(
		        &process, "[%s]: tlgrm db-sess unit error: parse out_buf error (object: \"user\")", nxs_proc_get_name(&process));

		rc = NXS_CFG_JSON_CONF_ERROR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_u_db_sess_pull_user_extract_user_memberships(nxs_process_t *     proc,
                                                                                      nxs_json_t *        json,
                                                                                      nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                      nxs_array_t *       cfg_arr)
{
	nxs_list_t *                      projects = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_u_db_sess_project_t *p;

	p = nxs_list_add_tail(projects);

	p->id = 0;

	nxs_string_init(&p->name);

	nxs_cfg_json_conf_array_skip_undef(cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_project,	p,	&nxs_chat_srv_u_db_sess_pull_user_extract_user_memberships_project,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t nxs_chat_srv_u_db_sess_pull_user_extract_user_memberships_project(nxs_process_t *     proc,
                                                                                              nxs_json_t *        json,
                                                                                              nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_u_db_sess_project_t *p = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                    cfg_json;
	nxs_array_t                       cfg_arr;
	nxs_cfg_json_state_t              rc;

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
		                    "[%s]: tlgrm db-sess unit error: parse out_buf error (object: \"user.memberships[].project\")",
		                    nxs_proc_get_name(&process));

		rc = NXS_CFG_JSON_CONF_ERROR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static void nxs_chat_srv_u_db_sess_s_value_init(nxs_chat_srv_u_db_sess_t_value_t *value)
{

	if(value == NULL) {

		return;
	}

	value->chat_id        = 0;
	value->usr_message_id = 0;
	value->bot_message_id = 0;
	value->updated_at     = 0;
	value->type           = NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE;
	value->wait_for       = NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;

	nxs_string_init_empty(&value->rdmn_api_key);

	nxs_chat_srv_u_db_sess_s_message_init(&value->message);
	nxs_chat_srv_u_db_sess_s_new_issue_init(&value->new_issue);
}

static void nxs_chat_srv_u_db_sess_s_value_free(nxs_chat_srv_u_db_sess_t_value_t *value)
{

	if(value == NULL) {

		return;
	}

	value->chat_id        = 0;
	value->usr_message_id = 0;
	value->bot_message_id = 0;
	value->updated_at     = 0;
	value->type           = NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE;
	value->wait_for       = NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;

	nxs_string_free(&value->rdmn_api_key);

	nxs_chat_srv_u_db_sess_s_message_free(&value->message);
	nxs_chat_srv_u_db_sess_s_new_issue_free(&value->new_issue);
}

static void nxs_chat_srv_u_db_sess_s_value_clear(nxs_chat_srv_u_db_sess_t_value_t *value)
{

	if(value == NULL) {

		return;
	}

	value->chat_id        = 0;
	value->usr_message_id = 0;
	value->bot_message_id = 0;
	value->updated_at     = 0;
	value->type           = NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE;
	value->wait_for       = NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;

	nxs_string_clear(&value->rdmn_api_key);

	nxs_chat_srv_u_db_sess_s_message_clear(&value->message);
	nxs_chat_srv_u_db_sess_s_new_issue_clear(&value->new_issue);
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_value_get(nxs_chat_srv_u_db_sess_t *u_ctx)
{
	nxs_string_t       value_str;
	nxs_chat_srv_err_t rc;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&value_str);

	switch(nxs_chat_srv_d_db_sess_get(u_ctx->sess_db_ctx, u_ctx->tlgrm_userid, &value_str)) {

		case NXS_CHAT_SRV_E_OK:

			if(nxs_chat_srv_u_db_sess_s_value_deserialize(&value_str, &u_ctx->value) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		case NXS_CHAT_SRV_E_EXIST:

			nxs_chat_srv_u_db_sess_s_value_clear(&u_ctx->value);

			break;

		default:

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_string_free(&value_str);

	return rc;
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_value_put(nxs_chat_srv_u_db_sess_t *u_ctx)
{
	nxs_string_t       value_str;
	nxs_chat_srv_err_t rc;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_string_init(&value_str);

	u_ctx->value.updated_at = time(NULL);

	if(nxs_chat_srv_u_db_sess_s_value_serialize(&value_str, &u_ctx->value) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if(nxs_chat_srv_d_db_sess_put(u_ctx->sess_db_ctx, u_ctx->tlgrm_userid, &value_str) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_string_free(&value_str);

	return rc;
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_value_serialize(nxs_string_t *value_str, nxs_chat_srv_u_db_sess_t_value_t *value)
{
	nxs_string_t       data_str;
	nxs_chat_srv_err_t rc;

	if(value_str == NULL || value == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	nxs_string_init(&data_str);

	rc = NXS_CHAT_SRV_E_OK;

	switch(value->type) {

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE:

			nxs_chat_srv_u_db_sess_s_message_serialize(&value->message, &data_str);

			break;

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE:

			nxs_chat_srv_u_db_sess_s_new_issue_serialize(&value->new_issue, &data_str);

			break;

		default:

			nxs_log_write_warn(&process,
			                   "[%s]: session value serialize error: incorrect type (chat_id: %zu, type: %d, wait-for: %d)",
			                   nxs_proc_get_name(&process),
			                   value->chat_id,
			                   value->type,
			                   value->wait_for);

			nxs_error(rc, NXS_CHAT_SRV_E_TYPE, error);
	}

	nxs_string_printf(value_str,
	                  "{"
	                  "\"rdmn_api_key\":\"%r\","
	                  "\"chat_id\":%zu,"
	                  "\"usr_message_id\":%zu,"
	                  "\"bot_message_id\":%zu,"
	                  "\"updated_at\":%zu,"
	                  "\"type\":%d,"
	                  "\"wait_for\":%d,"
	                  "\"data\":%r"
	                  "}",
	                  &value->rdmn_api_key,
	                  value->chat_id,
	                  value->usr_message_id,
	                  value->bot_message_id,
	                  value->updated_at,
	                  value->type,
	                  value->wait_for,
	                  &data_str);

error:

	nxs_string_free(&data_str);

	return rc;
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_value_deserialize(nxs_string_t *value_str, nxs_chat_srv_u_db_sess_t_value_t *value)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;
	nxs_json_t *       json, *j;

	if(value_str == NULL || value == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_json_init(&json);

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	if(nxs_json_read_mem(&process, &json, (nxs_buf_t *)value_str, 0) != NXS_JSON_E_OK) {

		nxs_log_write_error(&process, "[%s]: tlgrm db-sess unit error: read json from value error", nxs_proc_get_name(&process));

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_rdmn_api_key,	&value->rdmn_api_key,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_chat_id,	&value->chat_id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_usr_message_id,	&value->usr_message_id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_bot_message_id,	&value->bot_message_id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_updated_at,	&value->updated_at,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_type,		&value->type,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT_32,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_wait_for,	&value->wait_for,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT_32,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_cfg_json_free(&cfg_json);

		nxs_log_write_error(&process, "[%s]: tlgrm db-sess unit error: value deserialize error", nxs_proc_get_name(&process));

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_cfg_json_free(&cfg_json);

	if((j = nxs_json_child_get_by_key(json, nxs_string_str(&_s_par_data))) == NULL) {

		nxs_log_write_error(
		        &process, "[%s]: tlgrm db-sess unit error: can't find field 'data' in value", nxs_proc_get_name(&process));

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	switch(value->type) {

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE:

			if(nxs_chat_srv_u_db_sess_s_message_deserialize(&value->message, j) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE:

			if(nxs_chat_srv_u_db_sess_s_new_issue_deserialize(&value->new_issue, j) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		default:

			nxs_log_write_warn(&process,
			                   "[%s]: session value serialize error: incorrect type (chat_id: %zu, type: %d, wait-for: %d)",
			                   nxs_proc_get_name(&process),
			                   value->chat_id,
			                   value->type,
			                   value->wait_for);

			nxs_error(rc, NXS_CHAT_SRV_E_TYPE, error);
	}

error:

	nxs_json_free(&json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static void nxs_chat_srv_u_db_sess_s_message_init(nxs_chat_srv_u_db_sess_t_message_t *message)
{

	if(message == NULL) {

		return;
	}

	message->is_private = NXS_NO;
	message->is_ext     = NXS_NO;

	nxs_string_init_empty(&message->message);

	nxs_array_init2(&message->file_ids, nxs_string_t);
}

static void nxs_chat_srv_u_db_sess_s_message_free(nxs_chat_srv_u_db_sess_t_message_t *message)
{
	nxs_string_t *s;
	size_t        i;

	if(message == NULL) {

		return;
	}

	message->is_private = NXS_NO;
	message->is_ext     = NXS_NO;

	nxs_string_free(&message->message);

	for(i = 0; i < nxs_array_count(&message->file_ids); i++) {

		s = nxs_array_get(&message->file_ids, i);

		nxs_string_free(s);
	}

	nxs_array_free(&message->file_ids);
}

static void nxs_chat_srv_u_db_sess_s_message_clear(nxs_chat_srv_u_db_sess_t_message_t *message)
{
	nxs_string_t *s;
	size_t        i;

	if(message == NULL) {

		return;
	}

	message->is_private = NXS_NO;
	message->is_ext     = NXS_NO;

	nxs_string_clear(&message->message);

	for(i = 0; i < nxs_array_count(&message->file_ids); i++) {

		s = nxs_array_get(&message->file_ids, i);

		nxs_string_free(s);
	}

	nxs_array_clear(&message->file_ids);
}

static void nxs_chat_srv_u_db_sess_s_message_serialize(nxs_chat_srv_u_db_sess_t_message_t *message, nxs_string_t *out_str)
{
	nxs_string_t message_encoded, files, *s;
	size_t       i;

	if(message == NULL || out_str == NULL) {

		return;
	}

	nxs_string_init(&message_encoded);
	nxs_string_init(&files);

	for(i = 0; i < nxs_array_count(&message->file_ids); i++) {

		s = nxs_array_get(&message->file_ids, i);

		if(i > 0) {

			nxs_string_char_add_char(&files, (u_char)',');
		}

		nxs_string_printf2_cat(&files, "\"%r\"", s);
	}

	nxs_base64_encode_string(&message_encoded, &message->message);

	nxs_string_printf(out_str,
	                  "{\"message\":\"%r\",\"is_private\":%s,\"is_ext\":%s,\"file_ids\":[%r]}",
	                  &message_encoded,
	                  message->is_private == NXS_YES ? "true" : "false",
	                  message->is_ext == NXS_YES ? "true" : "false",
	                  &files);

	nxs_string_free(&message_encoded);
	nxs_string_free(&files);
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_message_deserialize(nxs_chat_srv_u_db_sess_t_message_t *message, nxs_json_t *json_data)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;
	nxs_string_t       message_encoded;

	if(message == NULL || json_data == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_string_init(&message_encoded);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_message,	&message_encoded,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_is_private,	&message->is_private,	NULL,	NULL,	NXS_CFG_JSON_TYPE_BOOL,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_is_ext,		&message->is_ext,	NULL,	NULL,	NXS_CFG_JSON_TYPE_BOOL,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_file_ids,	&message->file_ids,	NULL,	NULL,	NXS_CFG_JSON_TYPE_ARRAY_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json_data) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: tlgrm db-sess unit error: message deserialize error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}
	else {

		nxs_base64_decode_string(&message->message, &message_encoded);
	}

	nxs_string_free(&message_encoded);

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static void nxs_chat_srv_u_db_sess_s_new_issue_init(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue)
{

	if(new_issue == NULL) {

		return;
	}

	new_issue->priority_id = 0;
	new_issue->project_id  = 0;
	new_issue->is_private  = NXS_NO;
	new_issue->is_ext      = NXS_NO;

	nxs_string_init_empty(&new_issue->description);
	nxs_string_init_empty(&new_issue->priority_name);
	nxs_string_init_empty(&new_issue->project_name);
	nxs_string_init_empty(&new_issue->subject);
	nxs_string_init_empty(&new_issue->project_name_regex);

	nxs_list_init2(&new_issue->available_projects, nxs_chat_srv_u_db_sess_project_t);
	nxs_array_init2(&new_issue->selected_projects, nxs_chat_srv_u_db_sess_project_t);
	nxs_array_init2(&new_issue->file_ids, nxs_string_t);
}

static void nxs_chat_srv_u_db_sess_s_new_issue_free(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue)
{
	nxs_chat_srv_u_db_sess_project_t *p;
	nxs_string_t *                    s;
	size_t                            i;

	if(new_issue == NULL) {

		return;
	}

	new_issue->priority_id = 0;
	new_issue->project_id  = 0;
	new_issue->is_private  = NXS_NO;
	new_issue->is_ext      = NXS_NO;

	nxs_string_free(&new_issue->description);
	nxs_string_free(&new_issue->priority_name);
	nxs_string_free(&new_issue->project_name);
	nxs_string_free(&new_issue->subject);
	nxs_string_free(&new_issue->project_name_regex);

	for(p = nxs_list_ptr_init(&new_issue->available_projects, NXS_LIST_PTR_INIT_HEAD); p != NULL;
	    p = nxs_list_del(&new_issue->available_projects, NXS_LIST_MOVE_NEXT)) {

		p->id = 0;

		nxs_string_free(&p->name);
	}

	for(i = 0; i < nxs_array_count(&new_issue->selected_projects); i++) {

		p = nxs_array_get(&new_issue->selected_projects, i);

		p->id = 0;

		nxs_string_free(&p->name);
	}

	for(i = 0; i < nxs_array_count(&new_issue->file_ids); i++) {

		s = nxs_array_get(&new_issue->file_ids, i);

		nxs_string_free(s);
	}

	nxs_list_free(&new_issue->available_projects);
	nxs_array_free(&new_issue->selected_projects);
	nxs_array_free(&new_issue->file_ids);
}

static void nxs_chat_srv_u_db_sess_s_new_issue_clear(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue)
{
	nxs_chat_srv_u_db_sess_project_t *p;
	nxs_string_t *                    s;
	size_t                            i;

	if(new_issue == NULL) {

		return;
	}

	new_issue->priority_id = 0;
	new_issue->project_id  = 0;
	new_issue->is_private  = NXS_NO;
	new_issue->is_ext      = NXS_NO;

	nxs_string_clear(&new_issue->description);
	nxs_string_clear(&new_issue->priority_name);
	nxs_string_clear(&new_issue->project_name);
	nxs_string_clear(&new_issue->subject);
	nxs_string_clear(&new_issue->project_name_regex);

	for(p = nxs_list_ptr_init(&new_issue->available_projects, NXS_LIST_PTR_INIT_HEAD); p != NULL;
	    p = nxs_list_del(&new_issue->available_projects, NXS_LIST_MOVE_NEXT)) {

		p->id = 0;

		nxs_string_free(&p->name);
	}

	for(i = 0; i < nxs_array_count(&new_issue->selected_projects); i++) {

		p = nxs_array_get(&new_issue->selected_projects, i);

		p->id = 0;

		nxs_string_free(&p->name);
	}

	for(i = 0; i < nxs_array_count(&new_issue->file_ids); i++) {

		s = nxs_array_get(&new_issue->file_ids, i);

		nxs_string_free(s);
	}

	nxs_array_clear(&new_issue->selected_projects);
	nxs_array_clear(&new_issue->file_ids);
}

static void nxs_chat_srv_u_db_sess_s_new_issue_set_selected_project_first(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue)
{
	nxs_chat_srv_u_db_sess_project_t *p;

	if(new_issue == NULL) {

		return;
	}

	if((p = nxs_array_get(&new_issue->selected_projects, 0)) != NULL) {

		new_issue->project_id = p->id;

		nxs_string_clone(&new_issue->project_name, &p->name);
	}
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_new_issue_set_project_by_id(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue,
                                                                               size_t                                project_id)
{
	nxs_chat_srv_u_db_sess_project_t *p;
	nxs_chat_srv_err_t                rc;

	if(new_issue == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_EXIST;

	for(p = nxs_list_ptr_init(&new_issue->available_projects, NXS_LIST_PTR_INIT_HEAD); p != NULL;
	    p = nxs_list_ptr_next(&new_issue->available_projects)) {

		if(p->id == project_id) {

			new_issue->project_id = p->id;

			nxs_string_clone(&new_issue->project_name, &p->name);

			nxs_error(rc, NXS_CHAT_SRV_E_OK, error);
		}
	}

error:

	return rc;
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_new_issue_regex_set(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue,
                                                                       nxs_string_t *                        regex_str)
{
	nxs_chat_srv_err_t                rc;
	nxs_chat_srv_u_db_sess_project_t *p1, *p2;
	regex_t                           regex;
	size_t                            i;
	int                               ec;
	char                              msgbuf[100];

	if(new_issue == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	/* clean up regex context before filling it with new data */

	if(regex_str != NULL) {

		nxs_string_clone(&new_issue->project_name_regex, regex_str);
	}

	for(i = 0; i < nxs_array_count(&new_issue->selected_projects); i++) {

		p1 = nxs_array_get(&new_issue->selected_projects, i);

		p1->id = 0;

		nxs_string_free(&p1->name);
	}

	nxs_array_clear(&new_issue->selected_projects);

	if(nxs_string_len(&new_issue->project_name_regex) == 0) {

		/* when no regex */

		for(p1 = nxs_list_ptr_init(&new_issue->available_projects, NXS_LIST_PTR_INIT_HEAD); p1 != NULL;
		    p1 = nxs_list_ptr_next(&new_issue->available_projects)) {

			p2 = nxs_array_add(&new_issue->selected_projects);

			p2->id = p1->id;

			nxs_string_init3(&p2->name, &p1->name);
		}
	}
	else {

		/* when using regex */

		if(regcomp(&regex, (char *)nxs_string_str(&new_issue->project_name_regex), REG_ICASE) != 0) {

			nxs_log_write_error(&process,
			                    "db-sess set projects name regex error: could not compile regex (regex string: \"%r\")",
			                    &new_issue->project_name_regex);

			return NXS_CHAT_SRV_E_ERR;
		}

		for(p1 = nxs_list_ptr_init(&new_issue->available_projects, NXS_LIST_PTR_INIT_HEAD); p1 != NULL;
		    p1 = nxs_list_ptr_next(&new_issue->available_projects)) {

			if((ec = regexec(&regex, (char *)nxs_string_str(&p1->name), 0, NULL, 0)) == 0) {

				/* regex match */

				p2 = nxs_array_add(&new_issue->selected_projects);

				p2->id = p1->id;

				nxs_string_init3(&p2->name, &p1->name);
			}
			else {

				if(ec != REG_NOMATCH) {

					regerror(ec, &regex, msgbuf, sizeof(msgbuf));

					nxs_log_write_error(
					        &process,
					        "db-sess set projects name regex error: regex failed (regex string: \"%r\", error: %s)",
					        &new_issue->project_name_regex,
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

static void nxs_chat_srv_u_db_sess_s_new_issue_serialize(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue, nxs_string_t *out_str)
{
	nxs_string_t description_encoded, priority_name_encoded, project_name_encoded, subject_encoded, project_name_regex_encoded,
	        available_projects, selected_projects, files, *s;
	nxs_chat_srv_u_db_sess_project_t *p;
	size_t                            i;
	nxs_bool_t                        f;

	if(new_issue == NULL || out_str == NULL) {

		return;
	}

	nxs_string_init_empty(&available_projects);
	nxs_string_init_empty(&selected_projects);
	nxs_string_init(&description_encoded);
	nxs_string_init(&priority_name_encoded);
	nxs_string_init(&project_name_encoded);
	nxs_string_init(&subject_encoded);
	nxs_string_init(&project_name_regex_encoded);

	for(f = NXS_NO, p = nxs_list_ptr_init(&new_issue->available_projects, NXS_LIST_PTR_INIT_HEAD); p != NULL;
	    p = nxs_list_ptr_next(&new_issue->available_projects)) {

		if(f == NXS_YES) {

			nxs_string_char_add_char(&available_projects, (u_char)',');
		}
		else {

			f = NXS_YES;
		}

		nxs_base64_encode_string(&project_name_encoded, &p->name);

		nxs_string_printf2_cat(&available_projects, "{\"id\":%zu,\"name\":\"%r\"}", p->id, &project_name_encoded);
	}

	for(i = 0; i < nxs_array_count(&new_issue->selected_projects); i++) {

		if(i > 0) {

			nxs_string_char_add_char(&selected_projects, (u_char)',');
		}

		p = nxs_array_get(&new_issue->selected_projects, i);

		nxs_base64_encode_string(&project_name_encoded, &p->name);

		nxs_string_printf2_cat(&selected_projects, "{\"id\":%zu,\"name\":\"%r\"}", p->id, &project_name_encoded);
	}

	nxs_string_init(&files);

	for(i = 0; i < nxs_array_count(&new_issue->file_ids); i++) {

		s = nxs_array_get(&new_issue->file_ids, i);

		if(i > 0) {

			nxs_string_char_add_char(&files, (u_char)',');
		}

		nxs_string_printf2_cat(&files, "\"%r\"", s);
	}

	nxs_base64_encode_string(&description_encoded, &new_issue->description);
	nxs_base64_encode_string(&priority_name_encoded, &new_issue->priority_name);
	nxs_base64_encode_string(&project_name_encoded, &new_issue->project_name);
	nxs_base64_encode_string(&subject_encoded, &new_issue->subject);
	nxs_base64_encode_string(&project_name_regex_encoded, &new_issue->project_name_regex);

	nxs_string_printf(out_str,
	                  "{\"project_id\":%zu,"
	                  "\"project_name\":\"%r\","
	                  "\"priority_id\":%zu,"
	                  "\"priority_name\":\"%r\","
	                  "\"subject\":\"%r\","
	                  "\"description\":\"%r\","
	                  "\"is_private\":%s,"
	                  "\"is_ext\":%s,"
	                  "\"project_name_regex\":\"%r\","
	                  "\"available_projects\":[%r],"
	                  "\"selected_projects\":[%r],"
	                  "\"file_ids\":[%r]}",
	                  new_issue->project_id,
	                  &project_name_encoded,
	                  new_issue->priority_id,
	                  &priority_name_encoded,
	                  &subject_encoded,
	                  &description_encoded,
	                  new_issue->is_private == NXS_YES ? "true" : "false",
	                  new_issue->is_ext == NXS_YES ? "true" : "false",
	                  &project_name_regex_encoded,
	                  &available_projects,
	                  &selected_projects,
	                  &files);

	nxs_string_free(&available_projects);
	nxs_string_free(&selected_projects);
	nxs_string_free(&description_encoded);
	nxs_string_free(&priority_name_encoded);
	nxs_string_free(&project_name_encoded);
	nxs_string_free(&subject_encoded);
	nxs_string_free(&project_name_regex_encoded);
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_new_issue_deserialize(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue,
                                                                         nxs_json_t *                          json_data)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;
	nxs_string_t       project_name, priority_name, subject, description, project_name_regex;

	if(new_issue == NULL || json_data == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&subject);
	nxs_string_init(&description);
	nxs_string_init(&project_name_regex);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_project_id,		&new_issue->project_id,		NULL,	NULL,									NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_project_name,		&project_name,			NULL,	NULL,									NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_priority_id,		&new_issue->priority_id,	NULL,	NULL,									NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_priority_name,		&priority_name,			NULL,	NULL,									NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_subject,		&subject,			NULL,	NULL,									NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_description,		&description,			NULL,	NULL,									NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_is_private,		&new_issue->is_private,		NULL,	NULL,									NXS_CFG_JSON_TYPE_BOOL,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_is_ext,			&new_issue->is_ext,		NULL,	NULL,									NXS_CFG_JSON_TYPE_BOOL,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_project_name_regex,	&project_name_regex,		NULL,	NULL,									NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_available_projects,	&new_issue->available_projects,	NULL,	&nxs_chat_srv_u_db_sess_s_new_issue_deserialize_projects_available,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_selected_projects,	&new_issue->selected_projects,	NULL,	&nxs_chat_srv_u_db_sess_s_new_issue_deserialize_projects_selected,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_file_ids,		&new_issue->file_ids,		NULL,	NULL,									NXS_CFG_JSON_TYPE_ARRAY_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json_data) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: tlgrm db-sess unit error: new issue deserialize error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}
	else {

		nxs_base64_decode_string(&new_issue->project_name, &project_name);
		nxs_base64_decode_string(&new_issue->priority_name, &priority_name);
		nxs_base64_decode_string(&new_issue->subject, &subject);
		nxs_base64_decode_string(&new_issue->description, &description);
		nxs_base64_decode_string(&new_issue->project_name_regex, &project_name_regex);
	}

	nxs_string_free(&project_name);
	nxs_string_free(&priority_name);
	nxs_string_free(&subject);
	nxs_string_free(&description);
	nxs_string_free(&project_name_regex);

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_u_db_sess_s_new_issue_deserialize_projects_available(nxs_process_t *     proc,
                                                                                              nxs_json_t *        json,
                                                                                              nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                              nxs_array_t *       cfg_arr)
{
	nxs_list_t *                      projects = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_u_db_sess_project_t *p;

	p = nxs_list_add_tail(projects);

	p->id = 0;

	nxs_string_init(&p->name);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_id,	&p->id,		NULL,								NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_name,	&p->name,	&nxs_chat_srv_u_db_sess_s_new_issue_deserialize_projects_name,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t nxs_chat_srv_u_db_sess_s_new_issue_deserialize_projects_selected(nxs_process_t *     proc,
                                                                                             nxs_json_t *        json,
                                                                                             nxs_cfg_json_par_t *cfg_json_par_el,
                                                                                             nxs_array_t *       cfg_arr)
{
	nxs_array_t *                     projects = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_u_db_sess_project_t *p;

	p = nxs_array_add(projects);

	p->id = 0;

	nxs_string_init(&p->name);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_id,	&p->id,		NULL,								NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_name,	&p->name,	&nxs_chat_srv_u_db_sess_s_new_issue_deserialize_projects_name,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t nxs_chat_srv_u_db_sess_s_new_issue_deserialize_projects_name(nxs_process_t *     proc,
                                                                                         nxs_json_t *        json,
                                                                                         nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_string_t *name = nxs_cfg_json_get_val(cfg_json_par_el);

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_STRING) {

		nxs_log_write_error(
		        &process,
		        "[%s]: tlgrm db-sess unit error: wrong type for available or selected projects name in new issue deserialize",
		        nxs_proc_get_name(&process));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	nxs_base64_decode_string(name, nxs_json_string_val(json));

	return NXS_CFG_JSON_CONF_OK;
}
