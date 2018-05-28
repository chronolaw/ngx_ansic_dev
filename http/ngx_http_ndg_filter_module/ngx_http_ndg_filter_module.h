// Copyright (c) 2018
// Author: Chrono Law
#ifndef _NGX_HTTP_NDG_FILTER_MODULE_H_INCLUDED_
#define _NGX_HTTP_NDG_FILTER_MODULE_H_INCLUDED_

#include <nginx.h>
#include <ngx_http.h>
#include <assert.h>

typedef struct {
    ngx_array_t* headers;
    ngx_str_t   footer;
} ngx_http_ndg_filter_loc_conf_t;

// 0 - init; 1 - header ok; 2 - body ok
typedef struct {
    int flag;
} ngx_http_ndg_filter_ctx_t;

#endif  //_NGX_HTTP_NDG_FILTER_MODULE_H_INCLUDED_
