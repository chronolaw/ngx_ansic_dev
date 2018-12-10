// Copyright (c) 2018
// Author: Chrono Law
#include "ngx_http_ndg_complex_value_module.h"

static void *ngx_http_ndg_complex_value_create_loc_conf(ngx_conf_t* cf);
static char *ngx_http_ndg_complex_value_merge_loc_conf(
                ngx_conf_t *cf, void *parent, void *child);

static char *ngx_http_ndg_complex_value(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf);

static ngx_int_t ngx_http_ndg_complex_value_init(ngx_conf_t* cf);
static ngx_int_t ngx_http_ndg_complex_value_handler(ngx_http_request_t *r);

static ngx_command_t ngx_http_ndg_complex_value_cmds[] =
{
    {
        ngx_string("ndg_complex_value"),
        NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_http_ndg_complex_value,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t ngx_http_ndg_complex_value_module_ctx =
{
    NULL,                                    /*  preconfiguration */
    ngx_http_ndg_complex_value_init,        /*  postconfiguration */
    NULL,                                   /*  create main configuration */
    NULL,                                   /*  init main configuration */
    NULL,                                   /*  create server configuration */
    NULL,                                   /*  merge server configuration */
    ngx_http_ndg_complex_value_create_loc_conf,       /*  create location configuration */
    ngx_http_ndg_complex_value_merge_loc_conf, /*  merge location configuration */
};

ngx_module_t ngx_http_ndg_complex_value_module =
{
    NGX_MODULE_V1,
    &ngx_http_ndg_complex_value_module_ctx,           // module context
    ngx_http_ndg_complex_value_cmds,                  // module directives
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

static void *ngx_http_ndg_complex_value_create_loc_conf(ngx_conf_t* cf)
{
    ngx_http_ndg_complex_value_loc_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_ndg_complex_value_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    //conf->cv = NULL;

    return conf;
}

static char *ngx_http_ndg_complex_value_merge_loc_conf(
                    ngx_conf_t *cf, void *parent, void *child)
{
    //ngx_http_ndg_complex_value_loc_conf_t* prev = parent;
    //ngx_http_ndg_complex_value_loc_conf_t* conf = child;

    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_ndg_complex_value_init(ngx_conf_t* cf)
{
    ngx_http_handler_pt        *h;
    ngx_http_core_main_conf_t  *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_LOG_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_ndg_complex_value_handler;

    return NGX_OK;
}

static char *ngx_http_ndg_complex_value(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf)
{
    ngx_http_compile_complex_value_t         ccv;
    ngx_str_t                               *value;
    ngx_http_ndg_complex_value_loc_conf_t   *lcf;

    lcf = conf;
    lcf->cv = ngx_pcalloc(cf->pool, sizeof(ngx_http_complex_value_t));
    if (lcf->cv == NULL) {
        return NGX_CONF_ERROR;
    }

    value = cf->args->elts;

    ngx_memzero(&ccv, sizeof(ngx_http_compile_complex_value_t));

    ccv.cf = cf;
    ccv.value = &value[1];
    ccv.complex_value = lcf->cv;

    if (ngx_http_compile_complex_value(&ccv) != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_ndg_complex_value_handler(ngx_http_request_t *r)
{
    ngx_str_t                               str;
    ngx_http_ndg_complex_value_loc_conf_t  *lcf;

    lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_complex_value_module);

    if (lcf->cv) {

        if (ngx_http_complex_value(r, lcf->cv, &str) != NGX_OK) {
            return NGX_ERROR;
        }

        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
            "log complex %V ok", &str);
    }

    return NGX_OK;
}

