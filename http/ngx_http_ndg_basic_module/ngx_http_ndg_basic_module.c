// Copyright (c) 2018
// Author: Chrono Law
#include <ngx_md5.h>
#include <ngx_sha1.h>

#include "ngx_http_ndg_basic_module.h"

static void *ngx_http_ndg_basic_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_ndg_basic_merge_loc_conf(
                ngx_conf_t *cf, void *parent, void *child);

static ngx_int_t ngx_http_ndg_basic_init(ngx_conf_t *cf);
static ngx_int_t ngx_http_ndg_basic_handler(ngx_http_request_t *r);

static void ngx_http_ndg_int_test(ngx_http_request_t *r);
static ngx_int_t ngx_http_ndg_pool_test(ngx_http_request_t *r);
static void ngx_http_ndg_string_test(ngx_http_request_t *r);
static void ngx_http_ndg_time_test(ngx_http_request_t *r);
static void ngx_http_ndg_date_test(ngx_http_request_t *r);
static void ngx_http_ndg_log_test(ngx_http_request_t *r);
static void ngx_http_ndg_hash_test(ngx_http_request_t *r);
static void ngx_http_ndg_code_test(ngx_http_request_t *r);

static ngx_command_t ngx_http_ndg_basic_cmds[] =
{
    {
        ngx_string("ndg_basic"),
        NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
        ngx_conf_set_flag_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_ndg_basic_loc_conf_t, enable),
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t ngx_http_ndg_basic_module_ctx =
{
    NULL,                                   /*  preconfiguration */
    ngx_http_ndg_basic_init,                /*  postconfiguration */
    NULL,                                   /*  create main configuration */
    NULL,                                   /*  init main configuration */
    NULL,                                   /*  create server configuration */
    NULL,                                   /*  merge server configuration */
    ngx_http_ndg_basic_create_loc_conf,     /*  create location configuration */
    ngx_http_ndg_basic_merge_loc_conf,      /*  merge location configuration */
};

ngx_module_t ngx_http_ndg_basic_module =
{
    NGX_MODULE_V1,
    &ngx_http_ndg_basic_module_ctx,         // module context
    ngx_http_ndg_basic_cmds,                // module directives
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

static void *ngx_http_ndg_basic_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_ndg_basic_loc_conf_t *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_ndg_basic_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    conf->enable = NGX_CONF_UNSET;

    return conf;
}

static char *ngx_http_ndg_basic_merge_loc_conf(
    ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_ndg_basic_loc_conf_t *prev = parent;
    ngx_http_ndg_basic_loc_conf_t *conf = child;

    ngx_conf_merge_value(conf->enable, prev->enable, 0);

    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_ndg_basic_init(ngx_conf_t *cf)
{
    ngx_http_handler_pt        *h;
    ngx_http_core_main_conf_t  *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_REWRITE_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_ndg_basic_handler;

    return NGX_OK;
}

static ngx_int_t ngx_http_ndg_basic_handler(ngx_http_request_t *r)
{
    ngx_http_ndg_basic_loc_conf_t *lcf;

    lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_basic_module);

    if (lcf->enable) {

        ngx_http_ndg_int_test(r);
        ngx_http_ndg_pool_test(r);
        ngx_http_ndg_string_test(r);
        ngx_http_ndg_time_test(r);
        ngx_http_ndg_date_test(r);
        ngx_http_ndg_log_test(r);
        ngx_http_ndg_hash_test(r);
        ngx_http_ndg_code_test(r);

        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "basic ok");
    }

    return NGX_DECLINED;
}

// test cases below

static void ngx_http_ndg_int_test(ngx_http_request_t *r)
{
    size_t      a = 0;
    ngx_uint_t  b = NGX_CONF_UNSET_UINT;
    ngx_msec_t  c = NGX_CONF_UNSET_MSEC;

    ngx_conf_init_size_value(a, 1024);
    assert(a == 0);

    ngx_conf_init_uint_value(b, 256);
    assert(b == 256);

    ngx_conf_merge_msec_value(c, 10, 100);
    assert(c == 10);

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx int ok");
}

static ngx_int_t ngx_http_ndg_pool_test(ngx_http_request_t *r)
{
    void        *ptr;
    ngx_pool_t  *pool = ngx_cycle->pool;

    ptr = ngx_pcalloc(pool, NGX_PTR_SIZE * 10);
    if (ptr == NULL) {
        return NGX_ERROR;
    }

    ptr = ngx_pmemalign(pool, 5000, 64);
    if (ptr == NULL) {
        return NGX_ABORT;
    }

    ngx_pfree(pool, ptr);

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx pool ok");

    return NGX_OK;
}

static void ngx_http_ndg_string_test(ngx_http_request_t *r)
{
    ngx_str_t s1 = ngx_null_string;
    ngx_str_t s2 = ngx_string("matrix");
    ngx_str_t s3 = ngx_string(NGINX_VER);

    assert(s1.data == NULL && s1.len == 0);
    assert(s2.data && s2.len == 6);

    ngx_str_set(&s1, "reloaded");
    ngx_str_null(&s2);

    assert(s1.data && s1.len == 8);
    assert(s2.data == NULL && s2.len == 0);

    // strcmp
    assert(ngx_strcmp(s1.data, s3.data) > 0);
    assert(ngx_strnstr(s3.data, "nginx", 10));

    // aoti
    ngx_str_t s4 = ngx_string("256");
    ngx_str_t s5 = ngx_string("A0");
    assert(ngx_atoi(s4.data, s4.len) == 256);
    assert(ngx_hextoi(s5.data, s5.len) == 0xa0);

    // strdup
    s2.data = ngx_pstrdup(ngx_cycle->pool, &s1);
    s2.len = s1.len;
    assert(ngx_strncmp(s1.data, s2.data, s1.len) == 0);

    // sprintf
    s2.len = ngx_snprintf(s2.data, s2.len, "hello") - s2.data;
    assert(s2.len == 5);

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx str ok");
}

static void ngx_http_ndg_time_test(ngx_http_request_t *r)
{
    ngx_time_t *now = ngx_timeofday();
    ngx_msec_t msec = ngx_current_msec;
    time_t sec = ngx_time();

    assert(now->sec == sec);
    //assert(now->gmtoff == 8 * 60);

    ngx_time_update();

    // ngx_monotonic_time
    (void) msec;
    //assert(msec == now->sec * 1000 + now->msec);

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx time ok");
}

static void ngx_http_ndg_date_test(ngx_http_request_t *r)
{
    time_t   t, t2;
    ngx_tm_t tm;

    t = ngx_time();
    ngx_localtime(t, &tm);

    assert(tm.tm_year >= 2018);

    u_char buf[100];
    *ngx_http_time(buf, t) = '\0';
    printf("%s", (char*)buf);

    t2 = ngx_parse_http_time(buf, ngx_strlen(buf));
    assert(t == t2);

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx date ok");
}

static void ngx_http_ndg_log_test(ngx_http_request_t *r)
{
    ngx_log_t *log;
    log = ngx_cycle->log;

    ngx_log_error(NGX_LOG_DEBUG, log, 0, "debug");
    ngx_log_error(NGX_LOG_INFO, log, 0, "hello");
    ngx_log_error(NGX_LOG_NOTICE, log, 0, "check it");
    ngx_log_error(NGX_LOG_WARN, log, 0, "warning");
    ngx_log_error(NGX_LOG_ERR, log, 0, "error accured");
    //ngx_log_error(NGX_LOG_ALERT, log, 0, "ALERT!!");

    ngx_int_t       i   = -100;
    unsigned long   ul  = 65535L;
    double          f   = 0.618;
    ngx_str_t       str = ngx_string("metroid");
    char            cstr[] = "prime";

    ngx_log_error(NGX_LOG_ERR, log, 0,
        "%i,%uL,%.5f,%V,%uxz,%Xp,%P",
        i, ul, f, &str, ul, str.data,ngx_getpid()
        );

    ngx_log_error(NGX_LOG_ERR, log, 0,
        "%%,%T,%s,%c,%N",
        ngx_time(), cstr, cstr[0]
        );
}

static void ngx_http_ndg_hash_test(ngx_http_request_t *r)
{
    ngx_uint_t      h1, h2;
    uint32_t        key, hash1, hash2;
    ngx_str_t       s = ngx_string("abcd");
    ngx_str_t       cs = ngx_string("ABCD");
    u_char          buf[20];

    // times 33
    h1 = ngx_hash_key_lc(cs.data, cs.len);
    h2 = ngx_hash_strlow(buf, cs.data, cs.len);
    assert(h1 == h2);
    assert(ngx_strncmp(buf, s.data, s.len)==0);

    //crc32
    hash1 = ngx_crc32_short(s.data, s.len);
    hash2 = ngx_crc32_long(s.data, s.len);
    assert(hash1 == hash2);

    ngx_crc32_init(key);
    ngx_crc32_update(&key, s.data, 3);
    ngx_crc32_update(&key, s.data+3, 1);
    ngx_crc32_final(key);

    assert(key == hash1);

    // murmur

    //ngx_str_t str = ngx_string("heroes");
    //key = ngx_murmur_hash2(str.data, str.len);
    key = ngx_murmur_hash2((u_char*)"heroes", 6);

    // md5
    ngx_md5_t   md5;
    //u_char      buf[16];
    //u_char      buf[20];

    ngx_md5_init(&md5);
    ngx_md5_update(&md5, "metroid", 7);
    ngx_md5_final(buf, &md5);

    // sha1
    ngx_sha1_t  sha;

    ngx_sha1_init(&sha);
    ngx_sha1_update(&sha, "prime", 5);
    ngx_sha1_final(buf, &sha);

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx hash ok");
#if 0
    ngx_uint_t  i;
    ngx_msec_t  msec;
    ngx_str_t   str = ngx_string("12345678901234567890123456789012");

    ngx_time_update();
    msec = ngx_current_msec;

    for(i = 0; i < 100*1000; i++) {
        ngx_crc32_short(str.data, str.len);
    }

    ngx_time_update();
    ngx_log_error(
            NGX_LOG_ERR, r->connection->log, 0,
            "crc time = %ud", ngx_current_msec - msec);

    msec = ngx_current_msec;

    for(i = 0; i < 100*1000; i++) {
        ngx_murmur_hash2(str.data, str.len);
    }

    ngx_time_update();
    ngx_log_error(
            NGX_LOG_ERR, r->connection->log, 0,
            "mur time = %ud", ngx_current_msec - msec);
#endif
}

static void ngx_http_ndg_code_test(ngx_http_request_t *r)
{
    ngx_str_t src = ngx_string("mario");
    ngx_str_t dst;

    dst.len = ngx_base64_encoded_length(src.len);
    dst.data = ngx_palloc(ngx_cycle->pool, dst.len);

    ngx_encode_base64(&dst, &src);
    ngx_encode_base64url(&dst, &src);

    ngx_str_t html = ngx_string("<html>");
    ngx_str_t out;

    out.len = html.len +
        ngx_escape_html(NULL, html.data, html.len);
    out.data = ngx_palloc(ngx_cycle->pool, out.len);

    ngx_escape_html(out.data, html.data, html.len);

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx code ok");
}
