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
	size_t						id;			/* session id (auto increment) */
	size_t						tlgrm_userid;		/* session owner (it is not bot) */
	size_t						chat_id;		/* chat id where session was started */
	size_t						message_id;		/* bot message id where session was started */
	time_t						updated_at;		/* session last update */
	nxs_chat_srv_m_db_sess_type_t			type;			/* type of data stored in `data` field */
	nxs_chat_srv_m_db_sess_wait_for_type_t		wait_for;		/* used to help bot to determine type of expected data received from user (e.g. expected reply from user with new issue subject) */
	nxs_string_t					data;			/* session data (json with base64 encripted string fields ) */
} nxs_chat_srv_u_db_sess_el_t;

struct nxs_chat_srv_u_db_sess_s
{
	nxs_list_t					sessions;		/* type: nxs_chat_srv_u_db_sess_el_t */
};

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_chat_srv_u_db_sess_el_t *nxs_chat_srv_u_db_sess_el_get(nxs_chat_srv_u_db_sess_t *u_ctx, size_t sess_id);
static nxs_chat_srv_u_db_sess_el_t *nxs_chat_srv_u_db_sess_el_add(nxs_chat_srv_u_db_sess_t *             u_ctx,
                                                                  size_t                                 id,
                                                                  size_t                                 tlgrm_userid,
                                                                  size_t                                 chat_id,
                                                                  size_t                                 message_id,
                                                                  time_t                                 updated_at,
                                                                  nxs_chat_srv_m_db_sess_type_t          type,
                                                                  nxs_chat_srv_m_db_sess_wait_for_type_t wait_for,
                                                                  nxs_string_t *                         data);
static void nxs_chat_srv_u_db_sess_el_clear(nxs_chat_srv_u_db_sess_el_t *s);

static void nxs_chat_srv_u_db_sess_s_message_init(nxs_chat_srv_u_db_sess_t_message_t *ctx);
static void nxs_chat_srv_u_db_sess_s_message_free(nxs_chat_srv_u_db_sess_t_message_t *ctx);
static void nxs_chat_srv_u_db_sess_s_message_serialize(nxs_chat_srv_u_db_sess_t_message_t *ctx, nxs_string_t *out_str);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_message_deserialize(nxs_chat_srv_u_db_sess_t_message_t *ctx, nxs_string_t *data);

static void nxs_chat_srv_u_db_sess_s_new_issue_init(nxs_chat_srv_u_db_sess_t_new_issue_t *ctx);
static void nxs_chat_srv_u_db_sess_s_new_issue_free(nxs_chat_srv_u_db_sess_t_new_issue_t *ctx);
static void nxs_chat_srv_u_db_sess_s_new_issue_serialize(nxs_chat_srv_u_db_sess_t_new_issue_t *ctx, nxs_string_t *out_str);
static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_new_issue_deserialize(nxs_chat_srv_u_db_sess_t_new_issue_t *ctx, nxs_string_t *data);

// clang-format off

/* Module initializations */

static nxs_string_t _s_empty			= nxs_string("");

static nxs_string_t _s_par_message		= nxs_string("message");
static nxs_string_t _s_par_project_id		= nxs_string("project_id");
static nxs_string_t _s_par_project_name		= nxs_string("project_name");
static nxs_string_t _s_par_priority_id		= nxs_string("priority_id");
static nxs_string_t _s_par_priority_name	= nxs_string("priority_name");
static nxs_string_t _s_par_subject		= nxs_string("subject");
static nxs_string_t _s_par_description		= nxs_string("description");

/* Module global functions */

// clang-format on

nxs_chat_srv_u_db_sess_t *nxs_chat_srv_u_db_sess_init(void)
{
	nxs_chat_srv_u_db_sess_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_db_sess_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_db_sess_t));

	nxs_list_init2(&u_ctx->sessions, nxs_chat_srv_u_db_sess_el_t);

	return u_ctx;
}

nxs_chat_srv_u_db_sess_t *nxs_chat_srv_u_db_sess_free(nxs_chat_srv_u_db_sess_t *u_ctx)
{
	nxs_chat_srv_u_db_sess_el_t *s;

	if(u_ctx == NULL) {

		return NULL;
	}

	for(s = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, &u_ctx->sessions); s != NULL;
	    s = nxs_list_del(&u_ctx->sessions, NXS_LIST_MOVE_NEXT)) {

		nxs_chat_srv_u_db_sess_el_clear(s);
	}

	nxs_list_free(&u_ctx->sessions);

	return (nxs_chat_srv_u_db_sess_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_pull(nxs_chat_srv_u_db_sess_t *u_ctx)
{
	nxs_mysql_t               mysql;
	nxs_chat_srv_d_db_sess_t *sess_db_ctx;
	nxs_chat_srv_m_db_sess_t  sess;
	nxs_chat_srv_err_t        rc;
	size_t                    i;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(nxs_chat_srv_c_mysql_connect(&mysql) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process, "[%s]: MySQL connect error: \"%s\"", nxs_proc_get_name(&process), mysql.err_str);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	sess_db_ctx = nxs_chat_srv_d_db_sess_init();

	if(nxs_chat_srv_d_db_sess_select(sess_db_ctx, &mysql) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	for(i = 0; nxs_chat_srv_d_db_sess_get(sess_db_ctx, i, &sess) == NXS_CHAT_SRV_E_OK; i++) {

		nxs_chat_srv_u_db_sess_el_add(u_ctx,
		                              sess.id,
		                              sess.tlgrm_userid,
		                              sess.chat_id,
		                              sess.message_id,
		                              sess.updated_at,
		                              sess.type,
		                              sess.wait_for,
		                              sess.data);
	}

error:

	nxs_chat_srv_c_mysql_disconnect(&mysql);

	sess_db_ctx = nxs_chat_srv_d_db_sess_free(sess_db_ctx);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_start(nxs_chat_srv_u_db_sess_t *             u_ctx,
                                                size_t *                               sess_id,
                                                size_t                                 tlgrm_user_id,
                                                size_t                                 chat_id,
                                                size_t                                 message_id,
                                                nxs_chat_srv_m_db_sess_type_t          type,
                                                nxs_chat_srv_m_db_sess_wait_for_type_t wait_for)
{
	nxs_chat_srv_u_db_sess_el_t *s;
	nxs_mysql_t                  mysql;
	nxs_chat_srv_err_t           rc;
	time_t                       updated_at;

	if(u_ctx == NULL || sess_id == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(tlgrm_user_id == 0) {

		nxs_log_write_error(&process,
		                    "[%s]: session start error: wrong user id (tlgrm user id: %zu)",
		                    nxs_proc_get_name(&process),
		                    tlgrm_user_id);

		return NXS_CHAT_SRV_E_ERR;
	}

	if(nxs_chat_srv_c_mysql_connect(&mysql) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process, "[%s]: MySQL connect error: \"%s\"", nxs_proc_get_name(&process), mysql.err_str);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	/* dropping other user sessions */
	for(s = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, &u_ctx->sessions); s != NULL;) {

		if(s->tlgrm_userid == tlgrm_user_id) {

			if(nxs_chat_srv_d_db_sess_delete(&mysql, s->id) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			nxs_chat_srv_u_db_sess_el_clear(s);

			s = nxs_list_del(&u_ctx->sessions, NXS_LIST_MOVE_NEXT);
		}
		else {

			s = nxs_list_ptr_next(&u_ctx->sessions);
		}
	}

	updated_at = time(NULL);

	if(nxs_chat_srv_d_db_sess_insert(&mysql, sess_id, tlgrm_user_id, chat_id, message_id, updated_at, type, wait_for, &_s_empty) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_chat_srv_u_db_sess_el_add(u_ctx, *sess_id, tlgrm_user_id, chat_id, message_id, updated_at, type, wait_for, &_s_empty);

error:

	nxs_chat_srv_c_mysql_disconnect(&mysql);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_destroy(nxs_chat_srv_u_db_sess_t *u_ctx, size_t sess_id)
{
	nxs_chat_srv_u_db_sess_el_t *s;
	nxs_mysql_t                  mysql;
	nxs_chat_srv_err_t           rc;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if(sess_id == 0) {

		nxs_log_write_error(
		        &process, "[%s]: session destroy error: wrong session id (session id: %zu)", nxs_proc_get_name(&process), sess_id);

		return NXS_CHAT_SRV_E_ERR;
	}

	if(nxs_chat_srv_c_mysql_connect(&mysql) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process, "[%s]: MySQL connect error: \"%s\"", nxs_proc_get_name(&process), mysql.err_str);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_EXIST;

	for(s = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, &u_ctx->sessions); s != NULL;) {

		if(s->id == sess_id) {

			if(nxs_chat_srv_d_db_sess_delete(&mysql, s->id) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			nxs_chat_srv_u_db_sess_el_clear(s);

			s = nxs_list_del(&u_ctx->sessions, NXS_LIST_MOVE_NEXT);

			nxs_error(rc, NXS_CHAT_SRV_E_OK, error);
		}
		else {

			s = nxs_list_ptr_next(&u_ctx->sessions);
		}
	}

error:

	nxs_chat_srv_c_mysql_disconnect(&mysql);

	return rc;
}

/*
 * TODO: add sess_check: check id, chat_id, message_id and destroy session if wrong
 */

size_t nxs_chat_srv_u_db_sess_get_id(nxs_chat_srv_u_db_sess_t *u_ctx, size_t tlgrm_user_id)
{
	nxs_chat_srv_u_db_sess_el_t *s;

	if(u_ctx == NULL) {

		return 0;
	}

	if(tlgrm_user_id == 0) {

		return 0;
	}

	for(s = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, &u_ctx->sessions); s != NULL; s = nxs_list_ptr_next(&u_ctx->sessions)) {

		if(s->tlgrm_userid == tlgrm_user_id) {

			return s->id;
		}
	}

	return 0;
}

size_t nxs_chat_srv_u_db_sess_get_chat_id(nxs_chat_srv_u_db_sess_t *u_ctx, size_t sess_id)
{
	nxs_chat_srv_u_db_sess_el_t *s;

	if(u_ctx == NULL) {

		return 0;
	}

	if((s = nxs_chat_srv_u_db_sess_el_get(u_ctx, sess_id)) == NULL) {

		return 0;
	}

	return s->chat_id;
}

size_t nxs_chat_srv_u_db_sess_get_message_id(nxs_chat_srv_u_db_sess_t *u_ctx, size_t sess_id)
{
	nxs_chat_srv_u_db_sess_el_t *s;

	if(u_ctx == NULL) {

		return 0;
	}

	if((s = nxs_chat_srv_u_db_sess_el_get(u_ctx, sess_id)) == NULL) {

		return 0;
	}

	return s->message_id;
}

nxs_chat_srv_m_db_sess_type_t nxs_chat_srv_u_db_sess_get_type(nxs_chat_srv_u_db_sess_t *u_ctx, size_t sess_id)
{
	nxs_chat_srv_u_db_sess_el_t *s;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE;
	}

	if((s = nxs_chat_srv_u_db_sess_el_get(u_ctx, sess_id)) == NULL) {

		return NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE;
	}

	return s->type;
}

nxs_chat_srv_m_db_sess_wait_for_type_t nxs_chat_srv_u_db_sess_get_wait_for(nxs_chat_srv_u_db_sess_t *u_ctx, size_t sess_id)
{
	nxs_chat_srv_u_db_sess_el_t *s;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;
	}

	if((s = nxs_chat_srv_u_db_sess_el_get(u_ctx, sess_id)) == NULL) {

		return NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;
	}

	return s->wait_for;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_set_wait_for(nxs_chat_srv_u_db_sess_t *             u_ctx,
                                                       size_t                                 sess_id,
                                                       nxs_chat_srv_m_db_sess_wait_for_type_t wait_for)
{
	nxs_chat_srv_u_db_sess_el_t *s;
	nxs_mysql_t                  mysql;
	nxs_chat_srv_err_t           rc;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((s = nxs_chat_srv_u_db_sess_el_get(u_ctx, sess_id)) == NULL) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(nxs_chat_srv_c_mysql_connect(&mysql) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process, "[%s]: MySQL connect error: \"%s\"", nxs_proc_get_name(&process), mysql.err_str);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	switch(s->type) {

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE:

			if(wait_for != NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_SUBJECT &&
			   wait_for != NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_ISSUE_DESCRIPTION &&
			   wait_for != NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE) {

				nxs_error(rc, NXS_CHAT_SRV_E_TYPE, error);
			}

			s->wait_for   = wait_for;
			s->updated_at = time(NULL);

			if(nxs_chat_srv_d_db_sess_update(&mysql,
			                                 s->id,
			                                 s->tlgrm_userid,
			                                 s->chat_id,
			                                 s->message_id,
			                                 s->updated_at,
			                                 s->type,
			                                 s->wait_for,
			                                 &s->data) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		default:

			nxs_error(rc, NXS_CHAT_SRV_E_TYPE, error);
	}

error:

	nxs_chat_srv_c_mysql_disconnect(&mysql);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_set_ids(nxs_chat_srv_u_db_sess_t *u_ctx, size_t sess_id, size_t chat_id, size_t message_id)
{
	nxs_chat_srv_u_db_sess_el_t *s;
	nxs_mysql_t                  mysql;
	nxs_chat_srv_err_t           rc;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((s = nxs_chat_srv_u_db_sess_el_get(u_ctx, sess_id)) == NULL) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(nxs_chat_srv_c_mysql_connect(&mysql) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process, "[%s]: MySQL connect error: \"%s\"", nxs_proc_get_name(&process), mysql.err_str);

		return NXS_CHAT_SRV_E_ERR;
	}

	if(chat_id == 0 && message_id == 0) {

		return NXS_CHAT_SRV_E_OK;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if(chat_id > 0) {

		s->chat_id = chat_id;
	}

	if(message_id > 0) {

		s->message_id = message_id;
	}

	s->updated_at = time(NULL);

	if(nxs_chat_srv_d_db_sess_update(
	           &mysql, s->id, s->tlgrm_userid, s->chat_id, s->message_id, s->updated_at, s->type, s->wait_for, &s->data) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_chat_srv_c_mysql_disconnect(&mysql);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_t_get_message(nxs_chat_srv_u_db_sess_t *u_ctx, size_t sess_id, nxs_string_t *message)
{
	nxs_chat_srv_u_db_sess_el_t *      s;
	nxs_chat_srv_u_db_sess_t_message_t m;
	nxs_chat_srv_err_t                 rc;

	if(u_ctx == NULL || message == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((s = nxs_chat_srv_u_db_sess_el_get(u_ctx, sess_id)) == NULL) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(s->type != NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE) {

		return NXS_CHAT_SRV_E_TYPE;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_chat_srv_u_db_sess_s_message_init(&m);

	if(nxs_chat_srv_u_db_sess_s_message_deserialize(&m, &s->data) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	nxs_base64_decode_string(message, &m.message);

error:

	nxs_chat_srv_u_db_sess_s_message_free(&m);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_t_set_message(nxs_chat_srv_u_db_sess_t *u_ctx, size_t sess_id, nxs_string_t *message)
{
	nxs_chat_srv_u_db_sess_el_t *      s;
	nxs_chat_srv_u_db_sess_t_message_t m;
	nxs_chat_srv_err_t                 rc;
	nxs_mysql_t                        mysql;

	if(u_ctx == NULL || message == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((s = nxs_chat_srv_u_db_sess_el_get(u_ctx, sess_id)) == NULL) {

		nxs_log_write_warn(&process, "[%s]: can't find session (session id: %zu)", nxs_proc_get_name(&process), sess_id);

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(s->type != NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE) {

		nxs_log_write_warn(&process,
		                   "[%s]: wrong session type for 'sess_t_set_message' (session id: %zu, session type: %d)",
		                   nxs_proc_get_name(&process),
		                   sess_id,
		                   s->type);

		return NXS_CHAT_SRV_E_TYPE;
	}

	if(nxs_chat_srv_c_mysql_connect(&mysql) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process, "[%s]: MySQL connect error: \"%s\"", nxs_proc_get_name(&process), mysql.err_str);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_chat_srv_u_db_sess_s_message_init(&m);

	nxs_string_clone(&m.message, message);

	nxs_chat_srv_u_db_sess_s_message_serialize(&m, &s->data);

	s->updated_at = time(NULL);

	if(nxs_chat_srv_d_db_sess_update(
	           &mysql, s->id, s->tlgrm_userid, s->chat_id, s->message_id, s->updated_at, s->type, s->wait_for, &s->data) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_chat_srv_c_mysql_disconnect(&mysql);

	nxs_chat_srv_u_db_sess_s_message_free(&m);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_t_get_new_issue(nxs_chat_srv_u_db_sess_t *u_ctx,
                                                          size_t                    sess_id,
                                                          size_t *                  project_id,
                                                          nxs_string_t *            project_name,
                                                          size_t *                  priority_id,
                                                          nxs_string_t *            priority_name,
                                                          nxs_string_t *            subject,
                                                          nxs_string_t *            description)
{
	nxs_chat_srv_u_db_sess_el_t *        s;
	nxs_chat_srv_u_db_sess_t_new_issue_t iss;
	nxs_chat_srv_err_t                   rc;

	if(u_ctx == NULL || project_id == NULL || project_name == NULL || priority_id == NULL || priority_name == NULL || subject == NULL ||
	   description == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((s = nxs_chat_srv_u_db_sess_el_get(u_ctx, sess_id)) == NULL) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(s->type != NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE) {

		return NXS_CHAT_SRV_E_TYPE;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_chat_srv_u_db_sess_s_new_issue_init(&iss);

	if(nxs_chat_srv_u_db_sess_s_new_issue_deserialize(&iss, &s->data) != NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	*project_id  = iss.project_id;
	*priority_id = iss.priority_id;

	nxs_base64_decode_string(project_name, &iss.project_name);
	nxs_base64_decode_string(priority_name, &iss.priority_name);
	nxs_base64_decode_string(subject, &iss.subject);
	nxs_base64_decode_string(description, &iss.description);

error:

	nxs_chat_srv_u_db_sess_s_new_issue_free(&iss);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_t_set_new_issue(nxs_chat_srv_u_db_sess_t *u_ctx,
                                                          size_t                    sess_id,
                                                          size_t                    project_id,
                                                          nxs_string_t *            project_name,
                                                          size_t                    priority_id,
                                                          nxs_string_t *            priority_name,
                                                          nxs_string_t *            subject,
                                                          nxs_string_t *            description)
{
	nxs_chat_srv_u_db_sess_el_t *        s;
	nxs_chat_srv_u_db_sess_t_new_issue_t iss;
	nxs_chat_srv_err_t                   rc;
	nxs_mysql_t                          mysql;

	if(u_ctx == NULL || project_name == NULL || priority_name == NULL || subject == NULL || description == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((s = nxs_chat_srv_u_db_sess_el_get(u_ctx, sess_id)) == NULL) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(s->type != NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE) {

		return NXS_CHAT_SRV_E_TYPE;
	}

	if(nxs_chat_srv_c_mysql_connect(&mysql) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process, "[%s]: MySQL connect error: \"%s\"", nxs_proc_get_name(&process), mysql.err_str);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_chat_srv_u_db_sess_s_new_issue_init(&iss);

	iss.project_id  = project_id;
	iss.priority_id = priority_id;

	nxs_string_clone(&iss.project_name, project_name);
	nxs_string_clone(&iss.priority_name, priority_name);
	nxs_string_clone(&iss.subject, subject);
	nxs_string_clone(&iss.description, description);

	nxs_chat_srv_u_db_sess_s_new_issue_serialize(&iss, &s->data);

	s->updated_at = time(NULL);

	if(nxs_chat_srv_d_db_sess_update(
	           &mysql, s->id, s->tlgrm_userid, s->chat_id, s->message_id, s->updated_at, s->type, s->wait_for, &s->data) !=
	   NXS_CHAT_SRV_E_OK) {

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_chat_srv_c_mysql_disconnect(&mysql);

	nxs_chat_srv_u_db_sess_s_new_issue_free(&iss);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_t_conv_to_new_issue(nxs_chat_srv_u_db_sess_t *u_ctx,
                                                              size_t                    sess_id,
                                                              size_t                    project_id,
                                                              nxs_string_t *            project_name,
                                                              size_t                    priority_id,
                                                              nxs_string_t *            priority_name,
                                                              nxs_string_t *            subject,
                                                              nxs_string_t *            description)
{
	nxs_chat_srv_u_db_sess_el_t *        s;
	nxs_chat_srv_u_db_sess_t_message_t   m;
	nxs_chat_srv_u_db_sess_t_new_issue_t iss;
	nxs_chat_srv_err_t                   rc;
	nxs_mysql_t                          mysql;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((s = nxs_chat_srv_u_db_sess_el_get(u_ctx, sess_id)) == NULL) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(nxs_chat_srv_c_mysql_connect(&mysql) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process, "[%s]: MySQL connect error: \"%s\"", nxs_proc_get_name(&process), mysql.err_str);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_chat_srv_u_db_sess_s_message_init(&m);
	nxs_chat_srv_u_db_sess_s_new_issue_init(&iss);

	switch(s->type) {

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE:

			/* conv from 's->type' session type */

			if(nxs_chat_srv_u_db_sess_s_message_deserialize(&m, &s->data) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			nxs_base64_decode_string(&iss.description, &m.message);

			iss.project_id  = project_id;
			iss.priority_id = priority_id;

			nxs_string_clone(&iss.project_name, project_name);
			nxs_string_clone(&iss.priority_name, priority_name);
			nxs_string_clone(&iss.subject, subject);

			nxs_chat_srv_u_db_sess_s_new_issue_serialize(&iss, &s->data);

			s->updated_at = time(NULL);
			s->type       = NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE;
			s->wait_for   = NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;

			if(nxs_chat_srv_d_db_sess_update(&mysql,
			                                 s->id,
			                                 s->tlgrm_userid,
			                                 s->chat_id,
			                                 s->message_id,
			                                 s->updated_at,
			                                 s->type,
			                                 s->wait_for,
			                                 &s->data) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		default:

			nxs_error(rc, NXS_CHAT_SRV_E_TYPE, error);
	}

error:

	nxs_chat_srv_c_mysql_disconnect(&mysql);

	nxs_chat_srv_u_db_sess_s_message_free(&m);
	nxs_chat_srv_u_db_sess_s_new_issue_free(&iss);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_t_conv_to_message(nxs_chat_srv_u_db_sess_t *u_ctx, size_t sess_id, nxs_string_t *message)
{
	nxs_chat_srv_u_db_sess_el_t *        s;
	nxs_chat_srv_u_db_sess_t_message_t   m;
	nxs_chat_srv_u_db_sess_t_new_issue_t iss;
	nxs_chat_srv_err_t                   rc;
	nxs_mysql_t                          mysql;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((s = nxs_chat_srv_u_db_sess_el_get(u_ctx, sess_id)) == NULL) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	if(nxs_chat_srv_c_mysql_connect(&mysql) != NXS_CHAT_SRV_E_OK) {

		nxs_log_write_error(&process, "[%s]: MySQL connect error: \"%s\"", nxs_proc_get_name(&process), mysql.err_str);

		return NXS_CHAT_SRV_E_ERR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_chat_srv_u_db_sess_s_message_init(&m);
	nxs_chat_srv_u_db_sess_s_new_issue_init(&iss);

	switch(s->type) {

		case NXS_CHAT_SRV_M_DB_SESS_TYPE_NEW_ISSUE:

			/* conv 's->type' from session type */

			if(nxs_chat_srv_u_db_sess_s_new_issue_deserialize(&iss, &s->data) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			nxs_base64_decode_string(&m.message, &iss.description);

			nxs_chat_srv_u_db_sess_s_message_serialize(&m, &s->data);

			s->updated_at = time(NULL);
			s->type       = NXS_CHAT_SRV_M_DB_SESS_TYPE_MESSAGE;
			s->wait_for   = NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;

			if(nxs_chat_srv_d_db_sess_update(&mysql,
			                                 s->id,
			                                 s->tlgrm_userid,
			                                 s->chat_id,
			                                 s->message_id,
			                                 s->updated_at,
			                                 s->type,
			                                 s->wait_for,
			                                 &s->data) != NXS_CHAT_SRV_E_OK) {

				nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
			}

			break;

		default:

			nxs_error(rc, NXS_CHAT_SRV_E_TYPE, error);
	}

error:

	nxs_chat_srv_c_mysql_disconnect(&mysql);

	nxs_chat_srv_u_db_sess_s_message_free(&m);
	nxs_chat_srv_u_db_sess_s_new_issue_free(&iss);

	return rc;
}

/* Module internal (static) functions */

static nxs_chat_srv_u_db_sess_el_t *nxs_chat_srv_u_db_sess_el_get(nxs_chat_srv_u_db_sess_t *u_ctx, size_t sess_id)
{
	nxs_chat_srv_u_db_sess_el_t *s;

	if(sess_id == 0) {

		return NULL;
	}

	for(s = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, &u_ctx->sessions); s != NULL; s = nxs_list_ptr_next(&u_ctx->sessions)) {

		if(s->id == sess_id) {

			return s;
		}
	}

	return NULL;
}

static nxs_chat_srv_u_db_sess_el_t *nxs_chat_srv_u_db_sess_el_add(nxs_chat_srv_u_db_sess_t *             u_ctx,
                                                                  size_t                                 id,
                                                                  size_t                                 tlgrm_userid,
                                                                  size_t                                 chat_id,
                                                                  size_t                                 message_id,
                                                                  time_t                                 updated_at,
                                                                  nxs_chat_srv_m_db_sess_type_t          type,
                                                                  nxs_chat_srv_m_db_sess_wait_for_type_t wait_for,
                                                                  nxs_string_t *                         data)
{
	nxs_chat_srv_u_db_sess_el_t *s;

	s = nxs_list_add_tail(&u_ctx->sessions);

	s->id           = id;
	s->tlgrm_userid = tlgrm_userid;
	s->chat_id      = chat_id;
	s->message_id   = message_id;
	s->updated_at   = updated_at;
	s->type         = type;
	s->wait_for     = wait_for;

	nxs_string_init3(&s->data, data);

	return s;
}

static void nxs_chat_srv_u_db_sess_el_clear(nxs_chat_srv_u_db_sess_el_t *s)
{

	s->id           = 0;
	s->tlgrm_userid = 0;
	s->chat_id      = 0;
	s->message_id   = 0;
	s->updated_at   = 0;
	s->type         = NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE;
	s->wait_for     = NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;

	nxs_string_free(&s->data);
}

static void nxs_chat_srv_u_db_sess_s_message_init(nxs_chat_srv_u_db_sess_t_message_t *ctx)
{

	nxs_string_init_empty(&ctx->message);
}

static void nxs_chat_srv_u_db_sess_s_message_free(nxs_chat_srv_u_db_sess_t_message_t *ctx)
{

	nxs_string_free(&ctx->message);
}

static void nxs_chat_srv_u_db_sess_s_message_serialize(nxs_chat_srv_u_db_sess_t_message_t *ctx, nxs_string_t *out_str)
{
	nxs_string_t message_encoded;

	nxs_string_init(&message_encoded);

	nxs_base64_encode_string(&message_encoded, &ctx->message);

	nxs_string_printf_dyn(out_str, "{\"message\":\"%r\"}", &message_encoded);

	nxs_string_free(&message_encoded);
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_message_deserialize(nxs_chat_srv_u_db_sess_t_message_t *ctx, nxs_string_t *data)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_message,	&ctx->message,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, (nxs_buf_t *)data) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: tlgrm db-sess unit error: message deserialize error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static void nxs_chat_srv_u_db_sess_s_new_issue_init(nxs_chat_srv_u_db_sess_t_new_issue_t *ctx)
{

	ctx->priority_id = 0;
	ctx->project_id  = 0;

	nxs_string_init_empty(&ctx->description);
	nxs_string_init_empty(&ctx->priority_name);
	nxs_string_init_empty(&ctx->project_name);
	nxs_string_init_empty(&ctx->subject);
}

static void nxs_chat_srv_u_db_sess_s_new_issue_free(nxs_chat_srv_u_db_sess_t_new_issue_t *ctx)
{
	ctx->priority_id = 0;
	ctx->project_id  = 0;

	nxs_string_free(&ctx->description);
	nxs_string_free(&ctx->priority_name);
	nxs_string_free(&ctx->project_name);
	nxs_string_free(&ctx->subject);
}

static void nxs_chat_srv_u_db_sess_s_new_issue_serialize(nxs_chat_srv_u_db_sess_t_new_issue_t *ctx, nxs_string_t *out_str)
{
	nxs_string_t description_encoded, priority_name_encoded, project_name_encoded, subject_encoded;

	nxs_string_init(&description_encoded);
	nxs_string_init(&priority_name_encoded);
	nxs_string_init(&project_name_encoded);
	nxs_string_init(&subject_encoded);

	nxs_base64_encode_string(&description_encoded, &ctx->description);
	nxs_base64_encode_string(&priority_name_encoded, &ctx->priority_name);
	nxs_base64_encode_string(&project_name_encoded, &ctx->project_name);
	nxs_base64_encode_string(&subject_encoded, &ctx->subject);

	nxs_string_printf_dyn(out_str,
	                      "{\"project_id\":%zu,"
	                      "\"project_name\":\"%r\","
	                      "\"priority_id\":%zu,"
	                      "\"priority_name\":\"%r\","
	                      "\"subject\":\"%r\","
	                      "\"description\":\"%r\"}",
	                      ctx->project_id,
	                      &project_name_encoded,
	                      ctx->priority_id,
	                      &priority_name_encoded,
	                      &subject_encoded,
	                      &description_encoded);

	nxs_string_free(&description_encoded);
	nxs_string_free(&priority_name_encoded);
	nxs_string_free(&project_name_encoded);
	nxs_string_free(&subject_encoded);
}

static nxs_chat_srv_err_t nxs_chat_srv_u_db_sess_s_new_issue_deserialize(nxs_chat_srv_u_db_sess_t_new_issue_t *ctx, nxs_string_t *data)
{
	nxs_chat_srv_err_t rc;
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	nxs_cfg_json_conf_array_skip_undef(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_project_id,	&ctx->project_id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_project_name,	&ctx->project_name,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_priority_id,	&ctx->priority_id,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_priority_name,	&ctx->priority_name,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_subject,	&ctx->subject,		NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_description,	&ctx->description,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, (nxs_buf_t *)data) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_error(&process, "[%s]: tlgrm db-sess unit error: new issue deserialize error", nxs_proc_get_name(&process));

		rc = NXS_CHAT_SRV_E_ERR;
	}

	nxs_cfg_json_free(&cfg_json);
	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}
