// Copyright (c) 2018
// Author: Chrono Law
#include "ngx_http_ndg_basic_module.h"

static void *ngx_http_ndg_basic_create_loc_conf(ngx_conf_t* cf);
static char *ngx_http_ndg_basic_merge_loc_conf(
                ngx_conf_t *cf, void *parent, void *child);

static ngx_int_t ngx_http_ndg_basic_init(ngx_conf_t* cf);
static ngx_int_t ngx_http_ndg_basic_handler(ngx_http_request_t *r);

static void ngx_http_ngd_string_test(ngx_http_request_t *r);
static void ngx_http_ngd_time_test(ngx_http_request_t *r);

static ngx_command_t ngx_http_ndg_basic_cmds[] =
{
    {
        ngx_string("ndg_basic"),
        NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
        ngx_conf_set_flag_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_ndg_basic_loc_conf_t, enable),
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t ngx_http_ndg_basic_module_ctx =
{
    NULL,                                   /*  preconfiguration */
    ngx_http_ndg_basic_init,                /*  postconfiguration */
    NULL,                                   /*  create main configuration */
    NULL,                                   /*  init main configuration */
    NULL,                                   /*  create server configuration */
    NULL,                                   /*  merge server configuration */
    ngx_http_ndg_basic_create_loc_conf,     /*  create location configuration */
    ngx_http_ndg_basic_merge_loc_conf,      /*  merge location configuration */
};

ngx_module_t ngx_http_ndg_basic_module =
{
    NGX_MODULE_V1,
    &ngx_http_ndg_basic_module_ctx,         // module context
    ngx_http_ndg_basic_cmds,                // module directives
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

static void *ngx_http_ndg_basic_create_loc_conf(ngx_conf_t* cf)
{
    ngx_http_ndg_basic_loc_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_ndg_basic_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    conf->enable = NGX_CONF_UNSET;

    return conf;
}

static char *ngx_http_ndg_basic_merge_loc_conf(
                    ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_ndg_basic_loc_conf_t* prev = parent;
    ngx_http_ndg_basic_loc_conf_t* conf = child;

    ngx_conf_merge_value(conf->enable, prev->enable, 0);

    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_ndg_basic_init(ngx_conf_t* cf)
{
    ngx_http_handler_pt        *h;
    ngx_http_core_main_conf_t  *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_REWRITE_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_ndg_basic_handler;

    return NGX_OK;
}

static ngx_int_t ngx_http_ndg_basic_handler(ngx_http_request_t *r)
{
    ngx_http_ndg_basic_loc_conf_t* lcf;

    lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_basic_module);

    if (lcf->enable) {

        ngx_http_ngd_string_test(r);
        ngx_http_ngd_time_test(r);

        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "basic ok");
    }

    return NGX_DECLINED;
}

static void ngx_http_ngd_string_test(ngx_http_request_t *r)
{
    ngx_str_t s1 = ngx_null_string;
    ngx_str_t s2 = ngx_string("matrix");

    assert(s1.data == NULL && s1.len == 0);
    assert(s2.data && s2.len == 6);

    ngx_str_set(&s1, "reloaded");
    ngx_str_null(&s2);

    assert(s1.data && s1.len == 8);
    assert(s2.data == NULL && s2.len == 0);

    // strcmp

    // sprintf

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx_str ok");
}

static void ngx_http_ngd_time_test(ngx_http_request_t *r)
{
    time_t sec = ngx_time();
    ngx_time_t* now = ngx_timeofday();
    ngx_msec_t msec = ngx_current_msec;

    assert(now->sec == sec);
    assert(now->gmtoff = 8 * 60);

    // ngx_monotonic_time
    (void) msec;
    //assert(msec == now->sec * 1000 + now->msec);

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx_time ok");
}
