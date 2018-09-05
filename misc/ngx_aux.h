// Copyright (c) 2018 chronolaw

#ifndef _NGX_AUX_H_INCLUDED_
#define _NGX_AUX_H_INCLUDED_

// ngx_array_t aux

#define ngx_array_nelts(array)                                                \
    ((array)->nelts)

#define ngx_array_reset(array)                                                \
    (array)->nelts = 0;

#define ngx_array_empty(array)                                                \
    ((array)->nelts == 0)

#define ngx_array_capacity(array)                                             \
    ((array)->nalloc)

#define ngx_array_each(elt, array)                                            \
    do {                                                                      \
        void  *_end;                                                          \
        elt = (array)->elts;                                                  \
                                                                              \
        for (_end = (elt + (array)->nelts); elt != _end; elt++) {             \

#define ngx_array_loop                                                        \
        }                                                                     \
    } while (0)

// ngx_list_t aux

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

// ngx_queue_t aux

#endif  // _NGX_AUX_H_INCLUDED_

