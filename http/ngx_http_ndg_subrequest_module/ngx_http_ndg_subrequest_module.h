// Copyright (c) 2018
// Author: Chrono Law
#ifndef _NGX_HTTP_NDG_SUBREQUEST_MODULE_H_INCLUDED_
#define _NGX_HTTP_NDG_SUBREQUEST_MODULE_H_INCLUDED_

#include <nginx.h>
#include <ngx_http.h>
#include <assert.h>

#if nginx_version < 1013010
    #error "need latest nginx!"
#endif

typedef struct {
    ngx_str_t uri;
} ngx_http_ndg_subrequest_loc_conf_t;

typedef struct {
    ngx_http_request_t *sr;
} ngx_http_ndg_subrequest_ctx_t;


#endif  //_NGX_HTTP_NDG_SUBREQUEST_MODULE_H_INCLUDED_
