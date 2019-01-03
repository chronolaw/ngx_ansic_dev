// Copyright (c) 2018
// Author: Chrono Law

#include "ngx_http_ndg_debug_module.h"

static void *ngx_http_ndg_debug_create_loc_conf(ngx_conf_t* cf);
static char *ngx_http_ndg_debug_merge_loc_conf(
                ngx_conf_t *cf, void *parent, void *child);

static ngx_int_t ngx_http_ndg_debug_init(ngx_conf_t* cf);
static ngx_int_t ngx_http_ndg_debug_handler(ngx_http_request_t *r);

static ngx_command_t ngx_http_ndg_debug_cmds[] =
{
    {
        ngx_string("ndg_debug"),
        NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
        ngx_conf_set_flag_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_ndg_debug_loc_conf_t, enable),
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t ngx_http_ndg_debug_module_ctx =
{
    NULL,                                   /*  preconfiguration */
    ngx_http_ndg_debug_init,                /*  postconfiguration */
    NULL,                                   /*  create main configuration */
    NULL,                                   /*  init main configuration */
    NULL,                                   /*  create server configuration */
    NULL,                                   /*  merge server configuration */
    ngx_http_ndg_debug_create_loc_conf,     /*  create location configuration */
    ngx_http_ndg_debug_merge_loc_conf,      /*  merge location configuration */
};

ngx_module_t ngx_http_ndg_debug_module =
{
    NGX_MODULE_V1,
    &ngx_http_ndg_debug_module_ctx,         // module context
    ngx_http_ndg_debug_cmds,                // module directives
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

static void *ngx_http_ndg_debug_create_loc_conf(ngx_conf_t* cf)
{
    ngx_http_ndg_debug_loc_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_ndg_debug_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    conf->enable = NGX_CONF_UNSET;

    return conf;
}

static char *ngx_http_ndg_debug_merge_loc_conf(
    ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_ndg_debug_loc_conf_t* prev = parent;
    ngx_http_ndg_debug_loc_conf_t* conf = child;

    ngx_conf_merge_value(conf->enable, prev->enable, 0);

    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_ndg_debug_init(ngx_conf_t* cf)
{
    ngx_http_handler_pt        *h;
    ngx_http_core_main_conf_t  *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_ACCESS_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_ndg_debug_handler;

    return NGX_OK;
}

static ngx_int_t ngx_http_ndg_debug_handler(ngx_http_request_t *r)
{
    ngx_http_ndg_debug_loc_conf_t   *lcf;
    ngx_log_t                       *log;

    log = r->connection->log;

    lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_debug_module);

    if (lcf->enable) {
        ngx_debug_point();

        //printf("debug nginx\n");

        // needs "configure --with-debug"

        ngx_log_debug0(NGX_LOG_DEBUG_ALLOC, log, 0, "debug nginx");
        ngx_log_debug1(NGX_LOG_DEBUG_CORE, log, 0, "debug core %P", ngx_pid);
        ngx_log_debug2(NGX_LOG_DEBUG_HTTP, log, 0,
                      "debug http %V %uA",
                      &r->method_name, r->connection->number);

        ngx_log_error(NGX_LOG_ERR, log, 0, "debug ansi c");
    }

    return NGX_DECLINED;
}

