// Copyright (c) 2018
// Author: Chrono Law

#include "ngx_stream_ndg_time_module.h"


static char *
ngx_stream_ndg_time(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

void ngx_stream_ndg_time_write_handler(ngx_event_t *ev);
void ngx_stream_ndg_time_read_handler(ngx_event_t *ev);

static void ngx_stream_ndg_time_handler(ngx_stream_session_t *s);

static ngx_command_t ngx_stream_ndg_time_cmds[] =
{
    {
        ngx_string("ndg_time"),
        NGX_STREAM_SRV_CONF|NGX_CONF_NOARGS,
        ngx_stream_ndg_time,
        NGX_STREAM_SRV_CONF_OFFSET,
        0,
        NULL
    },

    ngx_null_command
};

static ngx_stream_module_t ngx_stream_ndg_time_module_ctx =
{
    NULL,                                       /*  preconfiguration */
    NULL,                                       /*  postconfiguration */
    NULL,                                       /*  create main configuration */
    NULL,                                       /*  init main configuration */
    NULL,                                       /*  create server configuration */
    NULL,                                       /*  merge server configuration */
};

ngx_module_t ngx_stream_ndg_time_module =
{
    NGX_MODULE_V1,
    &ngx_stream_ndg_time_module_ctx,           // module context
    ngx_stream_ndg_time_cmds,                  // module directives
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

static char *ngx_stream_ndg_time(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_stream_core_srv_conf_t    *cscf;

    cscf = ngx_stream_conf_get_module_srv_conf(cf, ngx_stream_core_module);
    if (cscf == NULL) {
        return NGX_CONF_ERROR;
    }

    cscf->handler = ngx_stream_ndg_time_handler;

    return NGX_CONF_OK;
}

static void ngx_stream_ndg_time_handler(ngx_stream_session_t *s)
{
    ngx_connection_t        *c;

    c = s->connection;

    c->read->handler = ngx_stream_ndg_time_read_handler;

    if (ngx_handle_read_event(c->read, 0) != NGX_OK) {
        ngx_stream_finalize_session(s, NGX_STREAM_INTERNAL_SERVER_ERROR);
        return;
    }

    c->write->handler = ngx_stream_ndg_time_write_handler;

    if (ngx_handle_write_event(c->write, 0) != NGX_OK) {
        ngx_stream_finalize_session(s, NGX_STREAM_INTERNAL_SERVER_ERROR);
        return;
    }

    c->write->handler(c->write);

    return;
}

void ngx_stream_ndg_time_write_handler(ngx_event_t *ev)
{
    ngx_int_t                    rc;
    ngx_buf_t                   *b;
    ngx_chain_t                 *out;
    ngx_connection_t            *c;
    ngx_stream_session_t        *s;

    c = ev->data;
    s = c->data;

    if (ev->timedout) {
        ngx_connection_error(c, NGX_ETIMEDOUT, "connection timed out");
        ngx_stream_finalize_session(s, NGX_STREAM_OK);
        return;
    }

    if (!ev->ready) {
        if (ngx_handle_write_event(ev, 0) != NGX_OK) {
            ngx_stream_finalize_session(s, NGX_STREAM_INTERNAL_SERVER_ERROR);
            return;
        }

        ngx_add_timer(ev, 100);
        return;
    }

    // write ready
    b = ngx_create_temp_buf(c->pool, 20);
    if (b == NULL) {
        ngx_log_error(NGX_LOG_ERR, c->log, 0, "ngx_create_temp_buf failed");
        ngx_stream_finalize_session(s, NGX_STREAM_INTERNAL_SERVER_ERROR);
        return;
    }

    b->last = ngx_slprintf(b->pos, b->end, "%T\n", ngx_time());
    b->last_buf = 1;
    b->last_in_chain = 1;

    out = ngx_alloc_chain_link(c->pool);
    if (out == NULL) {
        ngx_log_error(NGX_LOG_ERR, c->log, 0, "ngx_alloc_chain_link failed");
        ngx_stream_finalize_session(s, NGX_STREAM_INTERNAL_SERVER_ERROR);
        return;
    }

    out->buf = b;
    out->next = NULL;

    // from_upstream == 1, send to client
    rc = ngx_stream_top_filter(s, out, 1);

    ngx_log_error(NGX_LOG_ERR, c->log, 0, "ndg_time send %T OK.", ngx_time());

    ngx_stream_finalize_session(s, rc);
}

void ngx_stream_ndg_time_read_handler(ngx_event_t *ev)
{
    // do nothing
}

