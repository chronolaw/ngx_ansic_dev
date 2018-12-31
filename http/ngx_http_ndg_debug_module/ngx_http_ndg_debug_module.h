// Copyright (c) 2018
// Author: Chrono Law

#ifndef _NGX_HTTP_NDG_DEBUG_MODULE_H_INCLUDED_
#define _NGX_HTTP_NDG_DEBUG_MODULE_H_INCLUDED_

#include <nginx.h>
#include <ngx_http.h>
#include <assert.h>

typedef struct {
    ngx_flag_t enable;
} ngx_http_ndg_debug_loc_conf_t;

#endif  //_NGX_HTTP_NDG_DEBUG_MODULE_H_INCLUDED_
