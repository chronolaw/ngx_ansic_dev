// Copyright (c) 2018
// Author: Chrono Law
#include "ngx_http_ndg_variable_module.h"

static void *ngx_http_ndg_variable_create_loc_conf(ngx_conf_t* cf);
static char *ngx_http_ndg_variable_merge_loc_conf(
                ngx_conf_t *cf, void *parent, void *child);

static ngx_int_t ngx_http_ndg_add_variables(ngx_conf_t *cf);
static ngx_int_t ngx_http_current_method_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);

static ngx_int_t ngx_http_ndg_variable_init(ngx_conf_t* cf);
static ngx_int_t ngx_http_ndg_variable_handler(ngx_http_request_t *r);

static ngx_command_t ngx_http_ndg_variable_cmds[] =
{
    {
        ngx_string("ndg_variable"),
        NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
        ngx_conf_set_flag_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_ndg_variable_loc_conf_t, enable),
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t ngx_http_ndg_variable_module_ctx =
{
    ngx_http_ndg_add_variables,             /*  preconfiguration */
    ngx_http_ndg_variable_init,                  /*  postconfiguration */
    NULL,                                   /*  create main configuration */
    NULL,                                   /*  init main configuration */
    NULL,                                   /*  create server configuration */
    NULL,                                   /*  merge server configuration */
    ngx_http_ndg_variable_create_loc_conf,  /*  create location configuration */
    ngx_http_ndg_variable_merge_loc_conf,   /*  merge location configuration */
};

ngx_module_t ngx_http_ndg_variable_module =
{
    NGX_MODULE_V1,
    &ngx_http_ndg_variable_module_ctx,           // module context
    ngx_http_ndg_variable_cmds,                  // module directives
    NGX_HTTP_MODULE,                        // module type
    NULL,                                   // init master
    NULL,                                   // init module
    NULL,                                   // init process
    NULL,                                   // init thread
    NULL,                                   // exit thread
    NULL,                                   // exit process
    NULL,                                   // exit master
    NGX_MODULE_V1_PADDING
};

static ngx_http_variable_t  ngx_http_ndg_vars[] = {
    {
        ngx_string("current_method"), NULL,
        ngx_http_current_method_variable, 0,
        0, 0
    },

    ngx_http_null_variable
};

static ngx_int_t ngx_http_ndg_add_variables(ngx_conf_t *cf)
{
    ngx_http_variable_t  *var, *v;

    for (v = ngx_http_ndg_vars; v->name.len; v++) {
        var = ngx_http_add_variable(cf, &v->name, v->flags);
        if (var == NULL) {
            return NGX_ERROR;
        }

        var->get_handler = v->get_handler;
        var->data = v->data;
    }

    return NGX_OK;
}

static ngx_int_t ngx_http_current_method_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    v->len = r->method_name.len;
    v->data = r->method_name.data;

    v->valid = 1;
    v->not_found = 0;
    v->no_cacheable = 0;

    return NGX_OK;
}

static ngx_int_t ngx_http_ndg_variable_init(ngx_conf_t* cf)
{
    ngx_http_handler_pt        *h;
    ngx_http_core_main_conf_t  *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_LOG_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_ndg_variable_handler;

    return NGX_OK;
}


static ngx_int_t ngx_http_ndg_variable_handler(ngx_http_request_t *r)
{
    ngx_uint_t                   i;
    ngx_uint_t                   key;
    ngx_str_t                   *str;
    ngx_http_variable_value_t   *vv;

    ngx_http_ndg_variable_loc_conf_t* lcf;
    ngx_str_t                   names[] =
                {ngx_string("current_method"), ngx_string("hello_var")};

    lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_variable_module);

    if (!lcf->enable) {
        return NGX_OK;
    }

    // log var
    for (i = 0; i < 2; i++) {
        str = &names[i];
        key = ngx_hash_key(str->data, str->len);

        vv = ngx_http_get_variable(r, str, key);

        if (vv == NULL || vv->not_found || !vv->valid) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                "log var %V failed", str);
            return NGX_ERROR;
        }

        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                      "log var %V=%v ok", str, vv);
    }

    return NGX_OK;
}

static void *ngx_http_ndg_variable_create_loc_conf(ngx_conf_t* cf)
{
    ngx_http_ndg_variable_loc_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_ndg_variable_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    conf->enable = NGX_CONF_UNSET;

    return conf;
}

static char *ngx_http_ndg_variable_merge_loc_conf(
                    ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_ndg_variable_loc_conf_t* prev = parent;
    ngx_http_ndg_variable_loc_conf_t* conf = child;

    ngx_conf_merge_value(conf->enable, prev->enable, 0);

    return NGX_CONF_OK;
}

