// Copyright (c) 2018
// Author: Chrono Law

#include "ngx_http_ndg_hello_module.h"

static void *ngx_http_ndg_hello_create_loc_conf(ngx_conf_t* cf);
static char *ngx_http_ndg_hello_merge_loc_conf(
                ngx_conf_t *cf, void *parent, void *child);

static ngx_int_t ngx_http_ndg_hello_init(ngx_conf_t* cf);
static ngx_int_t ngx_http_ndg_hello_handler(ngx_http_request_t *r);

static ngx_command_t ngx_http_ndg_hello_cmds[] =
{
    {
        ngx_string("ndg_hello"),
        NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
        ngx_conf_set_flag_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_ndg_hello_loc_conf_t, enable),
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t ngx_http_ndg_hello_module_ctx =
{
    NULL,                                   /*  preconfiguration */
    ngx_http_ndg_hello_init,                /*  postconfiguration */
    NULL,                                   /*  create main configuration */
    NULL,                                   /*  init main configuration */
    NULL,                                   /*  create server configuration */
    NULL,                                   /*  merge server configuration */
    ngx_http_ndg_hello_create_loc_conf,     /*  create location configuration */
    ngx_http_ndg_hello_merge_loc_conf,      /*  merge location configuration */
};

ngx_module_t ngx_http_ndg_hello_module =
{
    NGX_MODULE_V1,
    &ngx_http_ndg_hello_module_ctx,         // module context
    ngx_http_ndg_hello_cmds,                // module directives
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

static void *ngx_http_ndg_hello_create_loc_conf(ngx_conf_t* cf)
{
    ngx_http_ndg_hello_loc_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_ndg_hello_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    conf->enable = NGX_CONF_UNSET;

    return conf;
}

static char *ngx_http_ndg_hello_merge_loc_conf(
    ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_ndg_hello_loc_conf_t* prev = parent;
    ngx_http_ndg_hello_loc_conf_t* conf = child;

    ngx_conf_merge_value(conf->enable, prev->enable, 0);

    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_ndg_hello_init(ngx_conf_t* cf)
{
    ngx_http_handler_pt        *h;
    ngx_http_core_main_conf_t  *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_REWRITE_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_ndg_hello_handler;

    return NGX_OK;
}

static ngx_int_t ngx_http_ndg_hello_handler(ngx_http_request_t *r)
{
    ngx_http_ndg_hello_loc_conf_t* lcf;

    lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_hello_module);

    if (lcf->enable) {
        printf("hello nginx\n");
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "hello ansi c");
    } else {
        //printf("hello disabled\n");
    }

    return NGX_DECLINED;
}

