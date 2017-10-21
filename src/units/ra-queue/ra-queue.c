#ifndef _INCLUDE_NXS_CHAT_SRV_U_RA_QUEUE_C
#define _INCLUDE_NXS_CHAT_SRV_U_RA_QUEUE_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/ra-queue/ra-queue-typedefs.h>
#include <units/ra-queue/ra-queue.h>
#include <units/ra-queue/ra-queue-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

struct nxs_chat_srv_u_ra_queue_s
{
	nxs_string_t			buf;
	nxs_chat_srv_d_ra_queue_t	*ra_queue_ctx;
};

/* Module internal (static) functions prototypes */

// clang-format on

static void nxs_chat_srv_u_ra_queue_serialize(nxs_chat_srv_u_ra_queue_t *u_ctx, nxs_chat_srv_m_ra_queue_type_t type, nxs_string_t *data);
nxs_chat_srv_err_t
        nxs_chat_srv_u_ra_queue_deserialize(nxs_chat_srv_u_ra_queue_t *u_ctx, nxs_chat_srv_m_ra_queue_type_t *type, nxs_string_t *data);

// clang-format off

/* Module initializations */

static nxs_string_t _s_par_type		= nxs_string("type");
static nxs_string_t _s_par_data		= nxs_string("data");

/* Module global functions */

// clang-format on

nxs_chat_srv_u_ra_queue_t *nxs_chat_srv_u_ra_queue_init(void)
{
	nxs_chat_srv_u_ra_queue_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_ra_queue_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_ra_queue_t));

	nxs_string_init(&u_ctx->buf);

	u_ctx->ra_queue_ctx = nxs_chat_srv_d_ra_queue_init();

	return u_ctx;
}

nxs_chat_srv_u_ra_queue_t *nxs_chat_srv_u_ra_queue_free(nxs_chat_srv_u_ra_queue_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	nxs_string_free(&u_ctx->buf);

	u_ctx->ra_queue_ctx = nxs_chat_srv_d_ra_queue_free(u_ctx->ra_queue_ctx);

	return (nxs_chat_srv_u_ra_queue_t *)nxs_free(u_ctx);
}

nxs_chat_srv_err_t nxs_chat_srv_u_ra_queue_add(nxs_chat_srv_u_ra_queue_t *u_ctx, nxs_chat_srv_m_ra_queue_type_t type, nxs_string_t *data)
{

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	nxs_chat_srv_u_ra_queue_serialize(u_ctx, type, data);

	return nxs_chat_srv_d_ra_queue_add(u_ctx->ra_queue_ctx, &u_ctx->buf);
}

nxs_chat_srv_err_t nxs_chat_srv_u_ra_queue_get(nxs_chat_srv_u_ra_queue_t *u_ctx, nxs_chat_srv_m_ra_queue_type_t *type, nxs_string_t *data)
{
	nxs_chat_srv_err_t rc;

	if(u_ctx == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	switch(nxs_chat_srv_d_ra_queue_get(u_ctx->ra_queue_ctx, &u_ctx->buf)) {

		case NXS_CHAT_SRV_E_EXIST:

			nxs_error(rc, NXS_CHAT_SRV_E_EXIST, error);

			break;

		case NXS_CHAT_SRV_E_OK:

			break;

		default:

			nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

	rc = nxs_chat_srv_u_ra_queue_deserialize(u_ctx, type, data);

error:

	return rc;
}

/* Module internal (static) functions */

static void nxs_chat_srv_u_ra_queue_serialize(nxs_chat_srv_u_ra_queue_t *u_ctx, nxs_chat_srv_m_ra_queue_type_t type, nxs_string_t *data)
{
	nxs_string_t data_encoded;

	nxs_string_init(&data_encoded);

	nxs_base64_encode_string(&data_encoded, data);

	nxs_string_printf(&u_ctx->buf, "{\"%r\":%d,\"%r\":\"%r\"}", &_s_par_type, type, &_s_par_data, &data_encoded);

	nxs_string_free(&data_encoded);
}

nxs_chat_srv_err_t
        nxs_chat_srv_u_ra_queue_deserialize(nxs_chat_srv_u_ra_queue_t *u_ctx, nxs_chat_srv_m_ra_queue_type_t *type, nxs_string_t *data)
{
	nxs_cfg_json_t     cfg_json;
	nxs_array_t        cfg_arr;
	nxs_chat_srv_err_t rc;

	if(u_ctx == NULL || type == NULL || data == NULL) {

		return NXS_CHAT_SRV_E_PTR;
	}

	rc = NXS_CHAT_SRV_E_OK;

	nxs_cfg_json_conf_array_init(&cfg_arr);

	// clang-format off

	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_type,	type,	NULL,	NULL,	NXS_CFG_JSON_TYPE_INT_32,	0,	0,	NXS_YES,	NULL);
	nxs_cfg_json_conf_array_add(&cfg_arr,	&_s_par_data,	data,	NULL,	NULL,	NXS_CFG_JSON_TYPE_STRING,	0,	0,	NXS_YES,	NULL);

	// clang-format on

	nxs_cfg_json_init(&process, &cfg_json, NULL, NULL, NULL, &cfg_arr);

	if(nxs_cfg_json_read_buf(&process, cfg_json, (nxs_buf_t *)&u_ctx->buf) != NXS_CFG_JSON_CONF_OK) {

		nxs_log_write_raw(&process, "rest-api queue error: parse error");

		nxs_error(rc, NXS_CHAT_SRV_E_ERR, error);
	}

error:

	nxs_cfg_json_free(&cfg_json);

	nxs_cfg_json_conf_array_free(&cfg_arr);

	return rc;
}
