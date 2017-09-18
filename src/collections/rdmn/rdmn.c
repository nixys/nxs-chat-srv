#ifndef _INCLUDE_NXS_CHAT_SRV_C_RDMN_C
#define _INCLUDE_NXS_CHAT_SRV_C_RDMN_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_data(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_issue(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_chat_srv_c_rdmn_extract_json_users(nxs_process_t *     proc,
                                                                   nxs_json_t *        json,
                                                                   nxs_cfg_json_par_t *cfg_json_par_el,
                                                                   nxs_array_t *       cfg_arr);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_chat_srv_c_rdmn_extract_json_journals(nxs_process_t *     proc,
                                                                      nxs_json_t *        json,
                                                                      nxs_cfg_json_par_t *cfg_json_par_el,
                                                                      nxs_array_t *       cfg_arr);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_priority(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_status(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_tracker(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_project(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

// clang-format off

/* Module initializations */

static nxs_string_t	_s_par_action		= nxs_string("action");
static nxs_string_t	_s_par_data		= nxs_string("data");
static nxs_string_t	_s_par_issue		= nxs_string("issue");
static nxs_string_t	_s_par_id		= nxs_string("id");
static nxs_string_t	_s_par_project		= nxs_string("project");
static nxs_string_t	_s_par_name		= nxs_string("name");
static nxs_string_t	_s_par_tracker		= nxs_string("tracker");
static nxs_string_t	_s_par_status		= nxs_string("status");
static nxs_string_t	_s_par_priority		= nxs_string("priority");
static nxs_string_t	_s_par_author		= nxs_string("author");
static nxs_string_t	_s_par_subject		= nxs_string("subject");
static nxs_string_t	_s_par_description	= nxs_string("description");
static nxs_string_t	_s_par_done_ratio	= nxs_string("done_ratio");
static nxs_string_t	_s_par_is_private	= nxs_string("is_private");
static nxs_string_t	_s_par_spent_hours	= nxs_string("spent_hours");
static nxs_string_t	_s_par_watchers		= nxs_string("watchers");
static nxs_string_t	_s_par_journals		= nxs_string("journals");
static nxs_string_t	_s_par_user		= nxs_string("user");
static nxs_string_t	_s_par_notes		= nxs_string("notes");
static nxs_string_t	_s_par_assigned_to	= nxs_string("assigned_to");

/* Module global functions */

// clang-format on

void nxs_chat_srv_c_rdmn_project_init(nxs_chat_srv_m_rdmn_project_t *project)
{

	if(project == NULL) {

		return;
	}

	project->_is_used = NXS_NO;

	project->id = 0;

	nxs_string_init_empty(&project->name);
}

void nxs_chat_srv_c_rdmn_project_free(nxs_chat_srv_m_rdmn_project_t *project)
{

	if(project == NULL) {

		return;
	}

	project->_is_used = NXS_NO;

	project->id = 0;

	nxs_string_free(&project->name);
}

void nxs_chat_srv_c_rdmn_tracker_init(nxs_chat_srv_m_rdmn_tracker_t *tracker)
{

	if(tracker == NULL) {

		return;
	}

	tracker->_is_used = NXS_NO;

	tracker->id = 0;

	nxs_string_init_empty(&tracker->name);
}

void nxs_chat_srv_c_rdmn_tracker_free(nxs_chat_srv_m_rdmn_tracker_t *tracker)
{

	if(tracker == NULL) {

		return;
	}

	tracker->_is_used = NXS_NO;

	tracker->id = 0;

	nxs_string_free(&tracker->name);
}

void nxs_chat_srv_c_rdmn_status_init(nxs_chat_srv_m_rdmn_status_t *status)
{

	if(status == NULL) {

		return;
	}

	status->_is_used = NXS_NO;

	status->id = 0;

	nxs_string_init_empty(&status->name);
}

void nxs_chat_srv_c_rdmn_status_free(nxs_chat_srv_m_rdmn_status_t *status)
{

	if(status == NULL) {

		return;
	}

	status->_is_used = NXS_NO;

	status->id = 0;

	nxs_string_free(&status->name);
}

void nxs_chat_srv_c_rdmn_priority_init(nxs_chat_srv_m_rdmn_priority_t *priority)
{

	if(priority == NULL) {

		return;
	}

	priority->_is_used = NXS_NO;

	priority->id = 0;

	nxs_string_init_empty(&priority->name);
}

void nxs_chat_srv_c_rdmn_priority_free(nxs_chat_srv_m_rdmn_priority_t *priority)
{

	if(priority == NULL) {

		return;
	}

	priority->_is_used = NXS_NO;

	priority->id = 0;

	nxs_string_free(&priority->name);
}

void nxs_chat_srv_c_rdmn_user_init(nxs_chat_srv_m_rdmn_user_t *user)
{

	if(user == NULL) {

		return;
	}

	user->_is_used = NXS_NO;

	user->id = 0;

	nxs_string_init_empty(&user->name);
}

void nxs_chat_srv_c_rdmn_user_free(nxs_chat_srv_m_rdmn_user_t *user)
{

	if(user == NULL) {

		return;
	}

	user->_is_used = NXS_NO;

	user->id = 0;

	nxs_string_free(&user->name);
}

void nxs_chat_srv_c_rdmn_journal_init(nxs_chat_srv_m_rdmn_journal_t *journal)
{

	if(journal == NULL) {

		return;
	}

	journal->_is_used = NXS_NO;

	journal->id = 0;

	nxs_string_init_empty(&journal->notes);

	nxs_chat_srv_c_rdmn_user_init(&journal->user);
}

void nxs_chat_srv_c_rdmn_journal_free(nxs_chat_srv_m_rdmn_journal_t *journal)
{

	if(journal == NULL) {

		return;
	}

	journal->_is_used = NXS_NO;

	journal->id = 0;

	nxs_string_free(&journal->notes);

	nxs_chat_srv_c_rdmn_user_free(&journal->user);
}

void nxs_chat_srv_c_rdmn_issue_init(nxs_chat_srv_m_rdmn_issue_t *issue)
{

	if(issue == NULL) {

		return;
	}

	issue->_is_used = NXS_NO;

	issue->id          = 0;
	issue->is_private  = NXS_NO;
	issue->done_ratio  = 0;
	issue->spent_hours = 0.0;

	nxs_string_init_empty(&issue->subject);
	nxs_string_init_empty(&issue->description);

	nxs_array_init2(&issue->journals, nxs_chat_srv_m_rdmn_issue_t);
	nxs_array_init2(&issue->watchers, nxs_chat_srv_m_rdmn_user_t);

	nxs_chat_srv_c_rdmn_project_init(&issue->project);
	nxs_chat_srv_c_rdmn_tracker_init(&issue->tracker);
	nxs_chat_srv_c_rdmn_status_init(&issue->status);
	nxs_chat_srv_c_rdmn_priority_init(&issue->priority);
	nxs_chat_srv_c_rdmn_user_init(&issue->author);
	nxs_chat_srv_c_rdmn_user_init(&issue->assigned_to);
}

void nxs_chat_srv_c_rdmn_issue_free(nxs_chat_srv_m_rdmn_issue_t *issue)
{
	nxs_chat_srv_m_rdmn_journal_t *j;
	nxs_chat_srv_m_rdmn_user_t *   u;
	size_t                         i;

	if(issue == NULL) {

		return;
	}

	issue->_is_used = NXS_NO;

	issue->id          = 0;
	issue->is_private  = NXS_NO;
	issue->done_ratio  = 0;
	issue->spent_hours = 0.0;

	nxs_string_free(&issue->subject);
	nxs_string_free(&issue->description);

	for(i = 0; i < nxs_array_count(&issue->journals); i++) {

		j = nxs_array_get(&issue->journals, i);

		nxs_chat_srv_c_rdmn_journal_free(j);
	}

	for(i = 0; i < nxs_array_count(&issue->watchers); i++) {

		u = nxs_array_get(&issue->watchers, i);

		nxs_chat_srv_c_rdmn_user_free(u);
	}

	nxs_array_free(&issue->journals);
	nxs_array_free(&issue->watchers);

	nxs_chat_srv_c_rdmn_project_free(&issue->project);
	nxs_chat_srv_c_rdmn_tracker_free(&issue->tracker);
	nxs_chat_srv_c_rdmn_status_free(&issue->status);
	nxs_chat_srv_c_rdmn_priority_free(&issue->priority);
	nxs_chat_srv_c_rdmn_user_free(&issue->author);
	nxs_chat_srv_c_rdmn_user_free(&issue->assigned_to);
}

void nxs_chat_srv_c_rdmn_data_init(nxs_chat_srv_m_rdmn_data_t *data)
{

	if(data == NULL) {

		return;
	}

	data->_is_used = NXS_NO;

	nxs_chat_srv_c_rdmn_issue_init(&data->issue);
}

void nxs_chat_srv_c_rdmn_data_free(nxs_chat_srv_m_rdmn_data_t *data)
{

	if(data == NULL) {

		return;
	}

	data->_is_used = NXS_NO;

	nxs_chat_srv_c_rdmn_issue_free(&data->issue);
}

void nxs_chat_srv_c_rdmn_update_init(nxs_chat_srv_m_rdmn_update_t *update)
{

	if(update == NULL) {

		return;
	}

	nxs_string_init_empty(&update->action);

	nxs_chat_srv_c_rdmn_data_init(&update->data);
}

void nxs_chat_srv_c_rdmn_update_free(nxs_chat_srv_m_rdmn_update_t *update)
{

	if(update == NULL) {

		return;
	}

	nxs_string_free(&update->action);

	nxs_chat_srv_c_rdmn_data_free(&update->data);
}

nxs_chat_srv_err_t nxs_chat_srv_c_rdmn_update_pull_json(nxs_chat_srv_m_rdmn_update_t *update, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	if(update == NULL || json_buf == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_action,		&update->action,	NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_data,		&update->data,		&nxs_chat_srv_c_rdmn_extract_json_data,		NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: rdmn update rest api error: parse update data error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

/* Module internal (static) functions */

static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_data(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_rdmn_data_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t              cfg_json;
	nxs_array_t                 cfg_arr;
	nxs_cfg_json_state_t        rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_issue,		&var->issue,		&nxs_chat_srv_c_rdmn_extract_json_issue,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn json read error: 'data' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_issue(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_rdmn_issue_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t               cfg_json;
	nxs_array_t                  cfg_arr;
	nxs_cfg_json_state_t         rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&var->id,		NULL,						NULL,						NXS_CFG_JSON_TYPE_INT,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_subject,	&var->subject,		NULL,						NULL,						NXS_CFG_JSON_TYPE_STRING,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_description,	&var->description,	NULL,						NULL,						NXS_CFG_JSON_TYPE_STRING,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_done_ratio,	&var->done_ratio,	NULL,						NULL,						NXS_CFG_JSON_TYPE_INT,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_is_private,	&var->is_private,	NULL,						NULL,						NXS_CFG_JSON_TYPE_BOOL,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_spent_hours,	&var->spent_hours,	NULL,						NULL,						NXS_CFG_JSON_TYPE_REAL,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_journals,	&var->journals,		NULL,						&nxs_chat_srv_c_rdmn_extract_json_journals,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_watchers,	&var->watchers,		NULL,						&nxs_chat_srv_c_rdmn_extract_json_users,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_project,	&var->project,		&nxs_chat_srv_c_rdmn_extract_json_project,	NULL,						NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_tracker,	&var->tracker,		&nxs_chat_srv_c_rdmn_extract_json_tracker,	NULL,						NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_status,		&var->status,		&nxs_chat_srv_c_rdmn_extract_json_status,	NULL,						NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_priority,	&var->priority,		&nxs_chat_srv_c_rdmn_extract_json_priority,	NULL,						NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_author,		&var->author,		&nxs_chat_srv_c_rdmn_extract_json_user,		NULL,						NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_assigned_to,	&var->assigned_to,	&nxs_chat_srv_c_rdmn_extract_json_user,		NULL,						NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_NO,		NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn json read error: 'issue' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_c_rdmn_extract_json_users(nxs_process_t *     proc,
                                                                   nxs_json_t *        json,
                                                                   nxs_cfg_json_par_t *cfg_json_par_el,
                                                                   nxs_array_t *       cfg_arr)
{
	nxs_array_t *               users = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_m_rdmn_user_t *u;

	u = nxs_array_add(users);

	nxs_chat_srv_c_rdmn_user_init(u);

	u->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_skip_undef(cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_id,		&u->id,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_name,		&u->name,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_user(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_rdmn_user_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t              cfg_json;
	nxs_array_t                 cfg_arr;
	nxs_cfg_json_state_t        rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&var->id,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_name,		&var->name,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn json read error: 'user' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_c_rdmn_extract_json_journals(nxs_process_t *     proc,
                                                                      nxs_json_t *        json,
                                                                      nxs_cfg_json_par_t *cfg_json_par_el,
                                                                      nxs_array_t *       cfg_arr)
{
	nxs_array_t *                  journals = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_m_rdmn_journal_t *j;

	j = nxs_array_add(journals);

	nxs_chat_srv_c_rdmn_journal_init(j);

	j->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_skip_undef(cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_id,		&j->id,		NULL,					NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_notes,		&j->notes,	NULL,					NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_user,		&j->user,	&nxs_chat_srv_c_rdmn_extract_json_user,	NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_priority(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_rdmn_priority_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                  cfg_json;
	nxs_array_t                     cfg_arr;
	nxs_cfg_json_state_t            rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&var->id,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_name,		&var->name,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn json read error: 'priority' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_status(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_rdmn_status_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                cfg_json;
	nxs_array_t                   cfg_arr;
	nxs_cfg_json_state_t          rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&var->id,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_name,		&var->name,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn json read error: 'status' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_tracker(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_rdmn_tracker_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                 cfg_json;
	nxs_array_t                    cfg_arr;
	nxs_cfg_json_state_t           rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&var->id,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_name,		&var->name,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn json read error: 'tracker' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_project(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_rdmn_project_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                 cfg_json;
	nxs_array_t                    cfg_arr;
	nxs_cfg_json_state_t           rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&var->id,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_name,		&var->name,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn json read error: 'project' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}
