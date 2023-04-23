// Copyright (c) 2018
// Author: Chrono Law
#include "ngx_http_ndg_advance_module.h"

static void *ngx_http_ndg_advance_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_ndg_advance_merge_loc_conf(
                ngx_conf_t *cf, void *parent, void *child);

static ngx_int_t ngx_http_ndg_advance_init(ngx_conf_t *cf);
static ngx_int_t ngx_http_ndg_advance_handler(ngx_http_request_t *r);

static void ngx_http_ndg_array_test(ngx_http_request_t *r);
static void ngx_http_ndg_list_test(ngx_http_request_t *r);
static void ngx_http_ndg_queue_test(ngx_http_request_t *r);
static void ngx_http_ndg_rbtree_test(ngx_http_request_t *r);
static void ngx_http_ndg_buf_test(ngx_http_request_t *r);
static void ngx_http_ndg_chain_test(ngx_http_request_t *r);

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
    NULL,                                  /*  preconfiguration */
    ngx_http_ndg_advance_init,             /*  postconfiguration */
    NULL,                                  /*  create main configuration */
    NULL,                                  /*  init main configuration */
    NULL,                                  /*  create server configuration */
    NULL,                                  /*  merge server configuration */
    ngx_http_ndg_advance_create_loc_conf,  /*  create location configuration */
    ngx_http_ndg_advance_merge_loc_conf,   /*  merge location configuration */
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

static void *ngx_http_ndg_advance_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_ndg_advance_loc_conf_t *conf;

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
    ngx_http_ndg_advance_loc_conf_t *prev = parent;
    ngx_http_ndg_advance_loc_conf_t *conf = child;

    ngx_conf_merge_value(conf->enable, prev->enable, 0);

    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_ndg_advance_init(ngx_conf_t *cf)
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
    ngx_http_ndg_advance_loc_conf_t *lcf;

    lcf = ngx_http_get_module_loc_conf(r, ngx_http_ndg_advance_module);

    if (lcf->enable) {

        ngx_http_ndg_array_test(r);
        ngx_http_ndg_list_test(r);
        ngx_http_ndg_queue_test(r);
        ngx_http_ndg_rbtree_test(r);
        ngx_http_ndg_buf_test(r);
        ngx_http_ndg_chain_test(r);

        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "advance ok");
    }

    return NGX_DECLINED;
}

static void ngx_http_ndg_array_test(ngx_http_request_t *r)
{
    ngx_pool_t  *pool = ngx_cycle->pool;
    ngx_array_t *arr;

    // create array
    arr = ngx_array_create(pool, 2, sizeof(ngx_uint_t));
    if (arr == NULL) {
        ngx_log_error(
            NGX_LOG_ERR, ngx_cycle->log, 0, "ngx_array_create failed");
        return;
    }

    assert(arr->nelts == 0);
    assert(arr->size == sizeof(ngx_uint_t));
    assert(arr->nalloc == 2);

    //ngx_aux.h
    assert(ngx_array_nelts(arr) == 0);
    assert(ngx_array_empty(arr));
    assert(ngx_array_capacity(arr) == 2);

    // push to array
    ngx_uint_t  i;
    ngx_uint_t *p;
    for (i = 0; i < 3; i++) {
        p = ngx_array_push(arr);
        if (p == NULL) {
            ngx_log_error(
                NGX_LOG_ERR, ngx_cycle->log, 0, "ngx_array_push failed");
            return;
        }

        *p = i;
    }
    assert(arr->nelts == 3);

    // iterate array
    ngx_uint_t *values = arr->elts;
    for (i = 0; i < arr->nelts; i++) {
        assert(values[i] == i);
    }

    // iterate array with each
    assert(!ngx_array_empty(arr));

    i = 0;
    ngx_uint_t *value;
    ngx_array_each(value, arr) {
        assert(*value == i++);
    } ngx_array_loop;

    ngx_array_init(arr, pool, 10, sizeof(ngx_str_t));

    // destroy array
    ngx_array_destroy(arr);

    ngx_log_error(NGX_LOG_ERR, ngx_cycle->log, 0, "ngx array ok");
}

static void ngx_http_ndg_list_test(ngx_http_request_t *r)
{
    ngx_pool_t  *pool = ngx_cycle->pool;
    ngx_list_t *ls;

    ls = ngx_list_create(pool, 2, sizeof(ngx_uint_t));
    if (ls == NULL) {
        ngx_log_error(
            NGX_LOG_ERR, ngx_cycle->log, 0, "ngx_list_create failed");
        return;
    }

    assert(ls->last == &ls->part);
    assert(ls->part.next == NULL);
    assert(ls->size == sizeof(ngx_uint_t));
    assert(ls->nalloc == 2);

    // push to list
    ngx_uint_t i;
    ngx_uint_t *p;
    for (i = 0; i < 5; i++) {
        p = ngx_list_push(ls);
        if (p == NULL) {
            ngx_log_error(
                NGX_LOG_ERR, ngx_cycle->log, 0, "ngx_list_push failed");
            return;
        }

        *p = i % ls->nalloc;
    }

    assert(ls->last != &ls->part);
    assert(ls->part.next != NULL);

    //ngx_uint_t i;
    ngx_list_part_t *part;
    ngx_uint_t      *data;

    for (part = &ls->part;part;part = part->next) {
        data = part->elts;
        for (i = 0;i < part->nelts; i++) {
            assert(data[i] == i);
        }
    }

    // use macro each loop
    assert(!ngx_list_empty(ls));

    ngx_uint_t *value;
    ngx_list_each(value, ls) {
        ngx_log_error(NGX_LOG_ERR, ngx_cycle->log, 0,
                        "elt = %ud in each", *value);
    } ngx_list_loop;

    ngx_log_error(NGX_LOG_ERR, ngx_cycle->log, 0, "ngx list ok");
}

typedef struct {
    int         x;
    ngx_queue_t link;
} info_node_t;

static void ngx_http_ndg_queue_test(ngx_http_request_t *r)
{
    ngx_uint_t  i;
    info_node_t *n;
    ngx_pool_t  *pool = ngx_cycle->pool;
    ngx_queue_t     h;

    ngx_queue_init(&h);
    assert(ngx_queue_empty(&h));

    for(i = 0; i < 3; i++) {
        n = ngx_pcalloc(pool, sizeof(info_node_t));
        if (n == NULL) {
            ngx_log_error(
                NGX_LOG_ERR, ngx_cycle->log, 0, "ngx_pcalloc failed");
            return;
        }

        n->x = i;

        ngx_queue_insert_tail(&h, &n->link);
    }

    n = ngx_queue_data(ngx_queue_head(&h), info_node_t, link);
    assert(n->x == 0);
    n = ngx_queue_data(ngx_queue_last(&h), info_node_t, link);
    assert(n->x == 2);

    n = ngx_pcalloc(pool, sizeof(info_node_t));
    n->x = 3;
    ngx_queue_insert_head(&h, &n->link);

    printf("iterate queue\n");

    // iterate
    ngx_queue_t *q;
    for(q = ngx_queue_head(&h);
        q != ngx_queue_sentinel(&h);
        q = ngx_queue_next(q)) {

        n = ngx_queue_data(q, info_node_t, link);
        printf("%d", n->x);
    }
    printf("\n");

    // iterate array with each
    info_node_t *value;
    ngx_queue_each(value, &h, info_node_t, link) {
        printf("%d", value->x);
    } ngx_queue_loop;
    printf("\n");

    q = ngx_queue_last(&h);
    ngx_queue_remove(q);

    ngx_queue_insert_after(ngx_queue_head(&h), q);

    ngx_log_error(NGX_LOG_ERR, ngx_cycle->log, 0, "ngx queue ok");
}

typedef struct {
    ngx_rbtree_node_t   node;
    int                 x;
} info_rbtree_node_t;

static info_rbtree_node_t*
info_rbtree_lookup(ngx_rbtree_t *rbtree, int val, ngx_uint_t key)
{
    info_rbtree_node_t *n;
    ngx_rbtree_node_t  *node, *sentinel;

    node = rbtree->root;
    sentinel = rbtree->sentinel;

    while (node != sentinel) {
        n = (info_rbtree_node_t*) node;

        if (key != node->key) {
            node = (key < node->key) ? node->left : node->right;
            continue;
        }

        if (val != n->x) {
            node = (val < n->x) ? node->left : node->right;
            continue;
        }

        return n;
    }

    return NULL;
}

static void
info_rbtree_traverse(ngx_rbtree_node_t  *node, ngx_rbtree_node_t *sentinel)
{
    if (node == sentinel) {
        return;
    }

    info_rbtree_traverse(node->left, sentinel);

    info_rbtree_node_t *n = (info_rbtree_node_t*) node;
    printf("%d", n->x);

    info_rbtree_traverse(node->right, sentinel);
}

static void ngx_http_ndg_rbtree_test(ngx_http_request_t *r)
{
    ngx_uint_t          i;
    info_rbtree_node_t *n;
    ngx_pool_t  *pool = ngx_cycle->pool;

    ngx_rbtree_t        tree;
    ngx_rbtree_node_t  sentinel;

    // init
    ngx_rbtree_init(&tree, &sentinel, ngx_rbtree_insert_value);

    // insert
    for(i = 0; i < 5; i++) {
        n = ngx_pcalloc(pool, sizeof(info_rbtree_node_t));
        if (n == NULL) {
            ngx_log_error(
                NGX_LOG_ERR, ngx_cycle->log, 0, "ngx_pcalloc failed");
            return;
        }

        n->x = i + 1;
        n->node.key = i;
        ngx_rbtree_insert(&tree, &n->node);
    }

    // find min
    ngx_rbtree_node_t  *p;
    p = ngx_rbtree_min(tree.root, tree.sentinel);
    assert(p->key == 0);

    n = (info_rbtree_node_t*) p;
    assert(n->x == 1);

    ngx_rbtree_delete(&tree, p);

    // lookup
    n = info_rbtree_lookup(&tree, 2, 1);
    assert(n->x == 2);

    n = info_rbtree_lookup(&tree, 10, 9);
    assert(n == NULL);

    printf("traverse rbtree\n");

    //traverse
    info_rbtree_traverse(tree.root, tree.sentinel);
    printf("\n");

    ngx_log_error(NGX_LOG_ERR, ngx_cycle->log, 0, "ngx rbtree ok");
}

static void ngx_http_ndg_buf_test(ngx_http_request_t *r)
{
    ngx_buf_t   *buf;
    ngx_pool_t  *pool = ngx_cycle->pool;

    buf = ngx_create_temp_buf(pool, 256);
    if (buf == NULL) {
        ngx_log_error(
                NGX_LOG_ERR, ngx_cycle->log, 0, "ngx_create_temp_buf failed");
        return;
    }

    assert(buf->temporary);
    assert(ngx_buf_size(buf) == 0);

    assert(!ngx_buf_special(buf));
    assert(ngx_buf_in_memory(buf));
    assert(ngx_buf_in_memory_only(buf));

    ngx_str_t str = ngx_string("metroid");

    ngx_memzero(buf->start, buf->end - buf->start);
    buf->last = ngx_cpymem(buf->pos, str.data, str.len);
    assert(ngx_buf_size(buf) == (off_t)str.len);

    ngx_log_error(NGX_LOG_ERR, ngx_cycle->log, 0, "ngx buf ok");
}

static void ngx_http_ndg_chain_test(ngx_http_request_t *r)
{
    ngx_buf_t   *buf;
    ngx_chain_t *ch;
    ngx_pool_t  *pool = ngx_cycle->pool;
    ngx_str_t    str = ngx_string("echoes");

    buf = ngx_calloc_buf(pool);
    if (buf == NULL) {
        ngx_log_error(
                NGX_LOG_ERR, ngx_cycle->log, 0, "ngx_calloc_buf failed");
        return;
    }

    buf->start = buf->pos = str.data;
    buf->end = buf->last = str.data + str.len;
    buf->memory = 1;

    ch = ngx_alloc_chain_link(pool);
    if (ch == NULL) {
        ngx_log_error(
                NGX_LOG_ERR, ngx_cycle->log, 0, "ngx_alloc_chain_link failed");
        return;
    }

    ch->buf = buf;
    ch->next = NULL;

    ngx_free_chain(pool, ch);

    ngx_log_error(NGX_LOG_ERR, ngx_cycle->log, 0, "ngx chain ok");
}
