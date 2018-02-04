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

#define TIME_STR_LEN			50
#define FORMAT_TIME			"%Y-%m-%d %H:%M:%S %z"

#define _nxs_label(label_key)		nxs_chat_srv_u_labels_compile_key( \
						labels_ctx, lang, label_key)

/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static void str_time(nxs_string_t *time_str, time_t date);

static void nxs_chat_srv_p_rest_api_statistic_make_html(nxs_string_t *                 response,
                                                        nxs_chat_srv_u_db_statistic_t *statistic_ctx,
                                                        nxs_chat_srv_u_db_cache_t *    cache_ctx,
                                                        nxs_string_t *                 lang);
static void nxs_chat_srv_p_rest_api_statistic_make_json(nxs_string_t *                 response,
                                                        nxs_chat_srv_u_db_statistic_t *statistic_ctx,
                                                        nxs_chat_srv_u_db_cache_t *    cache_ctx,
                                                        nxs_string_t *                 lang);

// clang-format off

/* Module initializations */

static nxs_string_t _s_ra_header_ct_key			= nxs_string("Content-Type");
static nxs_string_t _s_ra_header_ct_val_json		= nxs_string("application/json; charset=utf-8");
static nxs_string_t _s_ra_header_ct_val_html		= nxs_string("text/html; charset=utf-8");

static nxs_string_t _s_ra_arg_key_token			= nxs_string("token");
static nxs_string_t _s_ra_arg_key_format		= nxs_string("format");
static nxs_string_t _s_ra_arg_key_lang			= nxs_string("lang");
static nxs_string_t _s_ra_arg_val_json			= nxs_string("json");
static nxs_string_t _s_ra_arg_val_html			= nxs_string("html");

/* Module global functions */

// clang-format on

void nxs_chat_srv_p_rest_api_statistic_handler_get(nxs_rest_api_ctx_t *rest_api_ctx, nxs_rest_api_request_t *req, void *custom_ctx)
{
	nxs_chat_srv_u_db_statistic_t *statistic_ctx;
	nxs_chat_srv_u_db_cache_t *    cache_ctx;
	nxs_string_t *                 auth_token, *format_key, *response, *lang;
	nxs_rest_api_format_err_t      content_type;

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

	if((lang = nxs_rest_api_get_req_args_find(req, &_s_ra_arg_key_lang)) == NULL) {

		lang = &nxs_chat_srv_cfg.labels.default_lang;
	}

	nxs_log_write_debug(
	        &process, "[%s]: received statistic request (auth token: \"%r\" (correct))", nxs_proc_get_name(&process), auth_token);

	statistic_ctx = nxs_chat_srv_u_db_statistic_init();
	cache_ctx     = nxs_chat_srv_u_db_cache_init();

	if(nxs_chat_srv_u_db_statistic_pull(statistic_ctx) == NXS_CHAT_SRV_E_ERR) {

		nxs_rest_api_page_std(req, content_type, NXS_HTTP_CODE_500_INTERNAL_SERVER_ERROR, (u_char *)"");

		goto error;
	}

	if(nxs_chat_srv_u_db_cache_pull(cache_ctx) == NXS_CHAT_SRV_E_ERR) {

		nxs_rest_api_page_std(req, content_type, NXS_HTTP_CODE_500_INTERNAL_SERVER_ERROR, (u_char *)"");

		goto error;
	}

	response = (nxs_string_t *)nxs_rest_api_get_out_buf(req);

	nxs_rest_api_set_req_http_status(req, NXS_HTTP_CODE_200_OK);

	if(content_type == NXS_REST_API_FORMAT_ERR_JSON) {

		nxs_chat_srv_p_rest_api_statistic_make_json(response, statistic_ctx, cache_ctx, lang);
	}
	else {

		nxs_chat_srv_p_rest_api_statistic_make_html(response, statistic_ctx, cache_ctx, lang);
	}

error:

	statistic_ctx = nxs_chat_srv_u_db_statistic_free(statistic_ctx);
	cache_ctx     = nxs_chat_srv_u_db_cache_free(cache_ctx);
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

static void nxs_chat_srv_p_rest_api_statistic_make_html(nxs_string_t *                 response,
                                                        nxs_chat_srv_u_db_statistic_t *statistic_ctx,
                                                        nxs_chat_srv_u_db_cache_t *    cache_ctx,
                                                        nxs_string_t *                 lang)
{
	nxs_chat_srv_u_db_statistic_total_t   stat_total;
	nxs_chat_srv_u_db_statistic_general_t stat_general;
	nxs_chat_srv_u_labels_t *             labels_ctx;
	nxs_chat_srv_m_user_ctx_t             user_ctx;
	nxs_string_t                          time_str, stat_total_tbody_str, stat_users_tbody_str, username;
	size_t                                i;

	labels_ctx = nxs_chat_srv_u_labels_init();

	nxs_string_init_empty(&time_str);
	nxs_string_init_empty(&stat_total_tbody_str);
	nxs_string_init_empty(&stat_users_tbody_str);
	nxs_string_init_empty(&username);

	nxs_chat_srv_c_user_ctx_init(&user_ctx);

	nxs_chat_srv_u_db_statistic_get_total(statistic_ctx, &stat_total);

	str_time(&time_str, stat_total.last_action);

	/*
	 * Prepare summary statistic table body
	 */

	nxs_string_printf(&stat_total_tbody_str,
	                  "				<tr>\n"
	                  "					<th width=\"7%\"></th>\n"
	                  "					<th>%r</th>\n"
	                  "					<th>%r</th>\n"
	                  "					<th>%r</th>\n"
	                  "					<th>%r</th>\n"
	                  "					<th>%r</th>\n"
	                  "					<th>%r</th>\n"
	                  "					<th>%r</th>\n"
	                  "					<th>%r</th>\n"
	                  "				</tr>\n"
	                  "				<tr>\n"
	                  "					<td></td>\n"
	                  "					<td>%zu</td>\n"
	                  "					<td>%zu</td>\n"
	                  "					<td>%zu</td>\n"
	                  "					<td>%zu</td>\n"
	                  "					<td>%zu</td>\n"
	                  "					<td>%zu</td>\n"
	                  "					<td>%zu</td>\n"
	                  "					<td>%r</td>\n"
	                  "				</tr>\n",
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_RDMN_COUNT_ISSUE_CREATED),
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_RDMN_COUNT_ISSUE_UPDATED),
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_TLGRM_SESSIONS_DESTROYED),
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_TLGRM_COUNT_ISSUES_CREATED),
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_TLGRM_COUNT_MESSAGES_REPLIED),
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_TLGRM_COUNT_MESSAGES_CREATED),
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_TLGRM_COUNT_EXT_MESSAGES),
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_LAST_ACTION_DATE),
	                  stat_total.count_rdmn_issue_create,
	                  stat_total.count_rdmn_issue_update,
	                  stat_total.count_tlgrm_session_destroy,
	                  stat_total.count_tlgrm_create_issue,
	                  stat_total.count_tlgrm_reply_comment,
	                  stat_total.count_tlgrm_reply_empty,
	                  stat_total.count_tlgrm_reply_ext,
	                  &time_str);

	/*
	 * Prepare statistic by users
	 */

	nxs_string_printf(&stat_users_tbody_str,
	                  "				<tr>\n"
	                  "					<th width=\"7%\">%r</th>\n"
	                  "					<th>%r</th>\n"
	                  "					<th>%r</th>\n"
	                  "					<th>%r</th>\n"
	                  "					<th>%r</th>\n"
	                  "					<th>%r</th>\n"
	                  "					<th>%r</th>\n"
	                  "					<th>%r</th>\n"
	                  "					<th>%r</th>\n"
	                  "				</tr>\n",
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_USERNAME),
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_RDMN_COUNT_ISSUE_CREATED),
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_RDMN_COUNT_ISSUE_UPDATED),
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_TLGRM_SESSIONS_DESTROYED),
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_TLGRM_COUNT_ISSUES_CREATED),
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_TLGRM_COUNT_MESSAGES_REPLIED),
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_TLGRM_COUNT_MESSAGES_CREATED),
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_TLGRM_COUNT_EXT_MESSAGES),
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_FIELD_LAST_ACTION_DATE));

	for(i = 0; nxs_chat_srv_u_db_statistic_get_general(statistic_ctx, &stat_general, i) == NXS_CHAT_SRV_E_OK; i++) {

		if(nxs_chat_srv_u_db_cache_user_get_by_rdmn_id(cache_ctx, stat_general.rdmn_userid, &user_ctx) == NXS_CHAT_SRV_E_OK) {

			nxs_string_printf(&username, "%r %r", &user_ctx.r_userlname, &user_ctx.r_userfname);
		}
		else {

			nxs_string_printf(&username, "Unknown user name");
		}

		str_time(&time_str, stat_general.last_action);

		nxs_string_printf2_cat(&stat_users_tbody_str,
		                       "				<tr>\n"
		                       "					<td><a href=\"%r/users/%zu\">%r</a></td>\n"
		                       "					<td>%zu</td>\n"
		                       "					<td>%zu</td>\n"
		                       "					<td>%zu</td>\n"
		                       "					<td>%zu</td>\n"
		                       "					<td>%zu</td>\n"
		                       "					<td>%zu</td>\n"
		                       "					<td>%zu</td>\n"
		                       "					<td>%r</td>\n"
		                       "				</tr>\n",
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

	/*
	 * Compile response
	 */

	nxs_string_printf(response,
	                  "<html>\n"
	                  "	<head>\n"
	                  "		<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n"
	                  "		<link href=\"https://fonts.googleapis.com/css?family=Roboto+Condensed:300\" rel=\"stylesheet\">\n"
	                  "		<style>\n"
	                  "			body {\n"
	                  "				font-family: 'Roboto Condensed', sans-serif;\n"
	                  "				padding: 50px 100px;\n"
	                  "				color: #666;\n"
	                  "			}\n"
	                  "			h2 {\n"
	                  "				font-size: 40px;\n"
	                  "				margin-bottom: 0;\n"
	                  "				color: #587087;\n"
	                  "				text-align: center;\n"
	                  "			}\n"
	                  "			table, body {\n"
	                  "				font-size: 15px;\n"
	                  "			}\n"
	                  "			table {\n"
	                  "				text-align: center;\n"
	                  "				text-shadow: 1px 1px #fff;\n"
	                  "			}\n"
	                  "			table td, table th {\n"
	                  "				padding: 5px 18px;\n"
	                  "				//border-left: 1px solid #fff;\n"
	                  "				border-bottom: 1px solid #d2dbe4;\n"
	                  "				border-top: 1px solid #fdfeff;\n"
	                  "				color: #000;\n"
	                  "			}\n"
	                  "			table td {\n"
	                  "				background: #f4f7fb;\n"
	                  "			}\n"
	                  "			table th {\n"
	                  "				font-weight: 300;\n"
	                  "				background: #e2e8f0;\n"
	                  "			}\n"
	                  "			a {\n"
	                  "				color: #00398e;\n"
	                  "				text-decoration: none;\n"
	                  "				font-weight: bold;\n"
	                  "				transition: all .2s ease;\n"
	                  "				position: relative;\n"
	                  "			}\n"
	                  "			a:hover {\n"
	                  "				color: #8d001a;\n"
	                  "				text-decoration: none;\n"
	                  "			}\n"
	                  "			a:hover:before {\n"
	                  "				position: absolute;\n"
	                  "				left: -15px;\n"
	                  "			}\n"
	                  "		</style>\n"
	                  "	</head>\n"
	                  "	<body>\n"
	                  "		<h2>%r</h2>\n"
	                  "		<br>\n"
	                  "		<table cellpadding=\"0\" cellspacing=\"0\">\n"
	                  "			<tbody>\n"
	                  "%r"
	                  "			</tbody>\n"
	                  "		</table>\n"
	                  "		<br>\n"
	                  "		<h2>%r</h2>\n"
	                  "		<br>\n"
	                  "		<table cellpadding=\"0\" cellspacing=\"0\">\n"
	                  "			<tbody>\n"
	                  "%r"
	                  "			</tbody>\n"
	                  "		</table>\n"
	                  "	</body>\n"
	                  "</html>\n",
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_BY_USER),
	                  &stat_total_tbody_str,
	                  _nxs_label(NXS_CHAT_SRV_U_LABELS_KEY_STATISTIC_SUMMARY),
	                  &stat_users_tbody_str);

	nxs_string_free(&time_str);
	nxs_string_free(&stat_total_tbody_str);
	nxs_string_free(&stat_users_tbody_str);
	nxs_string_free(&username);

	labels_ctx = nxs_chat_srv_u_labels_free(labels_ctx);

	nxs_chat_srv_c_user_ctx_free(&user_ctx);
}

static void nxs_chat_srv_p_rest_api_statistic_make_json(nxs_string_t *                 response,
                                                        nxs_chat_srv_u_db_statistic_t *statistic_ctx,
                                                        nxs_chat_srv_u_db_cache_t *    cache_ctx,
                                                        nxs_string_t *                 lang)
{

	nxs_chat_srv_u_db_statistic_total_t   stat_total;
	nxs_chat_srv_u_db_statistic_general_t stat_general;
	nxs_chat_srv_m_user_ctx_t             user_ctx;
	nxs_string_t                          stat_total_str, stat_users_str, username;
	size_t                                i;

	nxs_string_init_empty(&stat_total_str);
	nxs_string_init_empty(&stat_users_str);
	nxs_string_init_empty(&username);

	nxs_chat_srv_c_user_ctx_init(&user_ctx);

	nxs_chat_srv_u_db_statistic_get_total(statistic_ctx, &stat_total);

	/*
	 * Prepare summary statistic table body
	 */

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

	/*
	 * Prepare statistic by users
	 */

	for(i = 0; nxs_chat_srv_u_db_statistic_get_general(statistic_ctx, &stat_general, i) == NXS_CHAT_SRV_E_OK; i++) {

		if(nxs_chat_srv_u_db_cache_user_get_by_rdmn_id(cache_ctx, stat_general.rdmn_userid, &user_ctx) == NXS_CHAT_SRV_E_OK) {

			nxs_string_printf(&username, "%r %r", &user_ctx.r_userlname, &user_ctx.r_userfname);
		}
		else {

			nxs_string_printf(&username, "Unknown user name");
		}

		if(i > 0) {

			nxs_string_char_add_char(&stat_users_str, (u_char)',');
		}

		nxs_string_printf2_cat(&stat_users_str,
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

	/*
	 * Compile response
	 */

	nxs_string_printf(response, "{\"stat_total\":%r,\"stat_users\":[%r]}", &stat_total_str, &stat_users_str);

	nxs_string_free(&stat_total_str);
	nxs_string_free(&stat_users_str);
	nxs_string_free(&username);

	nxs_chat_srv_c_user_ctx_free(&user_ctx);
}
