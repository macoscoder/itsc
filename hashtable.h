#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define kv_node_entry(addr, type, member) ((type *)((char *)addr - offsetof(type, member)))

struct kv_node {
    struct kv_node *prev;
    struct kv_node *next;
};

typedef unsigned int (*hash_func_t)(struct kv_node *);
typedef int (*equal_func_t)(struct kv_node *, struct kv_node *);
typedef void (*free_func_t)(struct kv_node *);
typedef void (*visit_func_t)(struct kv_node *);

struct hash_table *hash_table_new(hash_func_t hash, equal_func_t equal, free_func_t free);
void hash_table_free(struct hash_table *htab);
void hash_table_put(struct hash_table *htab, struct kv_node *node);
void hash_table_del(struct hash_table *htab, struct kv_node *node);
struct kv_node *hash_table_get(struct hash_table *htab, struct kv_node *node);
void hash_table_for_each(struct hash_table *htab, visit_func_t visit);
void hash_table_clear(struct hash_table *htab);
int hash_table_size(struct hash_table *htab);

unsigned int hash_string(const char *s);

#ifdef __cplusplus
}
#endif
