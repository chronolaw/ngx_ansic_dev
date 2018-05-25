// Copyright (c) 2018
// Author: Chrono Law
#include "ngx_http_ndg_advance_module.h"

static void *ngx_http_ndg_advance_create_loc_conf(ngx_conf_t* cf);
static char *ngx_http_ndg_advance_merge_loc_conf(
                ngx_conf_t *cf, void *parent, void *child);

static ngx_int_t ngx_http_ndg_advance_init(ngx_conf_t* cf);
static ngx_int_t ngx_http_ndg_advance_handler(ngx_http_request_t *r);

static void ngx_http_ndg_array_test(ngx_http_request_t *r);
static void ngx_http_ndg_list_test(ngx_http_request_t *r);

static ngx_command_t ngx_http_ndg_advance_cmds[] =
{
    {
        ngx_string("ndg_advance"),
        NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
        ngx_conf_set_flag_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_ndg_advance_loc_conf_t, enable),
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t ngx_http_ndg_advance_module_ctx =
{
    NULL,                                       /*  preconfiguration */
    ngx_http_ndg_advance_init,                  /*  postconfiguration */
    NULL,                                       /*  create main configuration */
    NULL,                                       /*  init main configuration */
    NULL,                                       /*  create server configuration */
    NULL,                                       /*  merge server configuration */
    ngx_http_ndg_advance_create_loc_conf,       /*  create location configuration */
    ngx_http_ndg_advance_merge_loc_conf,        /*  merge location configuration */
};

ngx_module_t ngx_http_ndg_advance_module =
{
    NGX_MODULE_V1,
    &ngx_http_ndg_advance_module_ctx,           // module context
    ngx_http_ndg_advance_cmds,                  // module directives
    NGX_HTTP_MODULE,                            // module type
    NULL,                                       // init master
    NULL,                                       // init module
    NULL,                                       // init process
    NULL,                                       // init thread
    NULL,                                       // exit thread
    NULL,                                       // exit process
    NULL,                                       // exit master
    NGX_MODULE_V1_PADDING
};

static void *ngx_http_ndg_advance_create_loc_conf(ngx_conf_t* cf)
{
    ngx_http_ndg_advance_loc_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_ndg_advance_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    conf->enable = NGX_CONF_UNSET;

    return conf;
}

static char *ngx_http_ndg_advance_merge_loc_conf(
                    ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_ndg_advance_loc_conf_t* prev = parent;
    ngx_http_ndg_advance_loc_conf_t* conf = child;

    ngx_conf_merge_value(conf->enable, prev->enable, 0);

    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_ndg_advance_init(ngx_conf_t* cf)
{
    ngx_http_handler_pt        *h;
    ngx_http_core_main_conf_t  *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_REWRITE_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_ndg_advance_handler;

    return NGX_OK;
}

static ngx_int_t ngx_http_ndg_advance_handler(ngx_http_request_t *r)
{
    ngx_http_ndg_advance_loc_conf_t* lcf;

    lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_advance_module);

    if (lcf->enable) {

        ngx_http_ndg_array_test(r);
        ngx_http_ndg_list_test(r);

        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "advance ok");
    }

    return NGX_DECLINED;
}

static void ngx_http_ndg_array_test(ngx_http_request_t *r)
{
    ngx_array_t* arr;

    // create array
    arr = ngx_array_create(r->pool, 4, sizeof(ngx_uint_t));
    if ( arr == NULL) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx_array_create failed");
        return;
    }

    assert(arr->nelts == 0);
    assert(arr->size == sizeof(ngx_uint_t));
    assert(arr->nalloc == 4);

    // push to array
    ngx_uint_t i;
    ngx_uint_t* p;
    for (i = 0; i < 3; ++i) {
        p = ngx_array_push(arr);
        if ( p == NULL) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx_array_push failed");
            return;
        }

        *p = i;
    }
    assert(arr->nelts == 3);

    // iterate array
    ngx_uint_t* data = arr->elts;
    for (i = 0; i < arr->nelts; ++i) {
        assert(data[i] == i);
    }

    // destroy array
    ngx_array_destroy(arr);

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx_array ok");
}

static void ngx_http_ndg_list_test(ngx_http_request_t *r)
{
    ngx_list_t* ls;

    ls = ngx_list_create(r->pool, 2, sizeof(ngx_uint_t));
    if (ls == NULL) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx_list_create failed");
        return;
    }

    assert(ls->last == &ls->part);
    assert(ls->part.next == NULL);
    assert(ls->size == sizeof(ngx_uint_t));
    assert(ls->nalloc == 2);

    // push to list
    ngx_uint_t i;
    ngx_uint_t* p;
    for (i = 0; i < 5; ++i) {
        p = ngx_list_push(ls);
        if ( p == NULL) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx_list_push failed");
            return;
        }

        *p = i % ls->nalloc;
    }

    assert(ls->last != &ls->part);
    assert(ls->part.next != NULL);

    //ngx_uint_t i;
    ngx_list_part_t* part;
    ngx_uint_t* data;

    for(part = &ls->part;part;part = part->next) {
        data = part->elts;
        for(i = 0;i < part->nelts; ++i) {
            assert(data[i] == i);
        }
    }

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx_list ok");
}
