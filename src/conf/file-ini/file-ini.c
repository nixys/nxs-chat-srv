#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <conf/file-ini/file-ini.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t				process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static int nxs_chat_srv_conf_file_ini_prep(nxs_cfg_t cfg);
static int nxs_chat_srv_conf_file_ini_post(nxs_cfg_t cfg);

// clang-format off

/* Module initializations */

nxs_cfg_par_t cfg_par[] =
{
	/* project config file settings */

	{nxs_string("loglevel"),		&nxs_chat_srv_cfg.log.level,		NULL,			NXS_CFG_TYPE_INT,		0,	31,		NXS_CFG_REQUIRED_NO,	NXS_CFG_TWICE_DEFINITION_NO},
	{nxs_string("logpath"),			&nxs_chat_srv_cfg.log.path,		NULL,			NXS_CFG_TYPE_STRING,		0,	0,		NXS_CFG_REQUIRED_YES,	NXS_CFG_TWICE_DEFINITION_NO},

	NXS_CFG_NULL
};

/* Module global functions */

// clang-format on

void nxs_chat_srv_conf_file_ini_init(nxs_cfg_t *cfg, nxs_chat_srv_cfg_ctx_t *cfg_ctx)
{

	cfg->cfg_par       = cfg_par;
	cfg->prep_function = &nxs_chat_srv_conf_file_ini_prep;
	cfg->post_function = &nxs_chat_srv_conf_file_ini_post;

	nxs_string_init(&cfg->cfg_path);
}

nxs_chat_srv_err_t nxs_chat_srv_conf_file_ini_runtime(nxs_cfg_t cfg, nxs_chat_srv_cfg_ctx_t *cfg_ctx)
{

	nxs_string_cpy(&cfg.cfg_path, 0, &cfg_ctx->cfg_path, 0);

	if(nxs_cfg_read(&process, cfg) != NXS_CFG_CONF_OK) {

		return NXS_CHAT_SRV_E_ERR;
	}

	return NXS_CHAT_SRV_E_OK;
}

/* Module internal (static) functions */

/* Project config file prepare function */
static int nxs_chat_srv_conf_file_ini_prep(nxs_cfg_t cfg)
{

	/* init config values */

	return NXS_CFG_CONF_OK;
}

/* Project config file post function */

static int nxs_chat_srv_conf_file_ini_post(nxs_cfg_t cfg)
{

	/* check all defined config values and prepare program runtime (on error - NXS_CFG_CONF_ERROR value must be returned)*/

	return NXS_CFG_CONF_OK;
}

/* Project config file auxiliaries functions */

/*
 *
 * This code only for example purposes!
 * This functionality realized in 'NXS_CFG_TYPE_LIST_DISTINCT' config type.
 *
static int nxs_chat_srv_conf_file_ini_emails(nxs_string_t *opt, nxs_string_t *val, nxs_cfg_par_t *cfg_par)
{
        nxs_list_t	*var = nxs_cfg_get_val(cfg_par);
        nxs_string_t	*el, *p;

        for(p = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, var); p != NULL; p = nxs_list_ptr_next(var)){

                if(nxs_string_cmp(p, NXS_STRING_NO_OFFSET, val, NXS_STRING_NO_OFFSET) == NXS_STRING_CMP_EQ){

                        return NXS_CFG_CONF_OK;
                }
        }

        el = nxs_list_add_tail(var);

        nxs_string_init2(el, 0, nxs_string_str(val));

        return NXS_CFG_CONF_OK;
}
*/
