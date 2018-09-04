// Copyright (c) 2018 chronolaw

#ifndef _NGX_AUX_H_INCLUDED_
#define _NGX_AUX_H_INCLUDED_

#define ngx_list_each(elt, list)                                              \
    do {                                                                      \
        void             *_end;                                           \
        ngx_list_part_t  *_part = &((list)->part);                    \
                                                                          \
        do {                                                              \
            elt = _part->elts;                                   \
                                                                          \
            for (_end = (elt + _part->nelts); elt != _end; elt++) {       \

#define ngx_list_loop                                                         \
            }                                                             \
                                                                          \
            _part = _part->next;                                          \
                                                                          \
        } while (_part != NULL);                                          \
    } while (0)

#endif  // _NGX_AUX_H_INCLUDED_

