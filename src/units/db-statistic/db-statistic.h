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
	NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_ISSUE_CREATE,	/* Action: create new issue in tlgrm */
	NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_MESSAGE_REPLIED,	/* Action: reply on received comment in tlgrm */
	NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_MESSAGE_CREATED,	/* Action: user write the comment to bot and then choose issue for posting the comment in tlgrm */
	NXS_CHAT_SRV_U_DB_STATISTIC_ACTION_TYPE_TLGRM_MESSAGE_EXTENDED	/* Action: for '/ext' comments in tlgrm */
};

struct nxs_chat_srv_u_db_statistic_general_s
{
	size_t			rdmn_userid;
	size_t			count_rdmn_issues_created;
	size_t			count_rdmn_issues_updated;
	size_t			count_tlgrm_sessions_destroyed;
	size_t			count_tlgrm_issues_created;
	size_t			count_tlgrm_messages_created;
	size_t			count_tlgrm_messages_replied;
	size_t			count_tlgrm_messages_extended;
	double			tlgrm_issues_replied_ratio;
	double			tlgrm_issues_created_ratio;
	time_t			last_action_timestamp;
};

struct nxs_chat_srv_u_db_statistic_total_s
{
	size_t			count_rdmn_issues_created;
	size_t			count_rdmn_issues_updated;
	size_t			count_tlgrm_sessions_destroyed;
	size_t			count_tlgrm_issues_created;
	size_t			count_tlgrm_messages_created;
	size_t			count_tlgrm_messages_replied;
	size_t			count_tlgrm_messages_extended;
	double			tlgrm_issues_replied_ratio;
	double			tlgrm_issues_created_ratio;
	time_t			last_action_timestamp;
};

/* Prototypes */

nxs_chat_srv_u_db_statistic_t		*nxs_chat_srv_u_db_statistic_init		(void);
nxs_chat_srv_u_db_statistic_t		*nxs_chat_srv_u_db_statistic_free		(nxs_chat_srv_u_db_statistic_t *u_ctx);

nxs_chat_srv_err_t			nxs_chat_srv_u_db_statistic_add			(nxs_chat_srv_u_db_statistic_t *u_ctx, nxs_chat_srv_u_db_statistic_action_type_t action_type, size_t rdmn_userid);

nxs_chat_srv_err_t			nxs_chat_srv_u_db_statistic_pull		(nxs_chat_srv_u_db_statistic_t *u_ctx);

nxs_chat_srv_err_t			nxs_chat_srv_u_db_statistic_get_general		(nxs_chat_srv_u_db_statistic_t *u_ctx, nxs_chat_srv_u_db_statistic_general_t *stat_general, size_t i);
nxs_chat_srv_err_t			nxs_chat_srv_u_db_statistic_get_total		(nxs_chat_srv_u_db_statistic_t *u_ctx, nxs_chat_srv_u_db_statistic_total_t *stat_total);

#endif /* _INCLUDE_NXS_CHAT_SRV_U_DB_STATISTIC_H */
