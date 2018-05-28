// Copyright (c) 2018
// Author: Chrono Law
#include "ngx_http_ndg_var_module.h"

static void *ngx_http_ndg_var_create_loc_conf(ngx_conf_t* cf);
//static char *ngx_http_ndg_var_merge_loc_conf(
//                ngx_conf_t *cf, void *parent, void *child);

static char* ngx_http_ndg_echo_v(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char* ngx_http_ndg_echo_cv(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_int_t ngx_http_ndg_var_handler(ngx_http_request_t *r);

static ngx_command_t ngx_http_ndg_var_cmds[] =
{
    {
        ngx_string("ndg_echo_v"),
        NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_http_ndg_echo_v,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_ndg_var_loc_conf_t, v),
        NULL
    },

    {
        ngx_string("ndg_echo_cv"),
        NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_http_ndg_echo_cv,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_ndg_var_loc_conf_t, cv),
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t ngx_http_ndg_var_module_ctx =
{
    NULL,                                   /*  preconfiguration */
    NULL,                                   /*  postconfiguration */
    NULL,                                   /*  create main configuration */
    NULL,                                   /*  init main configuration */
    NULL,                                   /*  create server configuration */
    NULL,                                   /*  merge server configuration */
    ngx_http_ndg_var_create_loc_conf,       /*  create location configuration */
    NULL,                                   /*  merge location configuration */
};

ngx_module_t ngx_http_ndg_var_module =
{
    NGX_MODULE_V1,
    &ngx_http_ndg_var_module_ctx,           // module context
    ngx_http_ndg_var_cmds,                  // module directives
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

static void *ngx_http_ndg_var_create_loc_conf(ngx_conf_t* cf)
{
    ngx_http_ndg_var_loc_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_ndg_var_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    return conf;
}

//static char *ngx_http_ndg_var_merge_loc_conf(
//                    ngx_conf_t *cf, void *parent, void *child)
//{
//    ngx_http_ndg_var_loc_conf_t* prev = parent;
//    ngx_http_ndg_var_loc_conf_t* conf = child;
//
//    ngx_conf_merge_str_value(conf->msg, prev->msg, "hello");
//
//    return NGX_CONF_OK;
//}

static char* ngx_http_ndg_echo_v(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t    *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    if (clcf == NULL) {
        return NGX_CONF_ERROR;
    }

    if (clcf->handler) {
        return "is duplicate";
    }

    clcf->handler = ngx_http_ndg_var_handler;

    // set var

    char* rc = ngx_conf_set_str_slot(cf, cmd, conf);
    if ( rc != NGX_CONF_OK) {
        return rc;
    }

    //char* p = conf;
    //ngx_str_t* v = (ngx_str_t*)(p + cmd->offset);
    ngx_http_ndg_var_loc_conf_t* lcf = conf;

    if ( lcf->v.data[0] != '$') {
        return NGX_CONF_ERROR;
    }

    lcf->v.data++;
    lcf->v.len--;

    return NGX_CONF_OK;
}

static char* ngx_http_ndg_echo_cv(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t    *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    if (clcf == NULL) {
        return NGX_CONF_ERROR;
    }

    if (clcf->handler) {
        return "is duplicate";
    }

    clcf->handler = ngx_http_ndg_var_handler;

    // set complex var

    ngx_str_t* value = cf->args->elts;
    ngx_http_ndg_var_loc_conf_t* lcf = conf;

    ngx_http_compile_complex_value_t ccv;
    ngx_memzero(&ccv, sizeof(ngx_http_compile_complex_value_t));

    ccv.cf = cf;
    ccv.value = &value[1];
    ccv.complex_value = &lcf->cv;

    if (ngx_http_compile_complex_value(&ccv) != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_ndg_var_handler(ngx_http_request_t *r)
{
    ngx_int_t   rc;

    if (!(r->method & NGX_HTTP_GET)) {
        return NGX_HTTP_NOT_ALLOWED;
    }

    rc = ngx_http_discard_request_body(r);
    if (rc != NGX_OK) {
        return rc;
    }

    ngx_str_t msg;

    ngx_http_ndg_var_loc_conf_t* lcf;
    lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_var_module);

    if (lcf->v.len) {
        // use single var

        ngx_http_variable_value_t   *vv;

        ngx_uint_t key = ngx_hash_strlow(lcf->v.data, lcf->v.data, lcf->v.len);

        vv = ngx_http_get_variable(r, &lcf->v, key);

        if (vv == NULL || vv->not_found) {
            return NGX_ERROR;
        }

        msg.data = vv->data;
        msg.len = vv->len;

        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "echo var ok");
    } else {
        // use complex var

        if (ngx_http_complex_value(r, &lcf->cv, &msg) != NGX_OK) {
            return NGX_ERROR;
        }

        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "echo cv ok");
    }

    size_t len = msg.len;
    if (r->args.len) {
        len += r->args.len + 1;     // args + ','
    }

    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = len;  //lcf->msg.len;

    rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }

    ngx_buf_t* b = ngx_create_temp_buf(r->pool, len);
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    if (r->args.len) {
        b->last = ngx_snprintf(b->pos, len, "%V,%V", &r->args, &msg);
    } else {
        b->last = ngx_snprintf(b->pos, len, "%V", &msg);
    }

    b->last_buf = 1;
    b->last_in_chain = 1;

    ngx_chain_t* out = ngx_alloc_chain_link(r->pool);
    out->buf = b;
    out->next = NULL;

    rc = ngx_http_output_filter(r, out);


    return rc;  //NGX_OK;
}

