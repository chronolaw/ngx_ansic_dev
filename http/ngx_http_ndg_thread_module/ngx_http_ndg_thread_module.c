// Copyright (c) 2018
// Author: Chrono Law

#include "ngx_http_ndg_thread_module.h"

static void *ngx_http_ndg_thread_create_loc_conf(ngx_conf_t* cf);
static char *ngx_http_ndg_thread_merge_loc_conf(
                ngx_conf_t *cf, void *parent, void *child);

static char *ngx_http_ndg_thread(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_int_t ngx_http_ndg_thread_handler(ngx_http_request_t *r);

static ngx_command_t ngx_http_ndg_thread_cmds[] =
{
    {
        ngx_string("ndg_thread"),
        NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_http_ndg_thread,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_ndg_thread_loc_conf_t, pool_name),
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t ngx_http_ndg_thread_module_ctx =
{
    NULL,                                   /*  preconfiguration */
    NULL,                                   /*  postconfiguration */
    NULL,                                   /*  create main configuration */
    NULL,                                   /*  init main configuration */
    NULL,                                   /*  create server configuration */
    NULL,                                   /*  merge server configuration */
    ngx_http_ndg_thread_create_loc_conf,     /*  create location configuration */
    ngx_http_ndg_thread_merge_loc_conf,      /*  merge location configuration */
};

ngx_module_t ngx_http_ndg_thread_module =
{
    NGX_MODULE_V1,
    &ngx_http_ndg_thread_module_ctx,         // module context
    ngx_http_ndg_thread_cmds,                // module directives
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

static void *ngx_http_ndg_thread_create_loc_conf(ngx_conf_t* cf)
{
    ngx_http_ndg_thread_loc_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_ndg_thread_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    return conf;
}

static char *ngx_http_ndg_thread_merge_loc_conf(
    ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_ndg_thread_loc_conf_t* prev = parent;
    ngx_http_ndg_thread_loc_conf_t* conf = child;

    ngx_conf_merge_str_value(conf->pool_name, prev->pool_name, "default");

    return NGX_CONF_OK;
}

static char *ngx_http_ndg_thread(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    char* rc = ngx_conf_set_str_slot(cf, cmd, conf);
    if (rc != NGX_CONF_OK) {
        return rc;
    }

    ngx_http_core_loc_conf_t    *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    if (clcf == NULL) {
        return NGX_CONF_ERROR;
    }

    clcf->handler = ngx_http_ndg_thread_handler;

    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_ndg_thread_handler(ngx_http_request_t *r)
{
    //ngx_http_ndg_thread_loc_conf_t* lcf;

    //lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_thread_module);


    return NGX_DONE;
}

