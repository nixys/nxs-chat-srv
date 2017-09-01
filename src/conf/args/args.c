#include <nxs-core/nxs-core.h>

// clang-format off

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <conf/args/args.h>

/* Definitions */

#define NXS_CHAT_SRV_CONF_ARGS_HELP_MSG			"nxs-chat-srv short description \n" \
										"Available options:\n" \
										"\t-v: show program version\n" \
										"\t-h: show help (this message)\n" \
										"\t-c CONFIG_PATH: change default config file path (default path: %s )"

/* Project globals */
extern		nxs_process_t				process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static int nxs_chat_srv_conf_args_prep(nxs_args_t args, int argc, u_char **argv);
static int nxs_chat_srv_conf_args_post(nxs_args_t args, int argc, u_char **argv);
static int nxs_chat_srv_conf_args_freeargs(nxs_args_t args, int argc, int argind, u_char **argv);
static int nxs_chat_srv_conf_args_help(nxs_args_t args, u_char arg, u_char *optarg);
static int nxs_chat_srv_conf_args_version(nxs_args_t args, u_char arg, u_char *optarg);
static int nxs_chat_srv_conf_args_conf(nxs_args_t args, u_char arg, u_char *optarg);

// clang-format off

/* Module initializations */

nxs_args_shortopt_t shortopts[] =
{
	{'h',	NXS_ARGS_HAVE_ARGS_NO,		&nxs_chat_srv_conf_args_help},
	{'v',	NXS_ARGS_HAVE_ARGS_NO,		&nxs_chat_srv_conf_args_version},
	{'c',	NXS_ARGS_HAVE_ARGS_YES,		&nxs_chat_srv_conf_args_conf},

	NXS_ARGS_NULL
};

/* Module global functions */

// clang-format on

void nxs_chat_srv_conf_args_init(nxs_args_t *args, void *user_ctx)
{

	nxs_string_init(&args->help);
	nxs_string_printf(&args->help, NXS_CHAT_SRV_CONF_ARGS_HELP_MSG, NXS_CHAT_SRV_CONF_PATH_DEFAULT);

	args->prep_function     = &nxs_chat_srv_conf_args_prep;
	args->post_function     = &nxs_chat_srv_conf_args_post;
	args->freeargs_function = &nxs_chat_srv_conf_args_freeargs;

	args->args_s_els = shortopts;

	args->ctx = user_ctx;
}

nxs_chat_srv_err_t nxs_chat_srv_conf_args_runtime(nxs_args_t args, int argc, u_char **argv)
{

	switch(nxs_args_handler(&process, args, argc, argv)) {

		case NXS_ARGS_CONF_ERROR:

			return NXS_CHAT_SRV_E_ERR;

		case NXS_ARGS_CONF_OK_EXIT:

			return NXS_CHAT_SRV_E_EXIT;
	}

	return NXS_CHAT_SRV_E_OK;
}

/* Module internal (static) functions */

/* Project command line prepare function */
static int nxs_chat_srv_conf_args_prep(nxs_args_t args, int argc, u_char **argv)
{

	/* init all command line values */

	return NXS_ARGS_CONF_OK;
}

/* Project command line post function */
static int nxs_chat_srv_conf_args_post(nxs_args_t args, int argc, u_char **argv)
{

	/* checking for defines and correct command line values (on error - NXS_ARGS_CONF_ERROR value must be returned)*/

	return NXS_ARGS_CONF_OK;
}

/* Project command line freeargs function */
static int nxs_chat_srv_conf_args_freeargs(nxs_args_t args, int argc, int argind, u_char **argv)
{

	/* processing all free command line arguments */

	/*
	int i;

	for(i = argind; i < argc; i++){

	        switch(i - argind){

	                case POSITION_1:


	                        break;

	                case POSITION_2:


	                        break;

	                default:

	                        nxs_log_write_error(&process, "too many arguments");

	                        return NXS_ARGS_CONF_ERROR;
	        }
	}
	*/

	return NXS_ARGS_CONF_OK;
}

static int nxs_chat_srv_conf_args_help(nxs_args_t args, u_char arg, u_char *optarg)
{

	nxs_log_write_info(&process, "%s", nxs_string_str(&args.help));

	return NXS_ARGS_CONF_OK_EXIT;
}

static int nxs_chat_srv_conf_args_version(nxs_args_t args, u_char arg, u_char *optarg)
{

	nxs_log_write_info(&process, "version: %s (nxs-fw v%s)", NXS_CHAT_SRV_VERSION, NXS_VERSION);

	return NXS_ARGS_CONF_OK_EXIT;
}

static int nxs_chat_srv_conf_args_conf(nxs_args_t args, u_char arg, u_char *optarg)
{
	nxs_chat_srv_cfg_ctx_t *cfg_ctx = nxs_args_get_ctx(args);

	nxs_string_char_cpy(&cfg_ctx->cfg_path, 0, optarg);

	return NXS_ARGS_CONF_OK;
}
