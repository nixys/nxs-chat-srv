#ifndef _INCLUDE_NXS_CHAT_SRV_U_DB_SESS_C
#define _INCLUDE_NXS_CHAT_SRV_U_DB_SESS_C
#endif

// clang-format off

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
	size_t						project_id;		/* redmine project id */
	nxs_string_t					project_name;		/* redmine project name */
	size_t						priority_id;		/* redmine priority id */
	nxs_string_t					priority_name;		/* redmine priority name */
	nxs_string_t					subject;		/* redmine issue subject */
	nxs_string_t					description;		/* redmine issue description */
	nxs_string_t					project_name_regex;	/* regex to filter redmine projects name */
} nxs_chat_srv_u_db_sess_t_new_issue_t;

/*
 * Using in session to send message to specified redmine issue
 */
typedef struct
{
	nxs_string_t					message;		/* message to append into redmine issue */
} nxs_chat_srv_u_db_sess_t_message_t;

typedef struct
{
	size_t						chat_id;		/* chat id where session was started */
	size_t						message_id;		/* bot message id where session was started */
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
static void nxs_chat_srv_u_db_sess_s_new_issue_serialize(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue, nxs_string_t *out_str);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_new_issue_deserialize(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue,
                                                                         nxs_json_t *                          json_data);

// clang-format off

/* Module initializations */

static nxs_string_t _s_par_chat_id		= nxs_string("chat_id");
static nxs_string_t _s_par_message_id		= nxs_string("message_id");
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
                                                size_t                                 chat_id,
                                                size_t                                 message_id,
                                                nxs_chat_srv_m_db_sess_type_t          type,
                                                nxs_chat_srv_m_db_sess_wait_for_type_t wait_for)
{

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	u_ctx->value.chat_id    = chat_id;
	u_ctx->value.message_id = message_id;
	u_ctx->value.type       = type;
	u_ctx->value.wait_for   = wait_for;

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

size_t nxs_chat_srv_u_db_sess_get_message_id(nxs_chat_srv_u_db_sess_t *u_ctx)
{

	if(u_ctx == NULL) {

		return 0;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return 0;
	}

	return u_ctx->value.message_id;
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

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_set_ids(nxs_chat_srv_u_db_sess_t *u_ctx, size_t chat_id, size_t message_id)
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

	if(message_id > 0) {

		u_ctx->value.message_id = message_id;
	}

	return nxs_chat_srv_u_db_sess_s_value_put(u_ctx);
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

		nxs_log_write_warn(&process,
		                   "[%s]: can't get session message: session type incorrect (session type: %d, expected type: %d)",
		                   nxs_proc_get_name(&process),
		                   u_ctx->value.type,
		                   NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE);

		return NULL;
	}

	return &u_ctx->value.message.message;
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

		nxs_log_write_warn(&process,
		                   "[%s]: can't set session message: session type incorrect (session type: %d, expected type: %d)",
		                   nxs_proc_get_name(&process),
		                   u_ctx->value.type,
		                   NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE);

		return NXS_CHAT_SRV_E_TYPE;
	}

	nxs_string_clone(&u_ctx->value.message.message, message);

	return nxs_chat_srv_u_db_sess_s_value_put(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_t_get_new_issue(nxs_chat_srv_u_db_sess_t *u_ctx,
                                                          size_t *                  project_id,
                                                          nxs_string_t *            project_name,
                                                          size_t *                  priority_id,
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
		                   "[%s]: can't get session new issue: session type incorrect (session type: %d, expected type: %d)",
		                   nxs_proc_get_name(&process),
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

	if(project_name_regex != NULL) {

		nxs_string_clone(project_name_regex, &u_ctx->value.new_issue.project_name_regex);
	}

	return NXS_CHAT_SRV_E_OK;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_t_set_new_issue(nxs_chat_srv_u_db_sess_t *u_ctx,
                                                          size_t                    project_id,
                                                          nxs_string_t *            project_name,
                                                          size_t                    priority_id,
                                                          nxs_string_t *            priority_name,
                                                          nxs_string_t *            subject,
                                                          nxs_string_t *            description,
                                                          nxs_string_t *            project_name_regex)
{
	if(u_ctx == NULL || project_name == NULL || priority_name == NULL || subject == NULL || description == NULL ||
	   project_name_regex == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(u_ctx->value.type != NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE) {

		nxs_log_write_warn(&process,
		                   "[%s]: can't set session new issue: session type incorrect (session type: %d, expected type: %d)",
		                   nxs_proc_get_name(&process),
		                   u_ctx->value.type,
		                   NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE);

		return NXS_CHAT_SRV_E_TYPE;
	}

	if(project_id > 0) {

		u_ctx->value.new_issue.project_id = project_id;
	}

	if(priority_id > 0) {

		u_ctx->value.new_issue.priority_id = priority_id;
	}

	if(project_name != NULL) {

		nxs_string_clone(&u_ctx->value.new_issue.project_name, project_name);
	}
	if(priority_name != NULL) {

		nxs_string_clone(&u_ctx->value.new_issue.priority_name, priority_name);
	}
	if(subject != NULL) {

		nxs_string_clone(&u_ctx->value.new_issue.subject, subject);
	}
	if(description != NULL) {

		nxs_string_clone(&u_ctx->value.new_issue.description, description);
	}
	if(project_name_regex != NULL) {

		nxs_string_clone(&u_ctx->value.new_issue.project_name_regex, project_name_regex);
	}

	return nxs_chat_srv_u_db_sess_s_value_put(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_t_conv_to_new_issue(nxs_chat_srv_u_db_sess_t *u_ctx,
                                                              size_t                    project_id,
                                                              nxs_string_t *            project_name,
                                                              size_t                    priority_id,
                                                              nxs_string_t *            priority_name,
                                                              nxs_string_t *            subject,
                                                              nxs_string_t *            description,
                                                              nxs_string_t *            project_name_regex)
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

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE:

			u_ctx->value.type     = NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE;
			u_ctx->value.wait_for = NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;

			u_ctx->value.new_issue.project_id  = project_id;
			u_ctx->value.new_issue.priority_id = priority_id;

			nxs_string_clone(&u_ctx->value.new_issue.project_name, project_name);
			nxs_string_clone(&u_ctx->value.new_issue.priority_name, priority_name);
			nxs_string_clone(&u_ctx->value.new_issue.subject, subject);
			nxs_string_clone(&u_ctx->value.new_issue.description, &u_ctx->value.message.message);
			nxs_string_clone(&u_ctx->value.new_issue.project_name_regex, project_name_regex);

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

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_chat_srv_u_db_sess_check_exist(u_ctx) == NXS_NO) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	rc = NXS_CHAT_SRV_E_OK;

	switch(u_ctx->value.type) {

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE:

			u_ctx->value.type     = NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE;
			u_ctx->value.wait_for = NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;

			nxs_string_clone(&u_ctx->value.message.message, &u_ctx->value.new_issue.description);

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

static void nxs_chat_srv_u_db_sess_s_value_init(nxs_chat_srv_u_db_sess_t_value_t *value)
{

	if(value == NULL) {

		return;
	}

	value->chat_id    = 0;
	value->message_id = 0;
	value->updated_at = 0;
	value->type       = NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE;
	value->wait_for   = NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;

	nxs_chat_srv_u_db_sess_s_message_init(&value->message);
	nxs_chat_srv_u_db_sess_s_new_issue_init(&value->new_issue);
}

static void nxs_chat_srv_u_db_sess_s_value_free(nxs_chat_srv_u_db_sess_t_value_t *value)
{

	if(value == NULL) {

		return;
	}

	value->chat_id    = 0;
	value->message_id = 0;
	value->updated_at = 0;
	value->type       = NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE;
	value->wait_for   = NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;

	nxs_chat_srv_u_db_sess_s_message_free(&value->message);
	nxs_chat_srv_u_db_sess_s_new_issue_free(&value->new_issue);
}

static void nxs_chat_srv_u_db_sess_s_value_clear(nxs_chat_srv_u_db_sess_t_value_t *value)
{

	if(value == NULL) {

		return;
	}

	value->chat_id    = 0;
	value->message_id = 0;
	value->updated_at = 0;
	value->type       = NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE;
	value->wait_for   = NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;

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
	                  "\"chat_id\":%zu,"
	                  "\"message_id\":%zu,"
	                  "\"updated_at\":%zu,"
	                  "\"type\":%d,"
	                  "\"wait_for\":%d,"
	                  "\"data\":%r"
	                  "}",
	                  value->chat_id,
	                  value->message_id,
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

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_chat_id,	&value->chat_id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_message_id,	&value->message_id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
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

	nxs_string_init_empty(&message->message);
}

static void nxs_chat_srv_u_db_sess_s_message_free(nxs_chat_srv_u_db_sess_t_message_t *message)
{

	if(message == NULL) {

		return;
	}

	nxs_string_free(&message->message);
}

static void nxs_chat_srv_u_db_sess_s_message_clear(nxs_chat_srv_u_db_sess_t_message_t *message)
{

	if(message == NULL) {

		return;
	}

	nxs_string_clear(&message->message);
}

static void nxs_chat_srv_u_db_sess_s_message_serialize(nxs_chat_srv_u_db_sess_t_message_t *message, nxs_string_t *out_str)
{
	nxs_string_t message_encoded;

	if(message == NULL || out_str == NULL) {

		return;
	}

	nxs_string_init(&message_encoded);

	nxs_base64_encode_string(&message_encoded, &message->message);

	nxs_string_printf(out_str, "{\"message\":\"%r\"}", &message_encoded);

	nxs_string_free(&message_encoded);
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_message_deserialize(nxs_chat_srv_u_db_sess_t_message_t *message, nxs_json_t *json_data)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;
	nxs_string_t       message_decoded;

	if(message == NULL || json_data == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	nxs_string_init(&message_decoded);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_message,	&message_decoded,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json_data) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: tlgrm db-sess unit error: message deserialize error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}
	else {

		nxs_base64_decode_string(&message->message, &message_decoded);
	}

	nxs_string_free(&message_decoded);

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

	nxs_string_init_empty(&new_issue->description);
	nxs_string_init_empty(&new_issue->priority_name);
	nxs_string_init_empty(&new_issue->project_name);
	nxs_string_init_empty(&new_issue->subject);
	nxs_string_init_empty(&new_issue->project_name_regex);
}

static void nxs_chat_srv_u_db_sess_s_new_issue_free(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue)
{

	if(new_issue == NULL) {

		return;
	}

	new_issue->priority_id = 0;
	new_issue->project_id  = 0;

	nxs_string_free(&new_issue->description);
	nxs_string_free(&new_issue->priority_name);
	nxs_string_free(&new_issue->project_name);
	nxs_string_free(&new_issue->subject);
	nxs_string_free(&new_issue->project_name_regex);
}

static void nxs_chat_srv_u_db_sess_s_new_issue_clear(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue)
{

	if(new_issue == NULL) {

		return;
	}

	new_issue->priority_id = 0;
	new_issue->project_id  = 0;

	nxs_string_clear(&new_issue->description);
	nxs_string_clear(&new_issue->priority_name);
	nxs_string_clear(&new_issue->project_name);
	nxs_string_clear(&new_issue->subject);
	nxs_string_clear(&new_issue->project_name_regex);
}

static void nxs_chat_srv_u_db_sess_s_new_issue_serialize(nxs_chat_srv_u_db_sess_t_new_issue_t *new_issue, nxs_string_t *out_str)
{
	nxs_string_t description_encoded, priority_name_encoded, project_name_encoded, subject_encoded, project_name_regex_encoded;

	if(new_issue == NULL || out_str == NULL) {

		return;
	}

	nxs_string_init(&description_encoded);
	nxs_string_init(&priority_name_encoded);
	nxs_string_init(&project_name_encoded);
	nxs_string_init(&subject_encoded);
	nxs_string_init(&project_name_regex_encoded);

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
	                  "\"project_name_regex\":\"%r\"}",
	                  new_issue->project_id,
	                  &project_name_encoded,
	                  new_issue->priority_id,
	                  &priority_name_encoded,
	                  &subject_encoded,
	                  &description_encoded,
	                  &project_name_regex_encoded);

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

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	nxs_string_init(&project_name);
	nxs_string_init(&priority_name);
	nxs_string_init(&subject);
	nxs_string_init(&description);
	nxs_string_init(&project_name_regex);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_project_id,		&new_issue->project_id,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_project_name,		&project_name,			NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_priority_id,		&new_issue->priority_id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_priority_name,		&priority_name,			NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_subject,		&subject,			NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_description,		&description,			NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_project_name_regex,	&project_name_regex,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

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
