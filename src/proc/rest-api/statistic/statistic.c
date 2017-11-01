#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-units.h>

#include <proc/rest-api/statistic/statistic.h>
#include <proc/rest-api/statistic/ctx/statistic-ctx.h>
#include <proc/rest-api/statistic/statistic-subproc.h>

/* Definitions */

#define TIME_STR_LEN		50
#define FORMAT_TIME		"%Y-%m-%d %H:%M:%S %z"

/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static void str_time(nxs_string_t *time_str, time_t date);

// clang-format off

/* Module initializations */

static nxs_string_t _s_ra_header_ct_key			= nxs_string("Content-Type");
static nxs_string_t _s_ra_header_ct_val_json		= nxs_string("application/json; charset=utf-8");
static nxs_string_t _s_ra_header_ct_val_html		= nxs_string("text/html; charset=utf-8");

static nxs_string_t _s_ra_arg_key_token			= nxs_string("token");
static nxs_string_t _s_ra_arg_key_format		= nxs_string("format");
static nxs_string_t _s_ra_arg_val_json			= nxs_string("json");
static nxs_string_t _s_ra_arg_val_html			= nxs_string("html");

/* Module global functions */

// clang-format on

void nxs_chat_srv_p_rest_api_statistic_handler_get(nxs_rest_api_ctx_t *rest_api_ctx, nxs_rest_api_request_t *req, void *custom_ctx)
{
	nxs_string_t *                        auth_token, *format_key, *response, stat_total_str, stat_general_str, time_str, username;
	nxs_chat_srv_u_db_statistic_t *       statistic_ctx;
	nxs_chat_srv_u_db_statistic_total_t   stat_total;
	nxs_chat_srv_u_db_statistic_general_t stat_general;
	nxs_chat_srv_u_db_cache_t *           cache_ctx;
	nxs_chat_srv_m_user_ctx_t             user_ctx;
	nxs_rest_api_format_err_t             content_type;
	size_t                                i;

	/* Request processing */

	if((format_key = nxs_rest_api_get_req_args_find(req, &_s_ra_arg_key_format)) == NULL) {

		nxs_rest_api_header_add(req, &_s_ra_header_ct_key, &_s_ra_header_ct_val_html);

		content_type = NXS_REST_API_FORMAT_ERR_HTML;
	}
	else {

		if(nxs_string_cmp(format_key, 0, &_s_ra_arg_val_html, 0) == NXS_YES) {

			nxs_rest_api_header_add(req, &_s_ra_header_ct_key, &_s_ra_header_ct_val_html);

			content_type = NXS_REST_API_FORMAT_ERR_HTML;
		}
		else {
			if(nxs_string_cmp(format_key, 0, &_s_ra_arg_val_json, 0) == NXS_YES) {

				nxs_rest_api_header_add(req, &_s_ra_header_ct_key, &_s_ra_header_ct_val_json);

				content_type = NXS_REST_API_FORMAT_ERR_JSON;
			}
			else {

				nxs_rest_api_header_add(req, &_s_ra_header_ct_key, &_s_ra_header_ct_val_html);

				nxs_rest_api_page_std(
				        req, NXS_REST_API_FORMAT_ERR_HTML, NXS_HTTP_CODE_400_BAD_REQUEST, (u_char *)"Unknown format");

				return;
			}
		}
	}

	if((auth_token = nxs_rest_api_get_req_args_find(req, &_s_ra_arg_key_token)) == NULL) {

		nxs_rest_api_page_std(req, content_type, NXS_HTTP_CODE_403_FORBIDDEN, (u_char *)"");

		return;
	}

	if(nxs_string_cmp(&nxs_chat_srv_cfg.statistic.auth_token, 0, auth_token, 0) == NXS_NO) {

		nxs_rest_api_page_std(req, content_type, NXS_HTTP_CODE_403_FORBIDDEN, (u_char *)"");

		return;
	}

	nxs_log_write_debug(
	        &process, "[%s]: received statistic request (auth token: \"%r\" (correct))", nxs_proc_get_name(&process), auth_token);

	statistic_ctx = nxs_chat_srv_u_db_statistic_init();
	cache_ctx     = nxs_chat_srv_u_db_cache_init();

	nxs_chat_srv_c_user_ctx_init(&user_ctx);

	nxs_string_init(&stat_total_str);
	nxs_string_init(&stat_general_str);
	nxs_string_init(&time_str);
	nxs_string_init(&username);

	if(nxs_chat_srv_u_db_statistic_pull(statistic_ctx) == NXS_CHAT_SRV_E_ERR) {

		nxs_rest_api_page_std(req, content_type, NXS_HTTP_CODE_500_INTERNAL_SERVER_ERROR, (u_char *)"");

		goto error;
	}

	if(nxs_chat_srv_u_db_cache_pull(cache_ctx) == NXS_CHAT_SRV_E_ERR) {

		nxs_rest_api_page_std(req, content_type, NXS_HTTP_CODE_500_INTERNAL_SERVER_ERROR, (u_char *)"");

		goto error;
	}

	/*
	 * Prepare total statistic
	 */

	nxs_chat_srv_u_db_statistic_get_total(statistic_ctx, &stat_total);

	str_time(&time_str, stat_total.last_action);

	if(content_type == NXS_REST_API_FORMAT_ERR_JSON) {

		nxs_string_printf(&stat_total_str,
		                  "{"
		                  "\"count_rdmn_issue_create\":%zu,"
		                  "\"count_rdmn_issue_update\":%zu,"
		                  "\"count_tlgrm_session_destroy\":%zu,"
		                  "\"count_tlgrm_create_issue\":%zu,"
		                  "\"count_tlgrm_reply_comment\":%zu,"
		                  "\"count_tlgrm_reply_empty\":%zu,"
		                  "\"count_tlgrm_reply_ext\":%zu,"
		                  "\"last_action\":%zu"
		                  "}",
		                  stat_total.count_rdmn_issue_create,
		                  stat_total.count_rdmn_issue_update,
		                  stat_total.count_tlgrm_session_destroy,
		                  stat_total.count_tlgrm_create_issue,
		                  stat_total.count_tlgrm_reply_comment,
		                  stat_total.count_tlgrm_reply_empty,
		                  stat_total.count_tlgrm_reply_ext,
		                  stat_total.last_action);
	}
	else {

		nxs_string_printf(&stat_total_str,
		                  "<tr>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "</tr>"
		                  "<tr>"
		                  "<td align=\"center\">%zu</td>"
		                  "<td align=\"center\">%zu</td>"
		                  "<td align=\"center\">%zu</td>"
		                  "<td align=\"center\">%zu</td>"
		                  "<td align=\"center\">%zu</td>"
		                  "<td align=\"center\">%zu</td>"
		                  "<td align=\"center\">%zu</td>"
		                  "<td align=\"center\">%r</td>"
		                  "</tr>",
		                  NXS_CHAT_SRV_STATISTIC_COUNT_RDMN_ISSUE_CREATE,
		                  NXS_CHAT_SRV_STATISTIC_COUNT_RDMN_ISSUE_UPDATE,
		                  NXS_CHAT_SRV_STATISTIC_COUNT_TLGRM_SESSION_DESTROY,
		                  NXS_CHAT_SRV_STATISTIC_COUNT_TLGRM_CREATE_ISSUE,
		                  NXS_CHAT_SRV_STATISTIC_COUNT_TLGRM_REPLY_COMMENT,
		                  NXS_CHAT_SRV_STATISTIC_COUNT_TLGRM_REPLY_EMPTY,
		                  NXS_CHAT_SRV_STATISTIC_COUNT_TLGRM_REPLY_EXT,
		                  NXS_CHAT_SRV_STATISTIC_LAST_ACTION,
		                  stat_total.count_rdmn_issue_create,
		                  stat_total.count_rdmn_issue_update,
		                  stat_total.count_tlgrm_session_destroy,
		                  stat_total.count_tlgrm_create_issue,
		                  stat_total.count_tlgrm_reply_comment,
		                  stat_total.count_tlgrm_reply_empty,
		                  stat_total.count_tlgrm_reply_ext,
		                  &time_str);
	}

	/*
	 * Prepare statistic by users
	 */

	if(content_type == NXS_REST_API_FORMAT_ERR_HTML) {

		nxs_string_printf(&stat_general_str,
		                  "<tr>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "<td align=\"center\"><i>%s</i></td>"
		                  "</tr>",
		                  NXS_CHAT_SRV_STATISTIC_RDMN_USER_NAME,
		                  NXS_CHAT_SRV_STATISTIC_COUNT_RDMN_ISSUE_CREATE,
		                  NXS_CHAT_SRV_STATISTIC_COUNT_RDMN_ISSUE_UPDATE,
		                  NXS_CHAT_SRV_STATISTIC_COUNT_TLGRM_SESSION_DESTROY,
		                  NXS_CHAT_SRV_STATISTIC_COUNT_TLGRM_CREATE_ISSUE,
		                  NXS_CHAT_SRV_STATISTIC_COUNT_TLGRM_REPLY_COMMENT,
		                  NXS_CHAT_SRV_STATISTIC_COUNT_TLGRM_REPLY_EMPTY,
		                  NXS_CHAT_SRV_STATISTIC_COUNT_TLGRM_REPLY_EXT,
		                  NXS_CHAT_SRV_STATISTIC_LAST_ACTION);
	}

	for(i = 0; nxs_chat_srv_u_db_statistic_get_general(statistic_ctx, &stat_general, i) == NXS_CHAT_SRV_E_OK; i++) {

		if(nxs_chat_srv_u_db_cache_user_get_by_rdmn_id(cache_ctx, stat_general.rdmn_userid, &user_ctx) == NXS_CHAT_SRV_E_OK) {

			nxs_string_printf(&username, "%r %r", &user_ctx.r_userlname, &user_ctx.r_userfname);
		}
		else {

			nxs_string_printf(&username, "Unknown user name");
		}

		if(content_type == NXS_REST_API_FORMAT_ERR_JSON) {

			if(i > 0) {

				nxs_string_char_add_char(&stat_general_str, (u_char)',');
			}

			nxs_string_printf2_cat(&stat_general_str,
			                       "{"
			                       "\"rdmn_userid\":%zu,"
			                       "\"rdmn_user_name\":\"%r\","
			                       "\"count_rdmn_issue_create\":%zu,"
			                       "\"count_rdmn_issue_update\":%zu,"
			                       "\"count_tlgrm_session_destroy\":%zu,"
			                       "\"count_tlgrm_create_issue\":%zu,"
			                       "\"count_tlgrm_reply_comment\":%zu,"
			                       "\"count_tlgrm_reply_empty\":%zu,"
			                       "\"count_tlgrm_reply_ext\":%zu,"
			                       "\"last_action\":%zu"
			                       "}",
			                       stat_general.rdmn_userid,
			                       &username,
			                       stat_general.count_rdmn_issue_create,
			                       stat_general.count_rdmn_issue_update,
			                       stat_general.count_tlgrm_session_destroy,
			                       stat_general.count_tlgrm_create_issue,
			                       stat_general.count_tlgrm_reply_comment,
			                       stat_general.count_tlgrm_reply_empty,
			                       stat_general.count_tlgrm_reply_ext,
			                       stat_general.last_action);
		}
		else {

			str_time(&time_str, stat_general.last_action);

			nxs_string_printf2_cat(&stat_general_str,
			                       "<tr>"
			                       "<td align=\"center\"><a href=\"%r/users/%zu\">%r</a></td>"
			                       "<td align=\"center\">%zu</td>"
			                       "<td align=\"center\">%zu</td>"
			                       "<td align=\"center\">%zu</td>"
			                       "<td align=\"center\">%zu</td>"
			                       "<td align=\"center\">%zu</td>"
			                       "<td align=\"center\">%zu</td>"
			                       "<td align=\"center\">%zu</td>"
			                       "<td align=\"center\">%r</td>"
			                       "</tr>",
			                       &nxs_chat_srv_cfg.rdmn.host,
			                       stat_general.rdmn_userid,
			                       &username,
			                       stat_general.count_rdmn_issue_create,
			                       stat_general.count_rdmn_issue_update,
			                       stat_general.count_tlgrm_session_destroy,
			                       stat_general.count_tlgrm_create_issue,
			                       stat_general.count_tlgrm_reply_comment,
			                       stat_general.count_tlgrm_reply_empty,
			                       stat_general.count_tlgrm_reply_ext,
			                       &time_str);
		}
	}

	response = (nxs_string_t *)nxs_rest_api_get_out_buf(req);

	nxs_rest_api_set_req_http_status(req, NXS_HTTP_CODE_200_OK);

	if(content_type == NXS_REST_API_FORMAT_ERR_JSON) {

		nxs_string_printf(response, "{\"stat_total\":%r,\"stat_users\":[%r]}", &stat_total_str, &stat_general_str);
	}
	else {

		nxs_string_printf(response,
		                  "<h2>%s</h2><br/><table border=\"1\">%r</table><br /><h2>%s</h2><br/><table border=\"1\">%r</table>",
		                  NXS_CHAT_SRV_STATISTIC_TOTAL,
		                  &stat_total_str,
		                  NXS_CHAT_SRV_STATISTIC_BY_USER,
		                  &stat_general_str);
	}

error:

	statistic_ctx = nxs_chat_srv_u_db_statistic_free(statistic_ctx);
	cache_ctx     = nxs_chat_srv_u_db_cache_free(cache_ctx);

	nxs_chat_srv_c_user_ctx_free(&user_ctx);

	nxs_string_free(&stat_total_str);
	nxs_string_free(&stat_general_str);
	nxs_string_free(&time_str);
	nxs_string_free(&username);
}

/* Module internal (static) functions */

static void str_time(nxs_string_t *time_str, time_t date)
{
	struct tm *t;
	char       t_s[TIME_STR_LEN];

	t = localtime(&date);
	strftime(t_s, TIME_STR_LEN, FORMAT_TIME, t);

	nxs_string_printf(time_str, "%s", t_s);
}
