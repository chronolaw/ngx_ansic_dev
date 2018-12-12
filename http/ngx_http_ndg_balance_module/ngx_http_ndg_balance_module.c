// Copyright (c) 2018
// Author: Chrono Law

#include "ngx_http_ndg_balance_module.h"

typedef struct {
    /* the round robin data must be first */
    ngx_http_upstream_rr_peer_data_t   rrp;

    u_char                             tries;

    ngx_event_get_peer_pt              get_rr_peer;
} ngx_http_upstream_ndg_balance_peer_data_t;

static char *
ngx_http_upstream_ndg_balance(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t
ngx_http_upstream_init_ndg_balance(ngx_conf_t *cf, ngx_http_upstream_srv_conf_t *us);
static ngx_int_t
ngx_http_upstream_init_ndg_balance_peer(ngx_http_request_t *r,
    ngx_http_upstream_srv_conf_t *us);
static ngx_int_t
ngx_http_upstream_get_ndg_balance_peer(ngx_peer_connection_t *pc, void *data);

static ngx_command_t ngx_http_ndg_balance_cmds[] =
{
    {
        ngx_string("ndg_balance"),
        NGX_HTTP_UPS_CONF|NGX_CONF_NOARGS,
        ngx_http_upstream_ndg_balance,
        0,
        0,
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t ngx_http_ndg_balance_module_ctx =
{
    NULL,                                   /*  preconfiguration */
    NULL,                                   /*  postconfiguration */
    NULL,                                   /*  create main configuration */
    NULL,                                   /*  init main configuration */
    NULL,                                   /*  create server configuration */
    NULL,                                   /*  merge server configuration */
    NULL,                                   /*  create location configuration */
    NULL,                                   /*  merge location configuration */
};

ngx_module_t ngx_http_ndg_balance_module =
{
    NGX_MODULE_V1,
    &ngx_http_ndg_balance_module_ctx,         // module context
    ngx_http_ndg_balance_cmds,                // module directives
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

static char *
ngx_http_upstream_ndg_balance(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_upstream_srv_conf_t  *uscf;

    uscf = ngx_http_conf_get_module_srv_conf(cf, ngx_http_upstream_module);

    if (uscf->peer.init_upstream) {
        ngx_conf_log_error(NGX_LOG_WARN, cf, 0,
                           "load balancing method redefined");
    }

    uscf->peer.init_upstream = ngx_http_upstream_init_ndg_balance;

    uscf->flags = 0;

    return NGX_CONF_OK;
}

static ngx_int_t
ngx_http_upstream_init_ndg_balance(ngx_conf_t *cf, ngx_http_upstream_srv_conf_t *us)
{
    if (ngx_http_upstream_init_round_robin(cf, us) != NGX_OK) {
        return NGX_ERROR;
    }

    us->peer.init = ngx_http_upstream_init_ndg_balance_peer;

    return NGX_OK;
}

static ngx_int_t
ngx_http_upstream_init_ndg_balance_peer(ngx_http_request_t *r,
    ngx_http_upstream_srv_conf_t *us)
{
    ngx_http_upstream_ndg_balance_peer_data_t  *pd;

    pd = ngx_pcalloc(r->pool, sizeof(ngx_http_upstream_ndg_balance_peer_data_t));
    if (pd == NULL) {
        return NGX_ERROR;
    }

    r->upstream->peer.data = &pd->rrp;

    if (ngx_http_upstream_init_round_robin_peer(r, us) != NGX_OK) {
        return NGX_ERROR;
    }

    r->upstream->peer.get = ngx_http_upstream_get_ndg_balance_peer;
    //r->upstream->peer.peer = ngx_http_upstream_get_round_robin_peer;

    return NGX_OK;
}

static ngx_int_t
ngx_http_upstream_get_ndg_balance_peer(ngx_peer_connection_t *pc, void *data)
{
    ngx_uint_t                                   i;
    ngx_http_upstream_rr_peer_t                 *peer;
    ngx_http_upstream_ndg_balance_peer_data_t   *pd = data;
    ngx_http_upstream_rr_peer_data_t            *rrp;
    ngx_http_upstream_rr_peers_t                *peers;

    rrp = &pd->rrp;
    peers = rrp->peers;

    if (pd->tries > 5 || peers->single) {
        return ngx_http_upstream_get_round_robin_peer(pc, rrp);
    }

    pc->cached = 0;
    pc->connection = NULL;

    i = ngx_random() % peers->number;

    for(peer = peers->peer; i > 0;
        peer = peer->next, i--)
    {
        // void
    }

    ngx_log_error(NGX_LOG_ERR, pc->log, 0,
                  "ndg balance ok, get %V ", &peer->name);

    if (peer->down) {
        return ngx_http_upstream_get_round_robin_peer(pc, rrp);
    }

    rrp->current = peer;

    pc->sockaddr = peer->sockaddr;
    pc->socklen = peer->socklen;
    pc->name = &peer->name;

    return NGX_OK;
}
