#pragma once

#include <stdlib.h>

#define vector(type) \
    struct {         \
        int len;     \
        int cap;     \
        type *arr;   \
    }

#define vector_push_back(vecp, elem)                                                  \
    do {                                                                              \
        if ((vecp)->len == (vecp)->cap) {                                             \
            (vecp)->cap = ((vecp)->cap == 0) ? 1 : (vecp)->cap << 1;                  \
            (vecp)->arr = realloc((vecp)->arr, (vecp)->cap * sizeof((vecp)->arr[0])); \
        }                                                                             \
        (vecp)->arr[(vecp)->len++] = (elem);                                          \
    } while (0)

#define vector_pop_back(vecp) \
    do {                      \
        (vecp)->len--;        \
    } while (0)

#define vector_front(vecp) ((vecp)->arr[0])
#define vector_back(vecp) ((vecp)->arr[(vecp)->len - 1])
#define vector_empty(vecp) ((vecp)->len == 0)
#define vector_size(vecp) ((vecp)->len)
#define vector_capacity(vecp) ((vecp)->cap)
