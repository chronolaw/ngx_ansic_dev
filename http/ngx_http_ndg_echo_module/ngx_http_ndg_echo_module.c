// Copyright (c) 2018
// Author: Chrono Law
#include "ngx_http_ndg_echo_module.h"

static void *ngx_http_ndg_echo_create_loc_conf(ngx_conf_t* cf);
//static char *ngx_http_ndg_echo_merge_loc_conf(
//                ngx_conf_t *cf, void *parent, void *child);

static char *ngx_http_ndg_echo(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_int_t ngx_http_ndg_echo_handler(ngx_http_request_t *r);

static ngx_command_t ngx_http_ndg_echo_cmds[] =
{
    {
        ngx_string("ndg_echo"),
        NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_http_ndg_echo,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_ndg_echo_loc_conf_t, msg),
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t ngx_http_ndg_echo_module_ctx =
{
    NULL,                               /*  preconfiguration */
    NULL,                               /*  postconfiguration */
    NULL,                               /*  create main configuration */
    NULL,                               /*  init main configuration */
    NULL,                               /*  create server configuration */
    NULL,                               /*  merge server configuration */
    ngx_http_ndg_echo_create_loc_conf,  /*  create location configuration */
    NULL,                               /*  merge location configuration */
};

ngx_module_t ngx_http_ndg_echo_module =
{
    NGX_MODULE_V1,
    &ngx_http_ndg_echo_module_ctx,          // module context
    ngx_http_ndg_echo_cmds,                 // module directives
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

static void *ngx_http_ndg_echo_create_loc_conf(ngx_conf_t* cf)
{
    ngx_http_ndg_echo_loc_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_ndg_echo_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    return conf;
}

//static char *ngx_http_ndg_echo_merge_loc_conf(
//                    ngx_conf_t *cf, void *parent, void *child)
//{
//    ngx_http_ndg_echo_loc_conf_t* prev = parent;
//    ngx_http_ndg_echo_loc_conf_t* conf = child;
//
//    ngx_conf_merge_str_value(conf->msg, prev->msg, "hello");
//
//    return NGX_CONF_OK;
//}

static char *ngx_http_ndg_echo(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
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

    clcf->handler = ngx_http_ndg_echo_handler;

    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_ndg_echo_handler(ngx_http_request_t *r)
{
    ngx_int_t   rc;
    ngx_http_ndg_echo_loc_conf_t *lcf;
    size_t      len;
    ngx_buf_t   *b;
    ngx_chain_t *out;

    if (!(r->method & NGX_HTTP_GET)) {
        return NGX_HTTP_NOT_ALLOWED;
    }

    rc = ngx_http_discard_request_body(r);
    if (rc != NGX_OK) {
        return rc;
    }

    lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_echo_module);

    len = lcf->msg.len;
    if (r->args.len) {
        len += r->args.len + 1;     // args + ','
    }

    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = len;  //lcf->msg.len;

    rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }

    //ngx_buf_t* b = ngx_calloc_buf(r->pool);
    b = ngx_create_temp_buf(r->pool, len);
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

#if 0
    if (r->args.len) {
        b->last = ngx_snprintf(b->pos, len, "%V,%V", &r->args, &lcf->msg);
    } else {
        b->last = ngx_snprintf(b->pos, len, "%V", &lcf->msg);
    }
#endif
    if (r->args.len) {
        //b->last = ngx_slprintf(b->pos, b->end, "%V,", &r->args);
        b->last = ngx_cpymem(b->pos, r->args.data, r->args.len);

        //*b->last = ',';
        //b->last++;
        *(b->last++) = ',';
    }
    b->last = ngx_cpymem(b->last, lcf->msg.data, lcf->msg.len);

    //b->pos = lcf->msg.data;
    //b->last = lcf->msg.data + lcf->msg.len;
    //b->memory = 1;

    b->last_buf = 1;
    b->last_in_chain = 1;

    out = ngx_alloc_chain_link(r->pool);
    out->buf = b;
    out->next = NULL;

    rc = ngx_http_output_filter(r, out);

    //ngx_log_t* log = r->connection->log;
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "echo ok");

    return rc;  //NGX_OK;
}

