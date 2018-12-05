// Copyright (c) 2018
// Author: Chrono Law

#include "ngx_stream_ndg_echo_module.h"

static void *ngx_stream_ndg_echo_create_srv_conf(ngx_conf_t* cf);
static char *ngx_stream_ndg_echo_merge_srv_conf(
                ngx_conf_t *cf, void *parent, void *child);

static char *
ngx_stream_ndg_echo(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_int_t ngx_stream_ndg_echo_init(ngx_conf_t* cf);
static ngx_int_t ngx_stream_ndg_echo_preread_handler(ngx_stream_session_t *s);

static void ngx_stream_ndg_echo_content_handler(ngx_stream_session_t *s);

static ngx_command_t ngx_stream_ndg_echo_cmds[] =
{
    {
        ngx_string("ndg_echo"),
        NGX_STREAM_SRV_CONF|NGX_CONF_NOARGS,
        ngx_stream_ndg_echo,
        NGX_STREAM_SRV_CONF_OFFSET,
        0,
        NULL
    },

    ngx_null_command
};

static ngx_stream_module_t ngx_stream_ndg_echo_module_ctx =
{
    NULL,                                       /*  preconfiguration */
    ngx_stream_ndg_echo_init,                   /*  postconfiguration */
    NULL,                                       /*  create main configuration */
    NULL,                                       /*  init main configuration */
    ngx_stream_ndg_echo_create_srv_conf,        /*  create server configuration */
    ngx_stream_ndg_echo_merge_srv_conf,         /*  merge server configuration */
};

ngx_module_t ngx_stream_ndg_echo_module =
{
    NGX_MODULE_V1,
    &ngx_stream_ndg_echo_module_ctx,           // module context
    ngx_stream_ndg_echo_cmds,                  // module directives
    NGX_STREAM_MODULE,                          // module type
    NULL,                                       // init master
    NULL,                                       // init module
    NULL,                                       // init process
    NULL,                                       // init thread
    NULL,                                       // exit thread
    NULL,                                       // exit process
    NULL,                                       // exit master
    NGX_MODULE_V1_PADDING
};

static void *ngx_stream_ndg_echo_create_srv_conf(ngx_conf_t* cf)
{
    ngx_stream_ndg_echo_srv_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_stream_ndg_echo_srv_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    conf->enable = NGX_CONF_UNSET;

    return conf;
}

static char *ngx_stream_ndg_echo_merge_srv_conf(
    ngx_conf_t *cf, void *parent, void *child)
{
    ngx_stream_ndg_echo_srv_conf_t* prev = parent;
    ngx_stream_ndg_echo_srv_conf_t* conf = child;

    ngx_conf_merge_value(conf->enable, prev->enable, 0);

    return NGX_CONF_OK;
}

static char *ngx_stream_ndg_echo(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_stream_core_srv_conf_t    *cscf;
    ngx_stream_ndg_echo_srv_conf_t *scf = conf;

    scf->enable = 1;

    cscf = ngx_stream_conf_get_module_srv_conf(cf, ngx_stream_core_module);
    if (cscf == NULL) {
        return NGX_CONF_ERROR;
    }

    cscf->handler = ngx_stream_ndg_echo_content_handler;

    return NGX_CONF_OK;
}

static void ngx_stream_ndg_echo_content_handler(ngx_stream_session_t *s)
{
    ngx_log_error(NGX_LOG_ERR, s->connection->log, 0, "never reach here");

    ngx_stream_finalize_session(s, NGX_STREAM_OK);
}


static ngx_int_t ngx_stream_ndg_echo_init(ngx_conf_t* cf)
{
    ngx_stream_handler_pt        *h;
    ngx_stream_core_main_conf_t  *cmcf;

    cmcf = ngx_stream_conf_get_module_main_conf(cf, ngx_stream_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_STREAM_PREREAD_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_stream_ndg_echo_preread_handler;

    return NGX_OK;
}

static ngx_int_t ngx_stream_ndg_echo_preread_handler(ngx_stream_session_t *s)
{
    ngx_buf_t                       *b;
    ngx_chain_t                     *out;
    ngx_connection_t                *c;
    ngx_stream_ndg_echo_srv_conf_t  *scf;

    scf = ngx_stream_get_module_srv_conf(s, ngx_stream_ndg_echo_module);
    if (!scf->enable) {
        return NGX_DECLINED;
    }

    c = s->connection;

    if (c->buffer == NULL) {
        return NGX_AGAIN;
    }

    b = ngx_create_temp_buf(c->pool, ngx_buf_size(c->buffer));
    if (b == NULL) {
        ngx_log_error(NGX_LOG_ERR, c->log, 0, "ngx_create_temp_buf failed");
        return NGX_ERROR;
    }

    b->last = ngx_cpymem(b->pos, c->buffer->pos, ngx_buf_size(c->buffer));

    out = ngx_alloc_chain_link(c->pool);
    if (out == NULL) {
        ngx_log_error(NGX_LOG_ERR, c->log, 0, "ngx_alloc_chain_link failed");
        return NGX_ERROR;
    }

    out->buf = b;
    out->next = NULL;

    ngx_log_error(NGX_LOG_ERR, s->connection->log, 0, "echo send %O bytes", ngx_buf_size(b));

    if (ngx_stream_top_filter(s, out, 1) == NGX_ERROR) {
        ngx_log_error(NGX_LOG_ERR, c->log, 0, "ngx_stream_top_filter failed");
        return NGX_ERROR;
    }

    // consume buffer
    c->buffer->last = c->buffer->pos;

    return NGX_AGAIN;
}

#if 0

// line echo
static ngx_int_t ngx_stream_ndg_echo_preread_handler(ngx_stream_session_t *s)
{
    u_char                          *p;
    ngx_buf_t                       *b;
    ngx_chain_t                     *out;
    ngx_connection_t                *c;
    ngx_stream_ndg_echo_srv_conf_t  *scf;

    scf = ngx_stream_get_module_srv_conf(s, ngx_stream_ndg_echo_module);
    if (!scf->enable) {
        return NGX_DECLINED;
    }

    c = s->connection;

    if (c->buffer == NULL) {
        return NGX_AGAIN;
    }

    p = ngx_strlchr(c->buffer->pos, c->buffer->last, LF);   //'\n'
    if (p == NULL) {
        return NGX_AGAIN;
    }

    // send without '\n'
    b = ngx_create_temp_buf(c->pool, p - c->buffer->pos);
    if (b == NULL) {
        ngx_log_error(NGX_LOG_ERR, c->log, 0, "ngx_create_temp_buf failed");
        return NGX_ERROR;
    }

    b->last = ngx_cpymem(b->pos, c->buffer->pos, p - c->buffer->pos);

    out = ngx_alloc_chain_link(c->pool);
    if (out == NULL) {
        ngx_log_error(NGX_LOG_ERR, c->log, 0, "ngx_alloc_chain_link failed");
        return NGX_ERROR;
    }

    out->buf = b;
    out->next = NULL;

    ngx_log_error(NGX_LOG_ERR, s->connection->log, 0, "echo send %O bytes", ngx_buf_size(b));

    if (ngx_stream_top_filter(s, out, 1) == NGX_ERROR) {
        ngx_log_error(NGX_LOG_ERR, c->log, 0, "ngx_stream_top_filter failed");
        return NGX_ERROR;
    }

    // consume buffer
    c->buffer->last = ngx_cpymem(c->buffer->pos, p + 1, c->buffer->last - (p+1));

    return NGX_AGAIN;
}

#endif

