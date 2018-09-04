// Copyright (c) 2018
// Author: Chrono Law
#include "ngx_http_ndg_filter_module.h"

static void *ngx_http_ndg_filter_create_loc_conf(ngx_conf_t* cf);
//static char *ngx_http_ndg_filter_merge_loc_conf(
//                ngx_conf_t *cf, void *parent, void *child);

static ngx_int_t ngx_http_ndg_filter_init(ngx_conf_t* cf);

static ngx_command_t ngx_http_ndg_filter_cmds[] =
{
    {
        ngx_string("ndg_header"),
        NGX_HTTP_LOC_CONF|NGX_CONF_TAKE2,
        ngx_conf_set_keyval_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_ndg_filter_loc_conf_t, headers),
        NULL
    },

    {
        ngx_string("ndg_footer"),
        NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_conf_set_str_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_ndg_filter_loc_conf_t, footer),
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t ngx_http_ndg_filter_module_ctx =
{
    NULL,                                   /*  preconfiguration */
    ngx_http_ndg_filter_init,               /*  postconfiguration */
    NULL,                                   /*  create main configuration */
    NULL,                                   /*  init main configuration */
    NULL,                                   /*  create server configuration */
    NULL,                                   /*  merge server configuration */
    ngx_http_ndg_filter_create_loc_conf,    /*  create location configuration */
    NULL,                                   /*  merge location configuration */
};

ngx_module_t ngx_http_ndg_filter_module =
{
    NGX_MODULE_V1,
    &ngx_http_ndg_filter_module_ctx,        // module context
    ngx_http_ndg_filter_cmds,               // module directives
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

// filter chain pointer
static ngx_http_output_header_filter_pt  ngx_http_next_header_filter;
static ngx_http_output_body_filter_pt    ngx_http_next_body_filter;

static void *ngx_http_ndg_filter_create_loc_conf(ngx_conf_t* cf)
{
    ngx_http_ndg_filter_loc_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_ndg_filter_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    //conf->headers = NULL;

    return conf;
}

//static char *ngx_http_ndg_filter_merge_loc_conf(
//                    ngx_conf_t *cf, void *parent, void *child)
//{
//    ngx_http_ndg_filter_loc_conf_t* prev = parent;
//    ngx_http_ndg_filter_loc_conf_t* conf = child;
//
//    ngx_conf_merge_value(conf->footer, prev->footer, "hello");
//
//    return NGX_CONF_OK;
//}

static ngx_int_t ngx_http_ndg_header_filter(ngx_http_request_t *r)
{
    ngx_http_ndg_filter_ctx_t* ctx =
            ngx_http_get_module_ctx(r, ngx_http_ndg_filter_module);

    if (ctx == NULL) {
        ctx = ngx_pcalloc(r->pool, sizeof(ngx_http_ndg_filter_ctx_t));
        if (ctx == NULL) {
            return NGX_ERROR;
        }
        ngx_http_set_ctx(r, ctx, ngx_http_ndg_filter_module);
    }

    if (ctx->flag > 0) {
        return ngx_http_next_header_filter(r);
    }

    ngx_http_ndg_filter_loc_conf_t* lcf;
    lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_filter_module);

    if (lcf->headers) {
        ngx_keyval_t* data = lcf->headers->elts;
        ngx_uint_t i;

        for (i = 0;i < lcf->headers->nelts;++i) {
            ngx_table_elt_t *h = ngx_list_push(&r->headers_out.headers);
            if (h == NULL) {
                return NGX_ERROR;
            }

            h->hash = 1;
            h->key = data[i].key;
            h->value = data[i].value;
        }

        ctx->flag = 1;
    }

    if (lcf->footer.len && r->headers_out.content_length_n > 0) {
        r->headers_out.content_length_n += lcf->footer.len;

        ctx->flag = 1;
    }

    if (ctx->flag > 0) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "header filter ok");
    }

    return ngx_http_next_header_filter(r);
}

static ngx_int_t ngx_http_ndg_body_filter(ngx_http_request_t *r,
    ngx_chain_t *in)
{
    if (in == NULL) {
        return ngx_http_next_body_filter(r, in);
    }

    ngx_http_ndg_filter_loc_conf_t* lcf;
    lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_filter_module);

    // no footer string
    if (lcf->footer.len == 0) {
        return ngx_http_next_body_filter(r, in);
    }

    // get ctx
    ngx_http_ndg_filter_ctx_t* ctx =
            ngx_http_get_module_ctx(r, ngx_http_ndg_filter_module);

    if (ctx == NULL) {
        ctx = ngx_pcalloc(r->pool, sizeof(ngx_http_ndg_filter_ctx_t));
        if (ctx == NULL) {
            return NGX_ERROR;
        }
        ngx_http_set_ctx(r, ctx, ngx_http_ndg_filter_module);
    }

    // header filter error
    if (ctx->flag != 1) {
        return ngx_http_next_body_filter(r, in);
    }

    // try to find eof
    ngx_chain_t *p;
    for (p = in; p; p = p->next) {
        if (p->buf->last_buf) {
            break;
        }
    }

    // eof not find
    if (p == NULL) {
        return ngx_http_next_body_filter(r, in);
    }

    // find eof
    ctx->flag = 2;

    ngx_buf_t* b = ngx_calloc_buf(r->pool);
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    b->pos = lcf->footer.data;
    b->last = lcf->footer.data + lcf->footer.len;
    b->memory = 1;
    b->last_buf = 1;
    b->last_in_chain = 1;

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "body filter ok");

    // set to the last node
    if (ngx_buf_size(p->buf) == 0) {
        p->buf = b;
        return ngx_http_next_body_filter(r, in);
    }

    // add a new last node
    ngx_chain_t* out = ngx_alloc_chain_link(r->pool);
    out->buf = b;
    out->next = NULL;

    // link to the new node
    p->next = out;
    p->buf->last_buf = 0;
    p->buf->last_in_chain = 0;

    return ngx_http_next_body_filter(r, in);
}

static ngx_int_t ngx_http_ndg_filter_init(ngx_conf_t* cf)
{
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_ndg_header_filter;

    ngx_http_next_body_filter = ngx_http_top_body_filter;
    ngx_http_top_body_filter = ngx_http_ndg_body_filter;

    return NGX_OK;
}

