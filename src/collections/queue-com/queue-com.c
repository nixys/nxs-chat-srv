#ifndef _INCLUDE_NXS_CHAT_SRV_C_QUEUE_COM_C
#define _INCLUDE_NXS_CHAT_SRV_C_QUEUE_COM_C
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
extern		nxs_process_t				process;
extern		nxs_chat_srv_cfg_t		nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */

static nxs_string_t _s_par_com_type	= nxs_string("com_type");
static nxs_string_t _s_par_body		= nxs_string("body");

/* Module global functions */

// clang-format on

void nxs_chat_srv_c_queue_com_serialize(nxs_string_t *out_msg, nxs_chat_srv_m_queue_com_types_t type, nxs_string_t *body)
{

	if(out_msg == NULL || body == NULL) {

		return;
	}

	nxs_string_printf_dyn(out_msg, "{\"%r\":%d,\"%r\":\"%r\"}", &_s_par_com_type, type, &_s_par_body, body);
}

nxs_chat_srv_err_t nxs_chat_srv_c_queue_com_deserialize(nxs_buf_t *in_msg, nxs_chat_srv_m_queue_com_types_t *type, nxs_string_t *body)
{
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;
	nxs_chat_srv_err_t rc;

	if(in_msg == NULL || type == NULL || body == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_com_type,	type,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT_32,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_body,		body,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, in_msg) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "internal queue communications error: parse error");

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}

/* Module internal (static) functions */
