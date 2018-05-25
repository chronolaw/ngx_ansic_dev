// Copyright (c) 2018
// Author: Chrono Law
#ifndef _NGX_HTTP_NDG_ECHO_MODULE_H_INCLUDED_
#define _NGX_HTTP_NDG_ECHO_MODULE_H_INCLUDED_

#include <nginx.h>
#include <ngx_http.h>
#include <assert.h>

typedef struct {
    ngx_str_t msg;
} ngx_http_ndg_echo_loc_conf_t;

#endif  //_NGX_HTTP_NDG_ECHO_MODULE_H_INCLUDED_
