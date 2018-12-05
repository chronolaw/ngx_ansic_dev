// Copyright (c) 2018
// Author: Chrono Law

#include "ngx_http_ndg_thread_module.h"

typedef struct {
    ngx_buf_t           *buf;

    ngx_int_t            rc;

    ngx_http_request_t  *r;
    ngx_thread_task_t   *task;
} ngx_http_ndg_thread_task_ctx_t;

static void *ngx_http_ndg_thread_create_loc_conf(ngx_conf_t* cf);
static char *ngx_http_ndg_thread_merge_loc_conf(
                ngx_conf_t *cf, void *parent, void *child);

static char *ngx_http_ndg_thread(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_int_t ngx_http_ndg_thread_handler(ngx_http_request_t *r);

static void ngx_http_ndg_thread_task_handler(void *data, ngx_log_t *log);
static void ngx_http_ndg_thread_event_handler(ngx_event_t *ev);

static ngx_command_t ngx_http_ndg_thread_cmds[] =
{
    {
        ngx_string("ndg_thread"),
        NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_http_ndg_thread,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_ndg_thread_loc_conf_t, poolname),
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

    ngx_conf_merge_str_value(conf->poolname, prev->poolname, "default");

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
    ngx_http_ndg_thread_loc_conf_t  *lcf;
    ngx_thread_pool_t               *tpool;
    ngx_thread_task_t               *task;
    ngx_http_ndg_thread_task_ctx_t  *tctx;

    lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_thread_module);

    tpool = ngx_thread_pool_get((ngx_cycle_t*)ngx_cycle, &lcf->poolname);
    if ( tpool == NULL) {
        ngx_log_error(NGX_LOG_ERR,
                      r->connection->log, 0, "ngx_thread_pool_get failed");
        return NGX_ERROR;
    }

    task = ngx_thread_task_alloc(r->pool, sizeof(ngx_http_ndg_thread_task_ctx_t));
    if (task == NULL) {
        ngx_log_error(NGX_LOG_ERR,
                      r->connection->log, 0, "ngx_thread_task_alloc failed");
        return NGX_ERROR;
    }

    tctx = task->ctx;

    tctx->rc = NGX_OK;
    tctx->r = r;
    tctx->task = task;
    tctx->buf = ngx_create_temp_buf(r->pool, 100);
    if (tctx->buf == NULL) {
        ngx_log_error(NGX_LOG_ERR,
                      r->connection->log, 0, "ngx_create_temp_buf failed");
        return NGX_ERROR;
    }

    task->handler = ngx_http_ndg_thread_task_handler;
    task->event.data = task;
    task->event.handler = ngx_http_ndg_thread_event_handler;

    if (ngx_thread_task_post(tpool, task) != NGX_OK) {
        ngx_log_error(NGX_LOG_ERR,
                      r->connection->log, 0, "ngx_thread_task_post failed");
        return NGX_ERROR;
    }

    r->main->blocked++;
    r->main->count++;

    return NGX_DONE;
}

static void ngx_http_ndg_thread_task_handler(void *data, ngx_log_t *log)
{
    ngx_http_ndg_thread_task_ctx_t *tctx;
    ngx_buf_t                      *buf;

    tctx = data;
    buf = tctx->buf;

    if (buf == NULL) {
        tctx->rc = NGX_ERROR;
        return;
    }

    buf->last = ngx_slprintf(
                buf->pos, buf->end, "%s", "hello nginx thread");
    buf->last_buf = 1;
    buf->last_in_chain = 1;

    if (tctx->task) {
        ngx_log_error(NGX_LOG_ERR, log, 0, "thread task %d ok", tctx->task->id);
    }
}
static void ngx_http_ndg_thread_event_handler(ngx_event_t *ev)
{
    ngx_thread_task_t               *task;
    ngx_http_ndg_thread_task_ctx_t  *tctx;
    ngx_http_request_t              *r;
    ngx_chain_t                     *out;
    ngx_int_t                        rc;

    task = ev->data;
    tctx = task->ctx;

    r = tctx->r;
    rc = tctx->rc;

    //ngx_http_set_log_request(r->connection->log, r);

    r->main->blocked--;
    r->main->count--;

    if (rc != NGX_OK) {
        ngx_http_finalize_request(r, rc);
        return;
    }

    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = ngx_buf_size(tctx->buf);

    rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        ngx_http_finalize_request(r, rc);
        return;
    }

    out = ngx_alloc_chain_link(r->pool);
    out->buf = tctx->buf;
    out->next = NULL;

    rc = ngx_http_output_filter(r, out);
    if (rc != NGX_OK) {
        ngx_http_finalize_request(r, rc);
        return;
    }

    // for subreqeusts
    ngx_http_run_posted_requests(r->connection);
}
