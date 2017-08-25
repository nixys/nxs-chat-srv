#ifndef _INCLUDE_NXS_CHAT_SRV_D_DB_SESS_C
#define _INCLUDE_NXS_CHAT_SRV_D_DB_SESS_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>

#include <dal/db-sess/db-sess-typedefs.h>
#include <dal/db-sess/db-sess.h>
#include <dal/db-sess/db-sess-subdals.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t			process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef enum
{
	NXS_CHAT_SRV_D_DB_SESS_FIELD_ID			= 0,
	NXS_CHAT_SRV_D_DB_SESS_FIELD_TLGRM_USER_ID	= 1,
	NXS_CHAT_SRV_D_DB_SESS_FIELD_CHAT_ID		= 2,
	NXS_CHAT_SRV_D_DB_SESS_FIELD_MESSAGE_ID		= 3,
	NXS_CHAT_SRV_D_DB_SESS_FIELD_UPDATED_AT		= 4,
	NXS_CHAT_SRV_D_DB_SESS_FIELD_TYPE		= 5,
	NXS_CHAT_SRV_D_DB_SESS_FIELD_WAIT_FOR		= 6,
	NXS_CHAT_SRV_D_DB_SESS_FIELD_DATA		= 7
} nxs_staff_srv_d_db_sess_fields_t;

typedef struct
{
	size_t						id;
	size_t						tlgrm_userid;
	size_t						chat_id;
	size_t						message_id;
	time_t						updated_at;
	nxs_chat_srv_m_db_sess_type_t			type;
	nxs_chat_srv_m_db_sess_wait_for_type_t		wait_for;
	nxs_string_t					data;
} nxs_chat_srv_d_db_sess_el_t;

struct nxs_chat_srv_d_db_sess_s
{
	nxs_array_t					sessions;		/* type: nxs_chat_srv_d_db_sess_el_t */
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_chat_srv_d_db_sess_t *nxs_chat_srv_d_db_sess_init(void)
{
	nxs_chat_srv_d_db_sess_t *d_ctx;

	d_ctx = (nxs_chat_srv_d_db_sess_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_d_db_sess_t));

	nxs_array_init2(&d_ctx->sessions, nxs_chat_srv_d_db_sess_el_t);

	return d_ctx;
}

nxs_chat_srv_d_db_sess_t *nxs_chat_srv_d_db_sess_free(nxs_chat_srv_d_db_sess_t *d_ctx)
{
	nxs_chat_srv_d_db_sess_el_t *s;
	size_t                       i;

	if(d_ctx == NULL) {

		return NULL;
	}

	for(i = 0; i < nxs_array_count(&d_ctx->sessions); i++) {

		s = nxs_array_get(&d_ctx->sessions, i);

		s->id           = 0;
		s->tlgrm_userid = 0;
		s->chat_id      = 0;
		s->message_id   = 0;
		s->updated_at   = 0;
		s->type         = NXS_CHAT_SRV_M_DB_SESS_TYPE_NONE;
		s->wait_for     = NXS_CHAT_SRV_M_DB_SESS_WAIT_FOR_TYPE_NONE;

		nxs_string_free(&s->data);
	}

	nxs_array_free(&d_ctx->sessions);

	return (nxs_chat_srv_d_db_sess_t *)nxs_free(d_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_sess_select(nxs_chat_srv_d_db_sess_t *d_ctx, nxs_mysql_t *mysql_ctx)
{
	nxs_mysql_res_t              res;
	nxs_chat_srv_d_db_sess_el_t *s;
	nxs_chat_srv_err_t           rc;
	size_t                       i, count;

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
	                   "	`tlgrm_user_id`, "
	                   "	`chat_id`, "
	                   "	`message_id`, "
	                   "	`updated_at`, "
	                   "	`type`, "
	                   "	`wait_for`, "
	                   "	`data` "
	                   "FROM "
	                   "	`sessions`") != NXS_MYSQL_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: MySQL query select sessions error: \"%s\"",
		                    nxs_proc_get_name(&process),
		                    (char *)mysql_ctx->err_str);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	if((count = nxs_mysql_res_rows(&res)) == 0) {

		nxs_error(rc, NXS_CHAT_SRV_E_OK, error);
	}

	for(i = 0; i < count; i++) {

		s = nxs_array_add(&d_ctx->sessions);

		s->id           = nxs_string_atoi(nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_SESS_FIELD_ID));
		s->tlgrm_userid = nxs_string_atoi(nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_SESS_FIELD_TLGRM_USER_ID));
		s->chat_id      = nxs_string_atoi(nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_SESS_FIELD_CHAT_ID));
		s->message_id   = nxs_string_atoi(nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_SESS_FIELD_MESSAGE_ID));
		s->updated_at   = nxs_string_atoi(nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_SESS_FIELD_UPDATED_AT));
		s->type         = nxs_string_atoi(nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_SESS_FIELD_TYPE));
		s->wait_for     = nxs_string_atoi(nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_SESS_FIELD_WAIT_FOR));

		nxs_string_init3(&s->data, nxs_mysql_res_get(&res, i, NXS_CHAT_SRV_D_DB_SESS_FIELD_DATA));
	}

error:

	nxs_mysql_res_free(&res);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_sess_insert(nxs_mysql_t *                          mysql_ctx,
                                                 size_t *                               sess_id,
                                                 size_t                                 tlgrm_userid,
                                                 size_t                                 chat_id,
                                                 size_t                                 message_id,
                                                 time_t                                 updated_at,
                                                 nxs_chat_srv_m_db_sess_type_t          type,
                                                 nxs_chat_srv_m_db_sess_wait_for_type_t wait_for,
                                                 nxs_string_t *                         data)
{
	nxs_mysql_res_t    res;
	nxs_chat_srv_err_t rc;

	if(mysql_ctx == NULL || data == NULL || sess_id == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_mysql_res_init(&res);

	if(nxs_mysql_query(mysql_ctx,
	                   NXS_MYSQL_QUERY_TYPE_INSERT,
	                   &res,
	                   "INSERT INTO"
	                   "	`sessions` "
	                   "    (`tlgrm_user_id`, `chat_id`, `message_id`, `updated_at`, `type`, `wait_for`, `data`) "
	                   "VALUES "
	                   "	(%zu, %zu, %zu, %zu, %d, %d, '%r')",
	                   tlgrm_userid,
	                   chat_id,
	                   message_id,
	                   updated_at,
	                   type,
	                   wait_for,
	                   data) != NXS_MYSQL_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: MySQL query insert into sessions error: \"%s\"",
		                    nxs_proc_get_name(&process),
		                    (char *)mysql_ctx->err_str);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	*sess_id = nxs_mysql_res_last_ins(&res);

error:

	nxs_mysql_res_free(&res);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_sess_update(nxs_mysql_t *                          mysql_ctx,
                                                 size_t                                 sess_id,
                                                 size_t                                 tlgrm_userid,
                                                 size_t                                 chat_id,
                                                 size_t                                 message_id,
                                                 time_t                                 updated_at,
                                                 nxs_chat_srv_m_db_sess_type_t          type,
                                                 nxs_chat_srv_m_db_sess_wait_for_type_t wait_for,
                                                 nxs_string_t *                         data)
{
	nxs_chat_srv_err_t rc;

	if(mysql_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if(nxs_mysql_query(mysql_ctx,
	                   NXS_MYSQL_QUERY_TYPE_UPDATE,
	                   NULL,
	                   "UPDATE `sessions` "
	                   "SET "
	                   "	`tlgrm_user_id` = %zu, "
	                   "	`chat_id` = %zu, "
	                   "	`message_id` = %zu, "
	                   "	`updated_at` = %zu, "
	                   "	`type` = %d, "
	                   "	`wait_for` = %d, "
	                   "	`data` = '%r'"
	                   "WHERE "
	                   "	`id` = %zu",
	                   tlgrm_userid,
	                   chat_id,
	                   message_id,
	                   updated_at,
	                   type,
	                   wait_for,
	                   data,
	                   sess_id) != NXS_MYSQL_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: MySQL query update sessions error: \"%s\"",
		                    nxs_proc_get_name(&process),
		                    (char *)mysql_ctx->err_str);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_sess_delete(nxs_mysql_t *mysql_ctx, size_t sess_id)
{
	nxs_chat_srv_err_t rc;

	if(mysql_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	if(nxs_mysql_query(mysql_ctx,
	                   NXS_MYSQL_QUERY_TYPE_UPDATE,
	                   NULL,
	                   "DELETE FROM `sessions` "
	                   "WHERE "
	                   "	`id` = %zu",
	                   sess_id) != NXS_MYSQL_E_OK) {

		nxs_log_write_error(&process,
		                    "[%s]: MySQL query delete from sessions error: \"%s\"",
		                    nxs_proc_get_name(&process),
		                    (char *)mysql_ctx->err_str);

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_d_db_sess_get(nxs_chat_srv_d_db_sess_t *d_ctx, size_t i, nxs_chat_srv_m_db_sess_t *sess)
{
	nxs_chat_srv_d_db_sess_el_t *s;

	if(d_ctx == NULL || sess == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	if((s = nxs_array_get(&d_ctx->sessions, i)) == NULL) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	sess->id           = s->id;
	sess->tlgrm_userid = s->tlgrm_userid;
	sess->chat_id      = s->chat_id;
	sess->message_id   = s->message_id;
	sess->updated_at   = s->updated_at;
	sess->type         = s->type;
	sess->wait_for     = s->wait_for;
	sess->data         = &s->data;

	return NXS_CHAT_SRV_E_OK;
}

/* Module internal (static) functions */
