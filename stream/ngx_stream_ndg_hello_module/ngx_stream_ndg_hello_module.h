// Copyright (c) 2018
// Author: Chrono Law

#ifndef _NGX_STREAM_NDG_HELLO_MODULE_H_INCLUDED_
#define _NGX_STREAM_NDG_HELLO_MODULE_H_INCLUDED_

#include <nginx.h>
#include <ngx_stream.h>
#include <assert.h>

typedef struct {
    ngx_flag_t enable;
} ngx_stream_ndg_hello_srv_conf_t;

#endif  //_NGX_STREAM_NDG_HELLO_MODULE_H_INCLUDED_
