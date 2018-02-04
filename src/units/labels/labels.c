#ifndef _INCLUDE_NXS_CHAT_SRV_U_LABELS_C
#define _INCLUDE_NXS_CHAT_SRV_U_LABELS_C
#endif

// clang-format off

#include <nxs-core/nxs-core.h>

/* Project core include */
#include <nxs-chat-srv-core.h>
#include <nxs-chat-srv-meta.h>
#include <nxs-chat-srv-collections.h>
#include <nxs-chat-srv-dal.h>

#include <units/labels/labels-typedefs.h>
#include <units/labels/labels.h>
#include <units/labels/labels-subunits.h>

/* Definitions */



/* Project globals */
extern		nxs_process_t		process;
extern		nxs_chat_srv_cfg_t	nxs_chat_srv_cfg;

/* Module typedefs */



/* Module declarations */

typedef struct
{
	nxs_string_t				name;
	nxs_string_t				value;
} nxs_chat_srv_u_labels_variable_t;

struct nxs_chat_srv_u_labels_s
{
	nxs_array_t				variables;	/* type: nxs_chat_srv_u_labels_variable_t */
	nxs_array_t				results;	/* type: nxs_string_t *. Array contains results for all labels compilations */
	nxs_string_t				t_name;		/* Temp variable for full variable name (i.e. "%{...}") */
};

/* Module internal (static) functions prototypes */

// clang-format on

static void nxs_chat_srv_u_labels_variable_init(void *element);
static void nxs_chat_srv_u_labels_variable_free(void *element);

static void nxs_chat_srv_u_labels_result_init(void *element);
static void nxs_chat_srv_u_labels_result_free(void *element);

static nxs_string_t *nxs_chat_srv_u_labels_get_var_value(nxs_chat_srv_u_labels_t *u_ctx, u_char *var_name);

static nxs_chat_srv_cfg_labels_set_t *nxs_chat_srv_u_labels_cfg_get_labels_set(nxs_string_t *lang);
static nxs_string_t *nxs_chat_srv_u_labels_cfg_get_label_text(nxs_chat_srv_cfg_labels_set_t *labels_set, u_char *label_key);
static nxs_string_t *nxs_chat_srv_u_labels_cfg_get_greeting_text(nxs_string_t *lang);

// clang-format off

/* Module initializations */

static nxs_string_t	_s_empty_string			= nxs_string("");

/* Module global functions */

// clang-format on

nxs_chat_srv_u_labels_t *nxs_chat_srv_u_labels_init(void)
{
	nxs_chat_srv_u_labels_t *u_ctx;

	u_ctx = (nxs_chat_srv_u_labels_t *)nxs_malloc(NULL, sizeof(nxs_chat_srv_u_labels_t));

	nxs_string_init_empty(&u_ctx->t_name);

	nxs_array_init3(&u_ctx->variables,
	                nxs_chat_srv_u_labels_variable_t,
	                &nxs_chat_srv_u_labels_variable_init,
	                &nxs_chat_srv_u_labels_variable_free);

	/*
	 * This array has type "nxs_string_t *" for safety use returned pointers to compiled
	 * labels (e.g. as arguments of same call nxs_string_printf).
	 * Because pointers to array elements may be wrong when array reallocated.
	 */
	nxs_array_init3(&u_ctx->results, nxs_string_t *, &nxs_chat_srv_u_labels_result_init, &nxs_chat_srv_u_labels_result_free);

	return u_ctx;
}

nxs_chat_srv_u_labels_t *nxs_chat_srv_u_labels_free(nxs_chat_srv_u_labels_t *u_ctx)
{

	if(u_ctx == NULL) {

		return NULL;
	}

	nxs_string_free(&u_ctx->t_name);

	nxs_array_free(&u_ctx->variables);
	nxs_array_free(&u_ctx->results);

	return (nxs_chat_srv_u_labels_t *)nxs_free(u_ctx);
}

void nxs_chat_srv_u_labels_variable_add(nxs_chat_srv_u_labels_t *u_ctx, const char *var_name, const char *value, ...)
{
	nxs_chat_srv_u_labels_variable_t *v;
	nxs_string_t *                    s;
	va_list                           msg_args;

	if(u_ctx == NULL) {

		return;
	}

	if((s = nxs_chat_srv_u_labels_get_var_value(u_ctx, (u_char *)var_name)) == NULL) {

		/* if variable name does not exist */

		v = nxs_array_add(&u_ctx->variables);

		nxs_string_char_cpy(&v->name, 0, (u_char *)var_name);

		s = &v->value;
	}

	va_start(msg_args, value);
	nxs_string_vprintf(s, value, msg_args);
	va_end(msg_args);

	return;
}

void nxs_chat_srv_u_labels_variable_clear(nxs_chat_srv_u_labels_t *u_ctx)
{

	if(u_ctx == NULL) {

		return;
	}

	nxs_array_clear(&u_ctx->variables);
}

nxs_string_t *nxs_chat_srv_u_labels_compile_text(nxs_chat_srv_u_labels_t *u_ctx, nxs_string_t *text)
{
	nxs_chat_srv_u_labels_variable_t *v;
	nxs_string_t **                   r;
	size_t                            i;

	if(u_ctx == NULL || text == NULL) {

		return &_s_empty_string;
	}

	r = nxs_array_add(&u_ctx->results);

	nxs_string_clone(*r, text);

	for(i = 0; i < nxs_array_count(&u_ctx->variables); i++) {

		v = nxs_array_get(&u_ctx->variables, i);

		nxs_string_printf(&u_ctx->t_name, "%%{%r}", &v->name);

		nxs_string_subs(*r, NULL, &u_ctx->t_name, &v->value, 0);
	}

	return *r;
}

nxs_string_t *nxs_chat_srv_u_labels_compile_key(nxs_chat_srv_u_labels_t *u_ctx, nxs_string_t *lang, char *label_key)
{
	nxs_chat_srv_cfg_labels_set_t *l_set;
	nxs_string_t *                 text, **r;

	if(u_ctx == NULL || lang == NULL || label_key == NULL) {

		return &_s_empty_string;
	}

	l_set = nxs_chat_srv_u_labels_cfg_get_labels_set(lang);

	if((text = nxs_chat_srv_u_labels_cfg_get_label_text(l_set, (u_char *)label_key)) == NULL) {

		text = nxs_chat_srv_u_labels_cfg_get_label_text(nxs_chat_srv_cfg.labels.default_labels_set, (u_char *)label_key);
	}

	if(text == NULL) {

		nxs_log_write_debug(&process,
		                    "[%s]: labels warn: can't find label text for specified key (label key: %s, specified lang: "
		                    "%r, default lang: %r)",
		                    nxs_proc_get_name(&process),
		                    label_key,
		                    nxs_string_len(lang) > 0 ? lang : &nxs_chat_srv_cfg.labels.default_lang,
		                    &nxs_chat_srv_cfg.labels.default_lang);

		r = nxs_array_add(&u_ctx->results);

		nxs_string_char_cpy(*r, 0, (u_char *)label_key);

		return *r;
	}

	return nxs_chat_srv_u_labels_compile_text(u_ctx, text);
}

nxs_string_t *nxs_chat_srv_u_labels_compile_greeting(nxs_chat_srv_u_labels_t *u_ctx, nxs_string_t *lang)
{
	nxs_string_t g_tmp, *r, *s;

	nxs_string_init(&g_tmp);

	if((s = nxs_chat_srv_u_labels_cfg_get_greeting_text(lang)) == NULL) {

		if((s = nxs_chat_srv_u_labels_cfg_get_greeting_text(&nxs_chat_srv_cfg.labels.default_lang)) == NULL) {

			nxs_string_char_cpy(&g_tmp, 0, (u_char *)NXS_CHAT_SRV_GREETING_DEFAULT);

			s = &g_tmp;
		}
	}

	r = nxs_chat_srv_u_labels_compile_text(u_ctx, s);

	nxs_string_free(&g_tmp);

	return r;
}

/* Module internal (static) functions */

static void nxs_chat_srv_u_labels_variable_init(void *element)
{
	nxs_chat_srv_u_labels_variable_t *var = element;

	nxs_string_init_empty(&var->name);
	nxs_string_init_empty(&var->value);
}

static void nxs_chat_srv_u_labels_variable_free(void *element)
{
	nxs_chat_srv_u_labels_variable_t *var = element;

	nxs_string_free(&var->name);
	nxs_string_free(&var->value);
}

static void nxs_chat_srv_u_labels_result_init(void *element)
{
	nxs_string_t **var = element;

	*var = nxs_string_malloc(0, NULL);
}

static void nxs_chat_srv_u_labels_result_free(void *element)
{
	nxs_string_t **var = element;

	*var = nxs_string_destroy(*var);
}

static nxs_string_t *nxs_chat_srv_u_labels_get_var_value(nxs_chat_srv_u_labels_t *u_ctx, u_char *var_name)
{
	nxs_chat_srv_u_labels_variable_t *v;
	size_t                            i;

	if(u_ctx == NULL) {

		return NULL;
	}

	for(i = 0; i < nxs_array_count(&u_ctx->variables); i++) {

		v = nxs_array_get(&u_ctx->variables, i);

		if(nxs_string_char_cmp(&v->name, 0, var_name) == NXS_YES) {

			return &v->value;
		}
	}

	return NULL;
}

static nxs_chat_srv_cfg_labels_set_t *nxs_chat_srv_u_labels_cfg_get_labels_set(nxs_string_t *lang)
{
	nxs_chat_srv_cfg_labels_set_t *l_set;
	size_t                         i;

	if(nxs_string_len(lang) == 0 || nxs_string_cmp(&nxs_chat_srv_cfg.labels.default_lang, 0, lang, 0) == NXS_YES) {

		/* if specified lang it's a default lang */

		return nxs_chat_srv_cfg.labels.default_labels_set;
	}

	for(i = 0; i < nxs_array_count(&nxs_chat_srv_cfg.labels.labels_sets); i++) {

		l_set = nxs_array_get(&nxs_chat_srv_cfg.labels.labels_sets, i);

		if(nxs_string_cmp(&l_set->lang, 0, lang, 0) == NXS_YES) {

			return l_set;
		}
	}

	return nxs_chat_srv_cfg.labels.default_labels_set;
}

static nxs_string_t *nxs_chat_srv_u_labels_cfg_get_label_text(nxs_chat_srv_cfg_labels_set_t *labels_set, u_char *label_key)
{
	nxs_chat_srv_cfg_label_t *l;
	size_t                    i;

	for(i = 0; i < nxs_array_count(&labels_set->labels); i++) {

		l = nxs_array_get(&labels_set->labels, i);

		if(nxs_string_char_cmp(&l->key, 0, label_key) == NXS_YES) {

			return &l->text;
		}
	}

	return NULL;
}

static nxs_string_t *nxs_chat_srv_u_labels_cfg_get_greeting_text(nxs_string_t *lang)
{
	nxs_chat_srv_cfg_greeting_t *g;
	size_t                       i;

	for(i = 0; i < nxs_array_count(&nxs_chat_srv_cfg.greetings); i++) {

		g = nxs_array_get(&nxs_chat_srv_cfg.greetings, i);

		if(nxs_string_cmp(&g->lang, 0, lang, 0) == NXS_YES) {

			return &g->text;
		}
	}

	return NULL;
}
