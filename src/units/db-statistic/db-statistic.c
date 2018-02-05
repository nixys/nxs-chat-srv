#ifndef _INCLUDE_NXS_CHAT_SRV_U_DB_STATISTIC_C
#define _INCLUDE_NXS_CHAT_SRV_U_DB_STATISTIC_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/db-statistic/db-statistic-typedefs.h>
#include <units/db-statistic/db-statistic.h>
#include <units/db-statistic/db-statistic-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	nxs_chat_srv_u_db_statistic_action_type_t	action_type;
	time_t						date;
} nxs_chat_srv_u_db_statistic_user_el_t;

typedef struct
{
	size_t						rdmn_userid;
	nxs_array_t					els;			/* type: nxs_chat_srv_u_db_statistic_user_el_t */
} nxs_chat_srv_u_db_statistic_user_t;

struct nxs_chat_srv_u_db_statistic_s
{
	nxs_string_t					buf;
	nxs_array_t					stat_users;		/* type: nxs_chat_srv_u_db_statistic_user_t */
	nxs_chat_srv_d_db_statistic_t			*db_statistic_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

static void nxs_chat_srv_u_db_statistic_serialize(nxs_chat_srv_u_db_statistic_t *           u_ctx,
                                                  nxs_chat_srv_u_db_statistic_action_type_t action_type,
                                                  size_t                                    rdmn_userid);
nxs_chat_srv_err_t nxs_chat_srv_u_db_statistic_deserialize(nxs_string_t *                             buf,
                                                           nxs_chat_srv_u_db_statistic_action_type_t *action_type,
                                                           size_t *                                   rdmn_userid,
                                                           time_t *                                   date);

static void nxs_chat_srv_u_db_statistic_user_init(void *element);
static void nxs_chat_srv_u_db_statistic_user_free(void *element);
static void nxs_chat_srv_u_db_statistic_user_add(nxs_array_t *                             stat_users,
                                                 size_t                                    rdmn_userid,
                                                 nxs_chat_srv_u_db_statistic_action_type_t action_type,
                                                 time_t                                    date);

// clang-format off

/* Module initializations */

static nxs_string_t _s_par_action_type		= nxs_string("action_type");
static nxs_string_t _s_par_rdmn_userid		= nxs_string("rdmn_userid");
static nxs_string_t _s_par_date			= nxs_string("date");

/* Module global functions */

// clang-format on

nxs_chat_srv_u_db_statistic_t *nxs_chat_srv_u_db_statistic_init(void)
{
	nxs_chat_srv_u_db_statistic_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_db_statistic_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_db_statistic_t));

	nxs_string_init(&u_ctx->buf);

	nxs_array_init3(&u_ctx->stat_users,
	                nxs_chat_srv_u_db_statistic_user_t,
	                &nxs_chat_srv_u_db_statistic_user_init,
	                &nxs_chat_srv_u_db_statistic_user_free);

	u_ctx->db_statistic_ctx = nxs_chat_srv_d_db_statistic_init();

	return u_ctx;
}

nxs_chat_srv_u_db_statistic_t *nxs_chat_srv_u_db_statistic_free(nxs_chat_srv_u_db_statistic_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	nxs_string_free(&u_ctx->buf);

	u_ctx->db_statistic_ctx = nxs_chat_srv_d_db_statistic_free(u_ctx->db_statistic_ctx);

	nxs_array_free(&u_ctx->stat_users);

	return (nxs_chat_srv_u_db_statistic_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_statistic_add(nxs_chat_srv_u_db_statistic_t *           u_ctx,
                                                   nxs_chat_srv_u_db_statistic_action_type_t action_type,
                                                   size_t                                    rdmn_userid)
{

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	nxs_chat_srv_u_db_statistic_serialize(u_ctx, action_type, rdmn_userid);

	return nxs_chat_srv_d_db_statistic_add(u_ctx->db_statistic_ctx, &u_ctx->buf);
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_statistic_pull(nxs_chat_srv_u_db_statistic_t *u_ctx)
{
	nxs_chat_srv_u_db_statistic_action_type_t action_type;
	nxs_array_t                               stat_elements;
	nxs_string_t *                            s;
	nxs_chat_srv_err_t                        rc;
	size_t                                    i, rdmn_userid;
	time_t                                    date;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_array_init_string(&stat_elements);

	switch(nxs_chat_srv_d_db_statistic_get(u_ctx->db_statistic_ctx, &stat_elements)) {

		case NXS_CHAT_SRV_E_OK:

			for(i = 0; i < nxs_array_count(&stat_elements); i++) {

				s = nxs_array_get(&stat_elements, i);

				if(nxs_chat_srv_u_db_statistic_deserialize(s, &action_type, &rdmn_userid, &date) != NXS_CHAT_SRV_E_OK) {

					nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
				}

				nxs_chat_srv_u_db_statistic_user_add(&u_ctx->stat_users, rdmn_userid, action_type, date);
			}

			break;

		case NXS_CHAT_SRV_E_EXIST:

			break;

		default:

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_array_free(&stat_elements);

	return rc;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_statistic_get_general(nxs_chat_srv_u_db_statistic_t *        u_ctx,
                                                           nxs_chat_srv_u_db_statistic_general_t *stat_general,
                                                           size_t                                 i)
{
	nxs_chat_srv_u_db_statistic_user_t *   user;
	nxs_chat_srv_u_db_statistic_user_el_t *el;
	size_t                                 j;

	if(u_ctx == NULL || stat_general == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	stat_general->rdmn_userid                    = 0;
	stat_general->count_rdmn_issues_created      = 0;
	stat_general->count_rdmn_issues_updated      = 0;
	stat_general->count_tlgrm_sessions_destroyed = 0;
	stat_general->count_tlgrm_issues_created     = 0;
	stat_general->count_tlgrm_messages_created   = 0;
	stat_general->count_tlgrm_messages_replied   = 0;
	stat_general->count_tlgrm_messages_extended  = 0;
	stat_general->tlgrm_issues_replied_ratio     = 0.0;
	stat_general->tlgrm_issues_created_ratio     = 0.0;
	stat_general->last_action_timestamp          = 0;

	if((user = nxs_array_get(&u_ctx->stat_users, i)) == NULL) {

		return NXS_CHAT_SRV_E_EXIST;
	}

	stat_general->rdmn_userid = user->rdmn_userid;

	for(j = 0; j < nxs_array_count(&user->els); j++) {

		el = nxs_array_get(&user->els, j);

		if(el->date > stat_general->last_action_timestamp) {

			stat_general->last_action_timestamp = el->date;
		}

		switch(el->action_type) {

			case NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_RDMN_ISSUE_CREATE:

				stat_general->count_rdmn_issues_created++;

				break;

			case NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_RDMN_ISSUE_UPDATE:

				stat_general->count_rdmn_issues_updated++;

				break;

			case NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_SESSION_DESTROY:

				stat_general->count_tlgrm_sessions_destroyed++;

				break;

			case NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_ISSUE_CREATE:

				stat_general->count_tlgrm_issues_created++;

				break;

			case NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_MESSAGE_REPLIED:

				stat_general->count_tlgrm_messages_replied++;

				break;

			case NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_MESSAGE_CREATED:

				stat_general->count_tlgrm_messages_created++;

				break;

			case NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_MESSAGE_EXTENDED:

				stat_general->count_tlgrm_messages_extended++;

				break;
		}
	}

	if(stat_general->count_rdmn_issues_created > 0) {

		stat_general->tlgrm_issues_created_ratio =
		        (double)stat_general->count_tlgrm_issues_created / (double)stat_general->count_rdmn_issues_created;
	}

	if(stat_general->count_rdmn_issues_updated > 0) {

		stat_general->tlgrm_issues_replied_ratio =
		        (double)(stat_general->count_tlgrm_messages_replied + stat_general->count_tlgrm_messages_created +
		                 stat_general->count_tlgrm_messages_extended) /
		        (double)stat_general->count_rdmn_issues_updated;
	}

	stat_general->count_rdmn_issues_created -= stat_general->count_tlgrm_issues_created;
	stat_general->count_rdmn_issues_updated -=
	        (stat_general->count_tlgrm_messages_replied + stat_general->count_tlgrm_messages_created +
	         stat_general->count_tlgrm_messages_extended);

	return NXS_CHAT_SRV_E_OK;
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_statistic_get_total(nxs_chat_srv_u_db_statistic_t *      u_ctx,
                                                         nxs_chat_srv_u_db_statistic_total_t *stat_total)
{
	nxs_chat_srv_u_db_statistic_user_t *   user;
	nxs_chat_srv_u_db_statistic_user_el_t *el;
	size_t                                 i, j;

	if(u_ctx == NULL || stat_total == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	stat_total->count_rdmn_issues_created      = 0;
	stat_total->count_rdmn_issues_updated      = 0;
	stat_total->count_tlgrm_sessions_destroyed = 0;
	stat_total->count_tlgrm_issues_created     = 0;
	stat_total->count_tlgrm_messages_replied   = 0;
	stat_total->count_tlgrm_messages_created   = 0;
	stat_total->count_tlgrm_messages_extended  = 0;
	stat_total->tlgrm_issues_replied_ratio     = 0.0;
	stat_total->tlgrm_issues_created_ratio     = 0.0;
	stat_total->last_action_timestamp          = 0;

	for(i = 0; i < nxs_array_count(&u_ctx->stat_users); i++) {

		user = nxs_array_get(&u_ctx->stat_users, i);

		for(j = 0; j < nxs_array_count(&user->els); j++) {

			el = nxs_array_get(&user->els, j);

			if(el->date > stat_total->last_action_timestamp) {

				stat_total->last_action_timestamp = el->date;
			}

			switch(el->action_type) {

				case NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_RDMN_ISSUE_CREATE:

					stat_total->count_rdmn_issues_created++;

					break;

				case NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_RDMN_ISSUE_UPDATE:

					stat_total->count_rdmn_issues_updated++;

					break;

				case NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_SESSION_DESTROY:

					stat_total->count_tlgrm_sessions_destroyed++;

					break;

				case NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_ISSUE_CREATE:

					stat_total->count_tlgrm_issues_created++;

					break;

				case NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_MESSAGE_REPLIED:

					stat_total->count_tlgrm_messages_replied++;

					break;

				case NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_MESSAGE_CREATED:

					stat_total->count_tlgrm_messages_created++;

					break;

				case NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_MESSAGE_EXTENDED:

					stat_total->count_tlgrm_messages_extended++;

					break;
			}
		}
	}

	if(stat_total->count_rdmn_issues_created > 0) {

		stat_total->tlgrm_issues_created_ratio =
		        (double)stat_total->count_tlgrm_issues_created / (double)stat_total->count_rdmn_issues_created;
	}

	if(stat_total->count_rdmn_issues_updated > 0) {

		stat_total->tlgrm_issues_replied_ratio =
		        (double)(stat_total->count_tlgrm_messages_replied + stat_total->count_tlgrm_messages_created +
		                 stat_total->count_tlgrm_messages_extended) /
		        (double)stat_total->count_rdmn_issues_updated;
	}

	stat_total->count_rdmn_issues_created -= stat_total->count_tlgrm_issues_created;
	stat_total->count_rdmn_issues_updated -= (stat_total->count_tlgrm_messages_replied + stat_total->count_tlgrm_messages_created +
	                                          stat_total->count_tlgrm_messages_extended);

	return NXS_CHAT_SRV_E_OK;
}

/* Module internal (static) functions */

static void nxs_chat_srv_u_db_statistic_serialize(nxs_chat_srv_u_db_statistic_t *           u_ctx,
                                                  nxs_chat_srv_u_db_statistic_action_type_t action_type,
                                                  size_t                                    rdmn_userid)
{

	nxs_string_printf(&u_ctx->buf,
	                  "{\"%r\":%d,\"%r\":%zu,\"%r\":%zu}",
	                  &_s_par_action_type,
	                  action_type,
	                  &_s_par_rdmn_userid,
	                  rdmn_userid,
	                  &_s_par_date,
	                  time(NULL));
}

nxs_chat_srv_err_t nxs_chat_srv_u_db_statistic_deserialize(nxs_string_t *                             buf,
                                                           nxs_chat_srv_u_db_statistic_action_type_t *action_type,
                                                           size_t *                                   rdmn_userid,
                                                           time_t *                                   date)
{
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;
	nxs_chat_srv_err_t rc;

	if(buf == NULL || action_type == NULL || rdmn_userid == NULL || date == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_action_type,	action_type,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT_32,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_rdmn_userid,	rdmn_userid,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_date,		date,		NULL,	NULL,	NXS_CFG_JSON_TYPE_INT,		0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, (nxs_buf_t *)buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "parse statistic json data error");

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

static void nxs_chat_srv_u_db_statistic_user_init(void *element)
{
	nxs_chat_srv_u_db_statistic_user_t *user = element;

	user->rdmn_userid = 0;

	nxs_array_init2(&user->els, nxs_chat_srv_u_db_statistic_user_el_t);
}

static void nxs_chat_srv_u_db_statistic_user_free(void *element)
{
	nxs_chat_srv_u_db_statistic_user_t *user = element;

	user->rdmn_userid = 0;

	nxs_array_free(&user->els);
}

static void nxs_chat_srv_u_db_statistic_user_add(nxs_array_t *                             stat_users,
                                                 size_t                                    rdmn_userid,
                                                 nxs_chat_srv_u_db_statistic_action_type_t action_type,
                                                 time_t                                    date)
{
	nxs_chat_srv_u_db_statistic_user_t *   user;
	nxs_chat_srv_u_db_statistic_user_el_t *el;
	size_t                                 i;

	if(stat_users == NULL) {

		return;
	}

	for(i = 0; i < nxs_array_count(stat_users); i++) {

		user = nxs_array_get(stat_users, i);

		if(user->rdmn_userid == rdmn_userid) {

			el = nxs_array_add(&user->els);

			el->action_type = action_type;
			el->date        = date;

			return;
		}
	}

	user = nxs_array_add(stat_users);

	user->rdmn_userid = rdmn_userid;

	el = nxs_array_add(&user->els);

	el->action_type = action_type;
	el->date        = date;
}
