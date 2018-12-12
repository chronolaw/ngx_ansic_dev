// Copyright (c) 2018
// Author: Chrono Law
#include "ngx_http_ndg_upstream_module.h"

static void *ngx_http_ndg_upstream_create_loc_conf(ngx_conf_t* cf);
static char *ngx_http_ndg_upstream_merge_loc_conf(
                ngx_conf_t *cf, void *parent, void *child);

static char *ngx_http_ndg_upstream_pass(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_int_t ngx_http_ndg_upstream_handler(ngx_http_request_t *r);

static ngx_int_t ngx_http_ndg_upstream_create_request(ngx_http_request_t *r);
static ngx_int_t ngx_http_ndg_upstream_reinit_request(ngx_http_request_t *r);
static ngx_int_t ngx_http_ndg_upstream_process_header(ngx_http_request_t *r);
static void ngx_http_ndg_upstream_finalize_request(ngx_http_request_t *r, ngx_int_t rc);

static ngx_command_t ngx_http_ndg_upstream_cmds[] =
{
    {
        ngx_string("ndg_upstream_pass"),
        NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_http_ndg_upstream_pass,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t ngx_http_ndg_upstream_module_ctx =
{
    NULL,                               /*  preconfiguration */
    NULL,                               /*  postconfiguration */
    NULL,                               /*  create main configuration */
    NULL,                               /*  init main configuration */
    NULL,                               /*  create server configuration */
    NULL,                               /*  merge server configuration */
    ngx_http_ndg_upstream_create_loc_conf,  /*  create location configuration */
    ngx_http_ndg_upstream_merge_loc_conf,   /*  merge location configuration */
};

ngx_module_t ngx_http_ndg_upstream_module =
{
    NGX_MODULE_V1,
    &ngx_http_ndg_upstream_module_ctx,          // module context
    ngx_http_ndg_upstream_cmds,                 // module directives
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

static void *ngx_http_ndg_upstream_create_loc_conf(ngx_conf_t* cf)
{
    ngx_http_ndg_upstream_loc_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_ndg_upstream_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    conf->upstream.connect_timeout = NGX_CONF_UNSET_MSEC;
    conf->upstream.send_timeout = NGX_CONF_UNSET_MSEC;
    conf->upstream.read_timeout = NGX_CONF_UNSET_MSEC;
    conf->upstream.buffer_size = NGX_CONF_UNSET_SIZE;

    return conf;
}

static char *ngx_http_ndg_upstream_merge_loc_conf(
                    ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_ndg_upstream_loc_conf_t* prev = parent;
    ngx_http_ndg_upstream_loc_conf_t* conf = child;

    ngx_conf_merge_msec_value(
        conf->upstream.connect_timeout, prev->upstream.connect_timeout, 1000);
    ngx_conf_merge_msec_value(
        conf->upstream.send_timeout, prev->upstream.send_timeout, 1000);
    ngx_conf_merge_msec_value(
        conf->upstream.read_timeout, prev->upstream.read_timeout, 1000);
    ngx_conf_merge_size_value(
        conf->upstream.buffer_size, prev->upstream.buffer_size, 4096);

    return NGX_CONF_OK;
}

static char *ngx_http_ndg_upstream_pass(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_url_t                            u;
    ngx_str_t                           *value;
    ngx_http_ndg_upstream_loc_conf_t    *lcf = conf;
    ngx_http_core_loc_conf_t            *clcf;

    value = cf->args->elts;

    ngx_memzero(&u, sizeof(ngx_url_t));
    u.url = value[1];
    u.no_resolve = 1;

    lcf->upstream.upstream = ngx_http_upstream_add(cf, &u, 0);

    if (lcf->upstream.upstream == NULL) {
        return NGX_CONF_ERROR;
    }

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    if (clcf == NULL) {
        return NGX_CONF_ERROR;
    }

    clcf->handler = ngx_http_ndg_upstream_handler;

    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_ndg_upstream_handler(ngx_http_request_t *r)
{
    ngx_int_t                            rc;
    ngx_http_upstream_t                 *u;
    ngx_http_ndg_upstream_loc_conf_t    *lcf;

    if (!(r->method & NGX_HTTP_GET)) {
        return NGX_HTTP_NOT_ALLOWED;
    }

    rc = ngx_http_discard_request_body(r);

    if (rc != NGX_OK) {
        return rc;
    }

    if (ngx_http_upstream_create(r) != NGX_OK) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    u = r->upstream;

    lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_upstream_module);

    u->conf = &lcf->upstream;

    u->create_request = ngx_http_ndg_upstream_create_request;
    u->reinit_request = ngx_http_ndg_upstream_reinit_request;
    u->process_header = ngx_http_ndg_upstream_process_header;
    u->finalize_request = ngx_http_ndg_upstream_finalize_request;

    r->main->count++;

    ngx_http_upstream_init(r);

    return NGX_DONE;
}

static ngx_int_t ngx_http_ndg_upstream_create_request(ngx_http_request_t *r)
{
    ngx_pool_t      *pool;
    ngx_buf_t       *b1, *b2;
    ngx_chain_t     *out1, *out2;
    ngx_str_t        lf = ngx_string("\n");

    pool = r->pool;

    b1 = ngx_calloc_buf(pool);
    b2 = ngx_calloc_buf(pool);
    if (b1 == NULL || b2 == NULL) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx_calloc_buf failed");
        return NGX_ERROR;
    }

    b1->pos = r->args.data;
    b1->last = r->args.data + r->args.len;
    b1->memory = 1;

    b2->pos = lf.data;
    b2->last = lf.data + lf.len;
    b2->memory = 1;
    b2->last_buf = 1;
    b2->last_in_chain = 1;

    out1 = ngx_alloc_chain_link(pool);
    out2 = ngx_alloc_chain_link(pool);
    if (out1 == NULL || out2 == NULL) {
        return NGX_ERROR;
    }

    out1->buf = b1;
    out1->next = out2;
    out2->buf = b2;
    out2->next = NULL;

    r->upstream->request_bufs = out1;

    return NGX_OK;
}

static ngx_int_t ngx_http_ndg_upstream_process_header(ngx_http_request_t *r)
{
    u_char                  *p;
    ngx_http_upstream_t     *u;

    u = r->upstream;

    p = ngx_strlchr(u->buffer.pos, u->buffer.last, LF);   //'\n'
    if (p == NULL) {
        return NGX_AGAIN;
    }

    u->headers_in.content_length_n = p - u->buffer.pos;
    u->headers_in.status_n = NGX_HTTP_OK;

    u->state->status = NGX_HTTP_OK;

    // remove '\n'
    u->buffer.last = p;

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ndg upstream ok");

    return NGX_OK;
}

static ngx_int_t ngx_http_ndg_upstream_reinit_request(ngx_http_request_t *r)
{
    // do nothing
    return NGX_OK;
}

static void ngx_http_ndg_upstream_finalize_request(ngx_http_request_t *r, ngx_int_t rc)
{
    // do nothing
}

