// Copyright (c) 2018
// Author: Chrono Law
#include "ngx_http_ndg_var_module.h"

static void *ngx_http_ndg_var_create_loc_conf(ngx_conf_t* cf);
//static char *ngx_http_ndg_var_merge_loc_conf(
//                ngx_conf_t *cf, void *parent, void *child);

#if 0
static char *ngx_http_ndg_echo_v(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char *ngx_http_ndg_echo_cv(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
#endif

static ngx_int_t ngx_http_ndg_add_variables(ngx_conf_t *cf);
static ngx_int_t ngx_http_current_method_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);

static char *ngx_http_ndg_complex_value(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_int_t ngx_http_ndg_var_init(ngx_conf_t* cf);
static ngx_int_t ngx_http_ndg_var_handler(ngx_http_request_t *r);

static ngx_command_t ngx_http_ndg_var_cmds[] =
{
#if 0
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
#endif

    {
        ngx_string("ndg_complex_value"),
        NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_http_ndg_complex_value,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t ngx_http_ndg_var_module_ctx =
{
    ngx_http_ndg_add_variables,             /*  preconfiguration */
    ngx_http_ndg_var_init,                  /*  postconfiguration */
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

static ngx_http_variable_t  ngx_http_ndg_vars[] = {
    {
        ngx_string("current_method"), NULL,
        ngx_http_current_method_variable, 0,
        NGX_HTTP_VAR_NOCACHEABLE, 0
    },

    ngx_http_null_variable
};

static ngx_int_t ngx_http_ndg_add_variables(ngx_conf_t *cf)
{
    ngx_http_variable_t  *var, *v;

    for (v = ngx_http_ndg_vars; v->name.len; v++) {
        var = ngx_http_add_variable(cf, &v->name, v->flags);
        if (var == NULL) {
            return NGX_ERROR;
        }

        var->get_handler = v->get_handler;
        var->data = v->data;
    }

    return NGX_OK;
}

static ngx_int_t ngx_http_current_method_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    v->len = r->method_name.len;
    v->data = r->method_name.data;

    v->valid = 1;
    v->not_found = 0;
    v->no_cacheable = 0;

    return NGX_OK;
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

static ngx_int_t ngx_http_ndg_var_init(ngx_conf_t* cf)
{
    ngx_http_handler_pt        *h;
    ngx_http_core_main_conf_t  *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_LOG_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_ndg_var_handler;

    return NGX_OK;
}

static char *ngx_http_ndg_complex_value(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_str_t* value = cf->args->elts;
    ngx_http_ndg_var_loc_conf_t* lcf = conf;

    lcf->cv = ngx_pcalloc(cf->pool, sizeof(ngx_http_complex_value_t));
    if (lcf->cv == NULL) {
        return NGX_CONF_ERROR;
    }

    ngx_http_compile_complex_value_t ccv;
    ngx_memzero(&ccv, sizeof(ngx_http_compile_complex_value_t));

    ccv.cf = cf;
    ccv.value = &value[1];
    ccv.complex_value = lcf->cv;

    if (ngx_http_compile_complex_value(&ccv) != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_ndg_var_handler(ngx_http_request_t *r)
{
    // log var
    if (r->args.len) {
        ngx_str_t name;

        name.len = r->args.len;
        name.data = ngx_pstrdup(r->connection->pool, &r->args);

        ngx_uint_t key = ngx_hash_strlow(name.data, name.data, name.len);

        ngx_http_variable_value_t *vv = ngx_http_get_variable(r, &name, key);

        if (vv == NULL || vv->not_found) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "log var %V failed", &name);
            return NGX_ERROR;
        }

        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "log var %v ok", vv);
    }

    // log complex var
    ngx_http_ndg_var_loc_conf_t* lcf;
    lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_var_module);

    if (lcf->cv) {
        ngx_str_t msg;

        if (ngx_http_complex_value(r, lcf->cv, &msg) != NGX_OK) {
            return NGX_ERROR;
        }

        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "log complex %V ok", &msg);
    }

    return NGX_OK;
}

#if 0

static char *ngx_http_ndg_echo_v(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
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
    if (rc != NGX_CONF_OK) {
        return rc;
    }

    //char* p = conf;
    //ngx_str_t* v = (ngx_str_t*)(p + cmd->offset);
    ngx_http_ndg_var_loc_conf_t* lcf = conf;

    if (lcf->v.data[0] != '$') {
        return NGX_CONF_ERROR;
    }

    lcf->v.data++;
    lcf->v.len--;

    return NGX_CONF_OK;
}

static char *ngx_http_ndg_echo_cv(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
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

#endif
