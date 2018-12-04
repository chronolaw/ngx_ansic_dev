// Copyright (c) 2018
// Author: Chrono Law

#include "ngx_stream_ndg_discard_module.h"

static void *ngx_stream_ndg_discard_create_srv_conf(ngx_conf_t* cf);
static char *ngx_stream_ndg_discard_merge_srv_conf(
                ngx_conf_t *cf, void *parent, void *child);

static char *
ngx_stream_ndg_discard(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

void ngx_stream_ndg_discard_write_handler(ngx_event_t *ev);
void ngx_stream_ndg_discard_read_handler(ngx_event_t *ev);

static void ngx_stream_ndg_discard_handler(ngx_stream_session_t *s);

static ngx_command_t ngx_stream_ndg_discard_cmds[] =
{
    {
        ngx_string("ndg_discard_timeout"),
        NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
        ngx_conf_set_msec_slot,
        NGX_STREAM_SRV_CONF_OFFSET,
        offsetof(ngx_stream_ndg_discard_srv_conf_t, timeout),
        NULL
    },

    {
        ngx_string("ndg_discard"),
        NGX_STREAM_SRV_CONF|NGX_CONF_NOARGS,
        ngx_stream_ndg_discard,
        NGX_STREAM_SRV_CONF_OFFSET,
        0,
        NULL
    },

    ngx_null_command
};

static ngx_stream_module_t ngx_stream_ndg_discard_module_ctx =
{
    NULL,                                       /*  preconfiguration */
    NULL,                                       /*  postconfiguration */
    NULL,                                       /*  create main configuration */
    NULL,                                       /*  init main configuration */
    ngx_stream_ndg_discard_create_srv_conf,     /*  create server configuration */
    ngx_stream_ndg_discard_merge_srv_conf,      /*  merge server configuration */
};

ngx_module_t ngx_stream_ndg_discard_module =
{
    NGX_MODULE_V1,
    &ngx_stream_ndg_discard_module_ctx,           // module context
    ngx_stream_ndg_discard_cmds,                  // module directives
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

static void *ngx_stream_ndg_discard_create_srv_conf(ngx_conf_t* cf)
{
    ngx_stream_ndg_discard_srv_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_stream_ndg_discard_srv_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    conf->timeout = NGX_CONF_UNSET_MSEC;

    return conf;
}

static char *ngx_stream_ndg_discard_merge_srv_conf(
    ngx_conf_t *cf, void *parent, void *child)
{
    ngx_stream_ndg_discard_srv_conf_t* prev = parent;
    ngx_stream_ndg_discard_srv_conf_t* conf = child;

    ngx_conf_merge_msec_value(conf->timeout, prev->timeout, 5000);

    return NGX_CONF_OK;
}

static char *ngx_stream_ndg_discard(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_stream_core_srv_conf_t    *cscf;

    cscf = ngx_stream_conf_get_module_srv_conf(cf, ngx_stream_core_module);
    if (cscf == NULL) {
        return NGX_CONF_ERROR;
    }

    cscf->handler = ngx_stream_ndg_discard_handler;

    return NGX_CONF_OK;
}

static void ngx_stream_ndg_discard_handler(ngx_stream_session_t *s)
{
    ngx_connection_t        *c;
    ngx_stream_ndg_discard_srv_conf_t* scf;

    c = s->connection;
    scf = ngx_stream_get_module_srv_conf(s, ngx_stream_ndg_discard_module);

    c->write->handler = ngx_stream_ndg_discard_write_handler;

    if (ngx_handle_write_event(c->write, 0) != NGX_OK) {
        ngx_stream_finalize_session(s, NGX_STREAM_INTERNAL_SERVER_ERROR);
        return;
    }

    c->read->handler = ngx_stream_ndg_discard_read_handler;
    ngx_add_timer(c->read, scf->timeout);

    c->read->handler(c->read);

    return;
}

void ngx_stream_ndg_discard_write_handler(ngx_event_t *ev)
{
    // do nothing
}

void ngx_stream_ndg_discard_read_handler(ngx_event_t *ev)
{
    // do nothing
}

