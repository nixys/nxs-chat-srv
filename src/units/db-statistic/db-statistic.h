#ifndef _INCLUDE_NXS_CHAT_SRV_U_DB_STATISTIC_H
#define _INCLUDE_NXS_CHAT_SRV_U_DB_STATISTIC_H

// clang-format off

/* Structs declarations */

struct nxs_chat_srv_u_db_statistic_s;

enum nxs_chat_srv_u_db_statistic_action_type_e
{
	NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_RDMN_ISSUE_CREATE,	/* Action: create new issue in rdmn */
	NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_RDMN_ISSUE_UPDATE,	/* Action: update issue in rdmn */
	NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_SESSION_DESTROY,	/* Action: destroy current session in tlgrm */
	NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_CREATE_ISSUE,	/* Action: create new issue in tlgrm */
	NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_REPLY_COMMENT,	/* Action: reply on received comment in Telegram in tlgrm */
	NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_REPLY_EMPTY,	/* Action: user write the comment to bot and then choose issue for posting the comment in tlgrm */
	NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_REPLY_EXT		/* Action: for '/ext' comments in tlgrm */
};

struct nxs_chat_srv_u_db_statistic_general_s
{
	size_t			rdmn_userid;
	size_t			count_rdmn_issue_create;
	size_t			count_rdmn_issue_update;
	size_t			count_tlgrm_session_destroy;
	size_t			count_tlgrm_create_issue;
	size_t			count_tlgrm_reply_comment;
	size_t			count_tlgrm_reply_empty;
	size_t			count_tlgrm_reply_ext;
	time_t			last_action;
};

struct nxs_chat_srv_u_db_statistic_total_s
{
	size_t			count_rdmn_issue_create;
	size_t			count_rdmn_issue_update;
	size_t			count_tlgrm_session_destroy;
	size_t			count_tlgrm_create_issue;
	size_t			count_tlgrm_reply_comment;
	size_t			count_tlgrm_reply_empty;
	size_t			count_tlgrm_reply_ext;
	time_t			last_action;
};

/* Prototypes */

nxs_chat_srv_u_db_statistic_t		*nxs_chat_srv_u_db_statistic_init		(void);
nxs_chat_srv_u_db_statistic_t		*nxs_chat_srv_u_db_statistic_free		(nxs_chat_srv_u_db_statistic_t *u_ctx);

nxs_chat_srv_err_t			nxs_chat_srv_u_db_statistic_add			(nxs_chat_srv_u_db_statistic_t *u_ctx, nxs_chat_srv_u_db_statistic_action_type_t action_type, size_t rdmn_userid);

nxs_chat_srv_err_t			nxs_chat_srv_u_db_statistic_pull		(nxs_chat_srv_u_db_statistic_t *u_ctx);

nxs_chat_srv_err_t			nxs_chat_srv_u_db_statistic_get_general		(nxs_chat_srv_u_db_statistic_t *u_ctx, nxs_chat_srv_u_db_statistic_general_t *stat_general, size_t i);
nxs_chat_srv_err_t			nxs_chat_srv_u_db_statistic_get_total		(nxs_chat_srv_u_db_statistic_t *u_ctx, nxs_chat_srv_u_db_statistic_total_t *stat_total);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_DB_STATISTIC_H */
