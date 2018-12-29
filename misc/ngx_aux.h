// Copyright (c) 2018 chronolaw

#ifndef _NGX_AUX_H_INCLUDED_
#define _NGX_AUX_H_INCLUDED_

/* ------------------------------------------------------------------------- */

// ngx_array_t aux

#define ngx_array_nelts(array)                                                \
    ((array)->nelts)

#define ngx_array_reset(array)                                                \
    (array)->nelts = 0;

#define ngx_array_empty(array)                                                \
    ((array)->nelts == 0)

#define ngx_array_capacity(array)                                             \
    ((array)->nalloc)

// usage:
//    some_type *value;
//    ngx_array_each(value, array) {
//        do something ...
//    } ngx_array_loop;

#define ngx_array_each(elt, array)                                            \
    do {                                                                      \
        void  *_end;                                                          \
        elt = (array)->elts;                                                  \
                                                                              \
        for (_end = (elt + (array)->nelts); elt != _end; elt++) {             \

#define ngx_array_loop                                                        \
        }                                                                     \
    } while (0)

/* ------------------------------------------------------------------------- */

// ngx_list_t aux

#define ngx_list_empty(list)                                                \
    ((list)->part.nelts == 0)

// usage:
//    some_type *value;
//    ngx_list_each(value, list) {
//        do something ...
//    } ngx_list_loop;

#define ngx_list_each(elt, list)                                              \
    do {                                                                      \
        void             *_end;                                               \
        ngx_list_part_t  *_part;                                              \
                                                                              \
        for (_part = &((list)->part); _part; _part = _part->next) {           \
            elt = _part->elts;                                                \
                                                                              \
            for (_end = (elt + _part->nelts); elt != _end; elt++) {           \

#define ngx_list_loop                                                         \
            }                                                                 \
        }                                                                     \
    } while (0)

/* ------------------------------------------------------------------------- */

// ngx_queue_t aux

#define ngx_queue_link_t            ngx_queue_t

#define ngx_queue_first             ngx_queue_head

#define ngx_queue_insert_before(q, x)                                         \
    ngx_queue_insert_after((q)->prev, x)

// usage:
//    some_type *value;
//    ngx_queue_each(value, queue) {
//        do something ...
//    } ngx_queue_loop;

#define ngx_queue_each(elt, queue, type, link)                                \
    do {                                                                      \
        ngx_queue_t  *_lnk;                                                   \
                                                                              \
        for (_lnk = ngx_queue_head(queue);                                    \
             _lnk != ngx_queue_sentinel(queue);                               \
             _lnk = ngx_queue_next(_lnk)) {                                   \
                                                                              \
            elt = ngx_queue_data(_lnk, type, link);                           \

#define ngx_queue_loop                                                        \
        }                                                                     \
    } while(0)

/* ------------------------------------------------------------------------- */

// ngx_rbtree_t aux

#define ngx_rbtree_data     ngx_queue_data

#endif  // _NGX_AUX_H_INCLUDED_

