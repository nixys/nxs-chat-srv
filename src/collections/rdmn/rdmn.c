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

typedef struct
{
	nxs_chat_srv_m_rdmn_issues_visibility_t		issues_visibility;
	nxs_string_t					name;
} nxs_chat_srv_c_rdmn_issues_visibilities_t;

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

static nxs_cfg_json_state_t nxs_chat_srv_c_rdmn_extract_json_details(nxs_process_t *     proc,
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
static nxs_cfg_json_state_t nxs_chat_srv_c_rdmn_extract_json_custom_fields(nxs_process_t *     proc,
                                                                           nxs_json_t *        json,
                                                                           nxs_cfg_json_par_t *cfg_json_par_el,
                                                                           nxs_array_t *       cfg_arr);
static nxs_cfg_json_state_t nxs_chat_srv_c_rdmn_extract_json_members(nxs_process_t *     proc,
                                                                     nxs_json_t *        json,
                                                                     nxs_cfg_json_par_t *cfg_json_par_el,
                                                                     nxs_array_t *       cfg_arr);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_access(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_chat_srv_c_rdmn_extract_json_roles(nxs_process_t *     proc,
                                                                   nxs_json_t *        json,
                                                                   nxs_cfg_json_par_t *cfg_json_par_el,
                                                                   nxs_array_t *       cfg_arr);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_permissions(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_attachment(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_chat_srv_c_rdmn_extract_json_attachments(nxs_process_t *     proc,
                                                                         nxs_json_t *        json,
                                                                         nxs_cfg_json_par_t *cfg_json_par_el,
                                                                         nxs_array_t *       cfg_arr);

// clang-format off

/* Module initializations */

nxs_chat_srv_c_rdmn_issues_visibilities_t issues_visibilities[] =
{
	{NXS_CHAT_SRV_M_RDMN_ISSUES_VISIBILITY_ALL,		nxs_string("all")},
	{NXS_CHAT_SRV_M_RDMN_ISSUES_VISIBILITY_DEFAULT,		nxs_string("default")},
	{NXS_CHAT_SRV_M_RDMN_ISSUES_VISIBILITY_OWN,		nxs_string("own")},

	{NXS_CHAT_SRV_M_RDMN_ISSUES_VISIBILITY_NONE,		NXS_STRING_NULL_STR},
};

static nxs_string_t	_s_par_action			= nxs_string("action");
static nxs_string_t	_s_par_data			= nxs_string("data");
static nxs_string_t	_s_par_issue			= nxs_string("issue");
static nxs_string_t	_s_par_id			= nxs_string("id");
static nxs_string_t	_s_par_project			= nxs_string("project");
static nxs_string_t	_s_par_name			= nxs_string("name");
static nxs_string_t	_s_par_tracker			= nxs_string("tracker");
static nxs_string_t	_s_par_status			= nxs_string("status");
static nxs_string_t	_s_par_priority			= nxs_string("priority");
static nxs_string_t	_s_par_author			= nxs_string("author");
static nxs_string_t	_s_par_subject			= nxs_string("subject");
static nxs_string_t	_s_par_description		= nxs_string("description");
static nxs_string_t	_s_par_done_ratio		= nxs_string("done_ratio");
static nxs_string_t	_s_par_is_private		= nxs_string("is_private");
static nxs_string_t	_s_par_spent_hours		= nxs_string("spent_hours");
static nxs_string_t	_s_par_watchers			= nxs_string("watchers");
static nxs_string_t	_s_par_journals			= nxs_string("journals");
static nxs_string_t	_s_par_user			= nxs_string("user");
static nxs_string_t	_s_par_notes			= nxs_string("notes");
static nxs_string_t	_s_par_assigned_to		= nxs_string("assigned_to");
static nxs_string_t	_s_par_details			= nxs_string("details");
static nxs_string_t	_s_par_property			= nxs_string("property");
static nxs_string_t	_s_par_attr			= nxs_string("attr");
static nxs_string_t	_s_par_attachment		= nxs_string("attachment");
static nxs_string_t	_s_par_attachments		= nxs_string("attachments");
static nxs_string_t	_s_par_private_notes		= nxs_string("private_notes");
static nxs_string_t	_s_par_custom_fields		= nxs_string("custom_fields");
static nxs_string_t	_s_par_value			= nxs_string("value");
static nxs_string_t	_s_par_members			= nxs_string("members");
static nxs_string_t	_s_par_access			= nxs_string("access");
static nxs_string_t	_s_par_roles			= nxs_string("roles");
static nxs_string_t	_s_par_view_current_issue	= nxs_string("view_current_issue");
static nxs_string_t	_s_par_view_private_notes	= nxs_string("view_private_notes");
static nxs_string_t	_s_par_permissions		= nxs_string("permissions");
static nxs_string_t	_s_par_issues_visibility	= nxs_string("issues_visibility");
static nxs_string_t	_s_par_filename			= nxs_string("filename");
static nxs_string_t	_s_par_filesize			= nxs_string("filesize");
static nxs_string_t	_s_par_content_type		= nxs_string("content_type");
static nxs_string_t	_s_par_content_url		= nxs_string("content_url");
static nxs_string_t	_s_par_created_on		= nxs_string("created_on");

/* Module global functions */

// clang-format on

void nxs_chat_srv_c_rdmn_access_init(nxs_chat_srv_m_rdmn_access_t *access)
{

	if(access == NULL) {

		return;
	}

	access->_is_used = NXS_NO;

	access->view_current_issue = NXS_NO;
	access->view_private_notes = NXS_NO;
}

void nxs_chat_srv_c_rdmn_access_free(nxs_chat_srv_m_rdmn_access_t *access)
{

	if(access == NULL) {

		return;
	}

	access->_is_used = NXS_NO;

	access->view_current_issue = NXS_NO;
	access->view_private_notes = NXS_NO;
}

void nxs_chat_srv_c_rdmn_permissions_init(nxs_chat_srv_m_rdmn_permissions_t *permissions)
{

	if(permissions == NULL) {

		return;
	}

	permissions->_is_used = NXS_NO;

	permissions->view_private_notes = NXS_NO;
	permissions->issues_visibility  = NXS_CHAT_SRV_M_RDMN_ISSUES_VISIBILITY_NONE;
}

void nxs_chat_srv_c_rdmn_permissions_free(nxs_chat_srv_m_rdmn_permissions_t *permissions)
{

	if(permissions == NULL) {

		return;
	}

	permissions->_is_used = NXS_NO;

	permissions->view_private_notes = NXS_NO;
	permissions->issues_visibility  = NXS_CHAT_SRV_M_RDMN_ISSUES_VISIBILITY_NONE;
}

void nxs_chat_srv_c_rdmn_role_init(nxs_chat_srv_m_rdmn_role_t *role)
{

	if(role == NULL) {

		return;
	}

	role->_is_used = NXS_NO;

	role->id = 0;

	nxs_string_init_empty(&role->name);

	nxs_chat_srv_c_rdmn_permissions_init(&role->permissions);
}

void nxs_chat_srv_c_rdmn_role_free(nxs_chat_srv_m_rdmn_role_t *role)
{

	if(role == NULL) {

		return;
	}

	role->_is_used = NXS_NO;

	role->id = 0;

	nxs_string_free(&role->name);

	nxs_chat_srv_c_rdmn_permissions_free(&role->permissions);
}

void nxs_chat_srv_c_rdmn_member_init(nxs_chat_srv_m_rdmn_member_t *member)
{

	if(member == NULL) {

		return;
	}

	member->_is_used = NXS_NO;

	member->id = 0;

	nxs_string_init_empty(&member->name);

	nxs_chat_srv_c_rdmn_access_init(&member->access);

	nxs_array_init2(&member->roles, nxs_chat_srv_m_rdmn_role_t);
}

void nxs_chat_srv_c_rdmn_member_free(nxs_chat_srv_m_rdmn_member_t *member)
{
	nxs_chat_srv_m_rdmn_role_t *r;
	size_t                      i;

	if(member == NULL) {

		return;
	}

	member->_is_used = NXS_NO;

	member->id = 0;

	nxs_string_free(&member->name);

	nxs_chat_srv_c_rdmn_access_free(&member->access);

	for(i = 0; i < nxs_array_count(&member->roles); i++) {

		r = nxs_array_get(&member->roles, i);

		nxs_chat_srv_c_rdmn_role_free(r);
	}

	nxs_array_free(&member->roles);
}

void nxs_chat_srv_c_rdmn_project_init(nxs_chat_srv_m_rdmn_project_t *project)
{

	if(project == NULL) {

		return;
	}

	project->_is_used = NXS_NO;

	project->id = 0;

	nxs_string_init_empty(&project->name);

	nxs_array_init2(&project->members, nxs_chat_srv_m_rdmn_member_t);
}

void nxs_chat_srv_c_rdmn_project_free(nxs_chat_srv_m_rdmn_project_t *project)
{
	nxs_chat_srv_m_rdmn_member_t *m;
	size_t                        i;

	if(project == NULL) {

		return;
	}

	project->_is_used = NXS_NO;

	project->id = 0;

	nxs_string_free(&project->name);

	for(i = 0; i < nxs_array_count(&project->members); i++) {

		m = nxs_array_get(&project->members, i);

		nxs_chat_srv_c_rdmn_member_free(m);
	}

	nxs_array_free(&project->members);
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

void nxs_chat_srv_c_rdmn_detail_init(nxs_chat_srv_m_rdmn_detail_t *detail)
{

	if(detail == NULL) {

		return;
	}

	detail->_is_used            = NXS_NO;
	detail->attr._is_used       = NXS_NO;
	detail->attachment._is_used = NXS_NO;

	detail->type = NXS_CHAT_SRV_M_RDMN_DETAIL_TYPE_NONE;

	detail->attachment.name = 0;

	nxs_string_init_empty(&detail->attr.name);
}

void nxs_chat_srv_c_rdmn_detail_free(nxs_chat_srv_m_rdmn_detail_t *detail)
{

	if(detail == NULL) {

		return;
	}

	detail->_is_used            = NXS_NO;
	detail->attr._is_used       = NXS_NO;
	detail->attachment._is_used = NXS_NO;

	detail->type = NXS_CHAT_SRV_M_RDMN_DETAIL_TYPE_NONE;

	detail->attachment.name = 0;

	nxs_string_free(&detail->attr.name);
}

void nxs_chat_srv_c_rdmn_journal_init(nxs_chat_srv_m_rdmn_journal_t *journal)
{

	if(journal == NULL) {

		return;
	}

	journal->_is_used = NXS_NO;

	journal->id            = 0;
	journal->private_notes = NXS_NO;

	nxs_string_init_empty(&journal->notes);

	nxs_chat_srv_c_rdmn_user_init(&journal->user);

	nxs_array_init2(&journal->details, nxs_chat_srv_m_rdmn_detail_t);
}

void nxs_chat_srv_c_rdmn_journal_free(nxs_chat_srv_m_rdmn_journal_t *journal)
{
	nxs_chat_srv_m_rdmn_detail_t *d;
	size_t                        i;

	if(journal == NULL) {

		return;
	}

	journal->_is_used = NXS_NO;

	journal->id            = 0;
	journal->private_notes = NXS_NO;

	nxs_string_free(&journal->notes);

	nxs_chat_srv_c_rdmn_user_free(&journal->user);

	for(i = 0; i < nxs_array_count(&journal->details); i++) {

		d = nxs_array_get(&journal->details, i);

		nxs_chat_srv_c_rdmn_detail_free(d);
	}

	nxs_array_free(&journal->details);
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
	nxs_array_init2(&issue->cf_watchers, size_t);
	nxs_array_init2(&issue->attachments, nxs_chat_srv_m_rdmn_attachment_t);

	nxs_chat_srv_c_rdmn_project_init(&issue->project);
	nxs_chat_srv_c_rdmn_tracker_init(&issue->tracker);
	nxs_chat_srv_c_rdmn_status_init(&issue->status);
	nxs_chat_srv_c_rdmn_priority_init(&issue->priority);
	nxs_chat_srv_c_rdmn_user_init(&issue->author);
	nxs_chat_srv_c_rdmn_user_init(&issue->assigned_to);
}

void nxs_chat_srv_c_rdmn_issue_free(nxs_chat_srv_m_rdmn_issue_t *issue)
{
	nxs_chat_srv_m_rdmn_journal_t *   j;
	nxs_chat_srv_m_rdmn_user_t *      u;
	nxs_chat_srv_m_rdmn_attachment_t *a;
	size_t                            i;

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

	for(i = 0; i < nxs_array_count(&issue->attachments); i++) {

		a = nxs_array_get(&issue->attachments, i);

		nxs_chat_srv_c_rdmn_attachment_free(a);
	}

	nxs_array_free(&issue->journals);
	nxs_array_free(&issue->watchers);
	nxs_array_free(&issue->cf_watchers);
	nxs_array_free(&issue->attachments);

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

void nxs_chat_srv_c_rdmn_attachment_init(nxs_chat_srv_m_rdmn_attachment_t *attachment)
{

	if(attachment == NULL) {

		return;
	}

	attachment->_is_used = NXS_NO;

	attachment->id       = 0;
	attachment->filesize = 0;

	nxs_string_init_empty(&attachment->filename);
	nxs_string_init_empty(&attachment->content_type);
	nxs_string_init_empty(&attachment->description);
	nxs_string_init_empty(&attachment->content_url);
	nxs_string_init_empty(&attachment->created_on);

	nxs_chat_srv_c_rdmn_user_init(&attachment->author);
}

void nxs_chat_srv_c_rdmn_attachment_free(nxs_chat_srv_m_rdmn_attachment_t *attachment)
{

	if(attachment == NULL) {

		return;
	}

	attachment->_is_used = NXS_NO;

	attachment->id       = 0;
	attachment->filesize = 0;

	nxs_string_free(&attachment->filename);
	nxs_string_free(&attachment->content_type);
	nxs_string_free(&attachment->description);
	nxs_string_free(&attachment->content_url);
	nxs_string_free(&attachment->created_on);

	nxs_chat_srv_c_rdmn_user_free(&attachment->author);
}

nxs_chat_srv_err_t nxs_chat_srv_c_rdmn_attachment_pull_json(nxs_chat_srv_m_rdmn_attachment_t *attachment, nxs_buf_t *json_buf)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	if(attachment == NULL || json_buf == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_attachment,	attachment,		&nxs_chat_srv_c_rdmn_extract_json_attachment,		NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, json_buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: rdmn attachment json extract error: parse data error", nxs_proc_get_name(&process));

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

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&var->id,		NULL,						NULL,							NXS_CFG_JSON_TYPE_INT,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_subject,	&var->subject,		NULL,						NULL,							NXS_CFG_JSON_TYPE_STRING,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_description,	&var->description,	NULL,						NULL,							NXS_CFG_JSON_TYPE_STRING,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_done_ratio,	&var->done_ratio,	NULL,						NULL,							NXS_CFG_JSON_TYPE_INT,			0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_is_private,	&var->is_private,	NULL,						NULL,							NXS_CFG_JSON_TYPE_BOOL,			0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_spent_hours,	&var->spent_hours,	NULL,						NULL,							NXS_CFG_JSON_TYPE_REAL,			0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_journals,	&var->journals,		NULL,						&nxs_chat_srv_c_rdmn_extract_json_journals,		NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_watchers,	&var->watchers,		NULL,						&nxs_chat_srv_c_rdmn_extract_json_users,		NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_project,	&var->project,		&nxs_chat_srv_c_rdmn_extract_json_project,	NULL,							NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_tracker,	&var->tracker,		&nxs_chat_srv_c_rdmn_extract_json_tracker,	NULL,							NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_status,		&var->status,		&nxs_chat_srv_c_rdmn_extract_json_status,	NULL,							NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_priority,	&var->priority,		&nxs_chat_srv_c_rdmn_extract_json_priority,	NULL,							NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_author,		&var->author,		&nxs_chat_srv_c_rdmn_extract_json_user,		NULL,							NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_assigned_to,	&var->assigned_to,	&nxs_chat_srv_c_rdmn_extract_json_user,		NULL,							NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_custom_fields,	var,			NULL,						&nxs_chat_srv_c_rdmn_extract_json_custom_fields,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_attachments,	&var->attachments,	NULL,						&nxs_chat_srv_c_rdmn_extract_json_attachments,		NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_NO,		NULL);

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
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_name,		&u->name,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);

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
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_name,		&var->name,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);

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

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_id,		&j->id,			NULL,					NULL,						NXS_CFG_JSON_TYPE_INT,			0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_notes,		&j->notes,		NULL,					NULL,						NXS_CFG_JSON_TYPE_STRING,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_private_notes,	&j->private_notes,	NULL,					NULL,						NXS_CFG_JSON_TYPE_BOOL,			0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_user,		&j->user,		&nxs_chat_srv_c_rdmn_extract_json_user,	NULL,						NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_details,	&j->details,		NULL,					&nxs_chat_srv_c_rdmn_extract_json_details,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_NO,		NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t nxs_chat_srv_c_rdmn_extract_json_details(nxs_process_t *     proc,
                                                                     nxs_json_t *        json,
                                                                     nxs_cfg_json_par_t *cfg_json_par_el,
                                                                     nxs_array_t *       cfg_arr)
{
	nxs_array_t *                 details = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_m_rdmn_detail_t *d;
	nxs_json_t *                  j;
	nxs_string_t *                p;

	if((j = nxs_json_child_get_by_key(json, nxs_string_str(&_s_par_property))) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: rdmn json read error: parse rdmn details error, missing filed \"%r\"",
		                    nxs_proc_get_name(&process),
		                    &_s_par_property);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(nxs_json_type_get(j) != NXS_JSON_TYPE_STRING) {

		nxs_log_write_error(&process,
		                    "[%s]: rdmn json read error: parse rdmn details error, expected string type for field \"%r\"",
		                    nxs_proc_get_name(&process),
		                    &_s_par_property);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	p = nxs_json_string_val(j);

	if(nxs_string_cmp(p, 0, &_s_par_attr, 0) == NXS_YES) {

		d = nxs_array_add(details);

		nxs_chat_srv_c_rdmn_detail_init(d);

		d->_is_used      = NXS_YES;
		d->attr._is_used = NXS_YES;

		d->type = NXS_CHAT_SRV_M_RDMN_DETAIL_TYPE_ATTR;
	}
	else {

		if(nxs_string_cmp(p, 0, &_s_par_attachment, 0) == NXS_YES) {

			d = nxs_array_add(details);

			nxs_chat_srv_c_rdmn_detail_init(d);

			d->_is_used            = NXS_YES;
			d->attachment._is_used = NXS_YES;

			d->type = NXS_CHAT_SRV_M_RDMN_DETAIL_TYPE_ATTACHMENT;
		}
		else {

			return NXS_CFG_JSON_CONF_OK;
		}
	}

	if((j = nxs_json_child_get_by_key(json, nxs_string_str(&_s_par_name))) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: rdmn json read error: parse rdmn details error, missing filed \"%r\"",
		                    nxs_proc_get_name(&process),
		                    &_s_par_name);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	switch(d->type) {

		case NXS_CHAT_SRV_M_RDMN_DETAIL_TYPE_ATTR:

			if(nxs_json_type_get(j) != NXS_JSON_TYPE_STRING) {

				nxs_log_write_error(
				        &process,
				        "[%s]: rdmn json read error: parse rdmn details error, expected string type for filed \"%r\"",
				        nxs_proc_get_name(&process),
				        &_s_par_name);

				return NXS_CFG_JSON_CONF_ERROR;
			}

			nxs_string_clone(&d->attr.name, nxs_json_string_val(j));

			break;

		case NXS_CHAT_SRV_M_RDMN_DETAIL_TYPE_ATTACHMENT:

			/*
			 * This field has different types for different Redmine versions
			 */
			switch(nxs_json_type_get(j)) {

				case NXS_JSON_TYPE_INTEGER:

					d->attachment.name = (size_t)nxs_json_integer_val(j);

					break;

				case NXS_JSON_TYPE_STRING:

					d->attachment.name = (size_t)nxs_string_atoi(nxs_json_string_val(j));

					break;

				default:

					nxs_log_write_error(
					        &process,
					        "[%s]: rdmn json read error: parse rdmn details error, expected integer or string types "
					        "for filed \"%r\"",
					        nxs_proc_get_name(&process),
					        &_s_par_name);

					return NXS_CFG_JSON_CONF_ERROR;
			}

			break;

		default:

			break;
	}

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
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_name,		&var->name,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);

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
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_name,		&var->name,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);

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
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_name,		&var->name,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);

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

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&var->id,		NULL,	NULL,						NXS_CFG_JSON_TYPE_INT,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_name,		&var->name,		NULL,	NULL,						NXS_CFG_JSON_TYPE_STRING,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_members,	&var->members,		NULL,	&nxs_chat_srv_c_rdmn_extract_json_members,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_NO,		NULL);

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

static nxs_cfg_json_state_t nxs_chat_srv_c_rdmn_extract_json_custom_fields(nxs_process_t *     proc,
                                                                           nxs_json_t *        json,
                                                                           nxs_cfg_json_par_t *cfg_json_par_el,
                                                                           nxs_array_t *       cfg_arr)
{
	nxs_chat_srv_m_rdmn_issue_t *issue = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_json_t *                 j;
	nxs_cfg_json_t               cfg_json_cf;
	nxs_array_t                  cfg_arr_cf, ids;
	nxs_cfg_json_state_t         rc;
	nxs_string_t *               s;
	size_t                       i, *id;

	/* search and check 'id' field */
	if((j = nxs_json_child_get_by_key(json, nxs_string_str(&_s_par_id))) == NULL) {

		nxs_log_write_error(&process,
		                    "[%s]: rdmn json read error: parse rdmn issue custom fields error, missing field \"%r\"",
		                    nxs_proc_get_name(&process),
		                    &_s_par_id);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(nxs_json_type_get(j) != NXS_JSON_TYPE_INTEGER) {

		nxs_log_write_error(
		        &process,
		        "[%s]: rdmn json read error: parse rdmn issue custom fields error, expected integer type for field \"%r\"",
		        nxs_proc_get_name(&process),
		        &_s_par_id);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	/* if it is not cf_watchers custom field */
	if((size_t)nxs_json_integer_val(j) != nxs_chat_srv_cfg.rdmn.cf_watchers) {

		return NXS_CFG_JSON_CONF_OK;
	}

	/* read values for cf_watchers */

	rc = NXS_CFG_JSON_CONF_OK;

	nxs_array_init2(&ids, nxs_string_t);

	nxs_cfg_json_conf_array_init(&cfg_arr_cf);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr_cf);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr_cf,	&_s_par_value,	&ids,	NULL,	NULL,	NXS_CFG_JSON_TYPE_ARRAY_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json_cf, NULL, NULL, NULL, &cfg_arr_cf);

	if(nxs_cfg_json_read_json(&process, cfg_json_cf, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(
		        &process, "[%s]: rdmn json read error: parse rdmn issue custom fields error", nxs_proc_get_name(&process));

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

	for(i = 0; i < nxs_array_count(&ids); i++) {

		s = nxs_array_get(&ids, i);

		id = nxs_array_add(&issue->cf_watchers);

		*id = nxs_string_atoi(s);
	}

error:

	for(i = 0; i < nxs_array_count(&ids); i++) {

		s = nxs_array_get(&ids, i);

		nxs_string_free(s);
	}

	nxs_array_free(&ids);

	nxs_cfg_json_free(&cfg_json_cf);

	nxs_cfg_json_conf_array_free(&cfg_arr_cf);

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_c_rdmn_extract_json_members(nxs_process_t *     proc,
                                                                     nxs_json_t *        json,
                                                                     nxs_cfg_json_par_t *cfg_json_par_el,
                                                                     nxs_array_t *       cfg_arr)
{
	nxs_array_t *                 members = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_m_rdmn_member_t *m;

	m = nxs_array_add(members);

	nxs_chat_srv_c_rdmn_member_init(m);

	m->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_skip_undef(cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_id,		&m->id,			NULL,						NULL,						NXS_CFG_JSON_TYPE_INT,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_name,		&m->name,		NULL,						NULL,						NXS_CFG_JSON_TYPE_STRING,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_access,		&m->access,		&nxs_chat_srv_c_rdmn_extract_json_access,	NULL,						NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_roles,		&m->roles,		NULL,						&nxs_chat_srv_c_rdmn_extract_json_roles,	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_access(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_rdmn_access_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                cfg_json;
	nxs_array_t                   cfg_arr;
	nxs_cfg_json_state_t          rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_view_current_issue,	&var->view_current_issue,	NULL,	NULL,	NXS_CFG_JSON_TYPE_BOOL,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_view_private_notes,	&var->view_private_notes,	NULL,	NULL,	NXS_CFG_JSON_TYPE_BOOL,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn json read error: 'access' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_c_rdmn_extract_json_roles(nxs_process_t *     proc,
                                                                   nxs_json_t *        json,
                                                                   nxs_cfg_json_par_t *cfg_json_par_el,
                                                                   nxs_array_t *       cfg_arr)
{
	nxs_array_t *               roles = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_m_rdmn_role_t *r;

	r = nxs_array_add(roles);

	nxs_chat_srv_c_rdmn_role_init(r);

	r->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_skip_undef(cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_id,		&r->id,			NULL,						NULL,						NXS_CFG_JSON_TYPE_INT,			0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_name,		&r->name,		NULL,						NULL,						NXS_CFG_JSON_TYPE_STRING,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_permissions,	&r->permissions,	&nxs_chat_srv_c_rdmn_extract_json_permissions,	NULL,						NXS_CFG_JSON_TYPE_VOID,			0,	0,	NXS_YES,	NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_permissions(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_rdmn_permissions_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                     cfg_json;
	nxs_array_t                        cfg_arr;
	nxs_string_t                       issues_visibility;
	nxs_cfg_json_state_t               rc;
	size_t                             i;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_string_init(&issues_visibility);

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_issues_visibility,	&issues_visibility,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_view_private_notes,	&var->view_private_notes,	NULL,	NULL,	NXS_CFG_JSON_TYPE_BOOL,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn json read error: 'permissions' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

	for(i = 0; issues_visibilities[i].issues_visibility != NXS_CHAT_SRV_M_RDMN_ISSUES_VISIBILITY_NONE; i++) {

		if(nxs_string_cmp(&issues_visibility, 0, &issues_visibilities[i].name, 0) == NXS_YES) {

			var->issues_visibility = issues_visibilities[i].issues_visibility;
		}
	}

error:

	nxs_string_free(&issues_visibility);

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t
        nxs_chat_srv_c_rdmn_extract_json_attachment(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_chat_srv_m_rdmn_attachment_t *var = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_cfg_json_t                    cfg_json;
	nxs_array_t                       cfg_arr;
	nxs_cfg_json_state_t              rc;

	rc = NXS_CFG_JSON_CONF_OK;

	var->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_id,		&var->id,		NULL,						NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_filename,	&var->filename,		NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_filesize,	&var->filesize,		NULL,						NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_content_type,	&var->content_type,	NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_description,	&var->description,	NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_content_url,	&var->content_url,	NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_created_on,	&var->created_on,	NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_author,		&var->author,		&nxs_chat_srv_c_rdmn_extract_json_user,		NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_NO,		NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_json(&process, cfg_json, json) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rdmn json read error: 'attachment' block");

		nxs_error(rc, NXS_CFG_JSON_CONF_ERROR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static nxs_cfg_json_state_t nxs_chat_srv_c_rdmn_extract_json_attachments(nxs_process_t *     proc,
                                                                         nxs_json_t *        json,
                                                                         nxs_cfg_json_par_t *cfg_json_par_el,
                                                                         nxs_array_t *       cfg_arr)
{
	nxs_array_t *                     attachments = nxs_cfg_json_get_val(cfg_json_par_el);
	nxs_chat_srv_m_rdmn_attachment_t *a;

	a = nxs_array_add(attachments);

	nxs_chat_srv_c_rdmn_attachment_init(a);

	a->_is_used = NXS_YES;

	nxs_cfg_json_conf_array_skip_undef(cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_id,		&a->id,			NULL,						NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_filename,	&a->filename,		NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_filesize,	&a->filesize,		NULL,						NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_content_type,	&a->content_type,	NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_description,	&a->description,	NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_content_url,	&a->content_url,	NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_created_on,	&a->created_on,		NULL,						NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_NO,		NULL);
	nxs_cfg_json_conf_array_add(cfg_arr,	&_s_par_author,		&a->author,		&nxs_chat_srv_c_rdmn_extract_json_user,		NULL,	NXS_CFG_JSON_TYPE_VOID,		0,	0,	NXS_NO,		NULL);

	// clang-format on

	return NXS_CFG_JSON_CONF_OK;
}
