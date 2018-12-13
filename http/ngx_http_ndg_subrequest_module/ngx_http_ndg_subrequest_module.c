// Copyright (c) 2018
// Author: Chrono Law
#include "ngx_http_ndg_subrequest_module.h"

static void *ngx_http_ndg_subrequest_create_loc_conf(ngx_conf_t* cf);
static char *ngx_http_ndg_subrequest_merge_loc_conf(
                ngx_conf_t *cf, void *parent, void *child);

//static char *ngx_http_ndg_subrequest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_int_t
ngx_http_ndg_subrequest_post_handler(ngx_http_request_t *r, void *data, ngx_int_t rc);
//static void
//ngx_http_ndg_parent_post_handler(ngx_http_request_t *r);

static ngx_int_t ngx_http_ndg_subrequest_init(ngx_conf_t* cf);
static ngx_int_t ngx_http_ndg_subrequest_handler(ngx_http_request_t *r);

static ngx_command_t ngx_http_ndg_subrequest_cmds[] =
{
    {
        ngx_string("ndg_subrequest"),
        NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_conf_set_str_slot, //ngx_http_ndg_subrequest,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_ndg_subrequest_loc_conf_t, uri),
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t ngx_http_ndg_subrequest_module_ctx =
{
    NULL,                               /*  preconfiguration */
    ngx_http_ndg_subrequest_init,       /*  postconfiguration */
    NULL,                               /*  create main configuration */
    NULL,                               /*  init main configuration */
    NULL,                               /*  create server configuration */
    NULL,                               /*  merge server configuration */
    ngx_http_ndg_subrequest_create_loc_conf,  /*  create location configuration */
    ngx_http_ndg_subrequest_merge_loc_conf,   /*  merge location configuration */
};

ngx_module_t ngx_http_ndg_subrequest_module =
{
    NGX_MODULE_V1,
    &ngx_http_ndg_subrequest_module_ctx,          // module context
    ngx_http_ndg_subrequest_cmds,                 // module directives
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

static void *ngx_http_ndg_subrequest_create_loc_conf(ngx_conf_t* cf)
{
    ngx_http_ndg_subrequest_loc_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_ndg_subrequest_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    return conf;
}

static char *ngx_http_ndg_subrequest_merge_loc_conf(
                    ngx_conf_t *cf, void *parent, void *child)
{
    //ngx_http_ndg_subrequest_loc_conf_t* prev = parent;
    //ngx_http_ndg_subrequest_loc_conf_t* conf = child;

    //ngx_conf_merge_str_value(conf->uri, prev->uri, "/access");

    return NGX_CONF_OK;
}

//static char *ngx_http_ndg_subrequest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
//{
//    ngx_http_ndg_subrequest_loc_conf_t  *lcf;
//
//    char* rc = ngx_conf_set_str_slot(cf, cmd, conf);
//    if (rc != NGX_CONF_OK) {
//        return rc;
//    }
//
//    lcf = conf;
//
//    ngx_log_error(NGX_LOG_ERR, cf->log, 0,
//                  "ndg subrequest uri %V", &lcf->uri);
//
//    return NGX_CONF_OK;
//}

static ngx_int_t ngx_http_ndg_subrequest_init(ngx_conf_t* cf)
{
    ngx_http_handler_pt        *h;
    ngx_http_core_main_conf_t  *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_ACCESS_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_ndg_subrequest_handler;

    return NGX_OK;
}

static ngx_int_t ngx_http_ndg_subrequest_handler(ngx_http_request_t *r)
{
    ngx_int_t                            rc;
    ngx_str_t                            str;
    ngx_http_request_t                  *sr;
    ngx_http_post_subrequest_t          *psr;
    ngx_http_ndg_subrequest_loc_conf_t  *lcf;
    ngx_http_ndg_subrequest_ctx_t       *ctx;

    lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_subrequest_module);

    if (lcf->uri.len == 0) {
        return NGX_DECLINED;
    }

    ctx = ngx_http_get_module_ctx(r, ngx_http_ndg_subrequest_module);

    if (ctx == NULL) {
        psr = ngx_pcalloc(r->pool, sizeof(ngx_http_post_subrequest_t));
        if (psr == NULL) {
            return NGX_ERROR;
        }

        psr->handler = ngx_http_ndg_subrequest_post_handler;
        //psr->data = NULL;

        rc = ngx_http_subrequest(r, &lcf->uri, &r->args, &sr, psr,
                NGX_HTTP_SUBREQUEST_IN_MEMORY);
        if (rc != NGX_OK) {
            return NGX_ERROR;
        }

        return NGX_DONE;
    }

    // subreqeust finished
    sr = ctx->sr;

    str.data = sr->out->buf->pos;
    str.len = ngx_buf_size(sr->out->buf);
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                  "ndg subrequest ok, body is %V", &str);

    // 200 or 403
    return sr->headers_out.status == NGX_HTTP_OK ?
        NGX_OK : NGX_HTTP_FORBIDDEN;
}

static ngx_int_t
ngx_http_ndg_subrequest_post_handler(ngx_http_request_t *r, void *data, ngx_int_t rc)
{
    ngx_http_request_t              *pr;
    ngx_http_ndg_subrequest_ctx_t   *ctx;

    pr = r->parent;
    pr->write_event_handler = ngx_http_core_run_phases; //ngx_http_ndg_parent_post_handler;

    ctx = ngx_http_get_module_ctx(pr, ngx_http_ndg_subrequest_module);
    if (ctx == NULL) {
        ctx = ngx_pcalloc(pr->pool, sizeof(ngx_http_ndg_subrequest_ctx_t));
        if (ctx == NULL) {
            return NGX_ERROR;
        }

        ngx_http_set_ctx(pr, ctx, ngx_http_ndg_subrequest_module);
    }

    ctx->sr = r;

    return NGX_OK;
}

//static void
//ngx_http_ndg_parent_post_handler(ngx_http_request_t *r)
//{
//}
