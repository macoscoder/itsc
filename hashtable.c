#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hash_table {
    int cap, len;
    hash_func_t hash;
    equal_func_t equal;
    free_func_t free;
    struct kv_node *table;
};

static void *xmalloc(size_t size)
{
    void *p = malloc(size);
    if (!p) {
        fputs("malloc: out of memory\n", stderr);
        exit(1);
    }
    return p;
}

static void *xrealloc(void *ptr, size_t size)
{
    void *p = realloc(ptr, size);
    if (!p) {
        fputs("realloc: out of memory\n", stderr);
        exit(1);
    }
    return p;
}

static inline void kv_node_init(struct kv_node *head)
{
    head->next = head->prev = head;
}

static inline void __kv_node_add(struct kv_node *node, struct kv_node *prev, struct kv_node *next)
{
    node->next = next;
    node->prev = prev;
    prev->next = node;
    next->prev = node;
}

static inline void kv_node_add(struct kv_node *node, struct kv_node *head)
{
    __kv_node_add(node, head, head->next);
}

static inline void kv_node_del(struct kv_node *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

static inline int kv_node_empty(const struct kv_node *head)
{
    return head->next == head;
}

struct hash_table *hash_table_new(hash_func_t hash, equal_func_t equal, free_func_t free)
{
    struct hash_table *htab;
    int i;

    htab = xmalloc(sizeof(struct hash_table));
    htab->cap = 1;
    htab->len = 0;
    htab->hash = hash;
    htab->equal = equal;
    htab->free = free;
    htab->table = xmalloc(htab->cap * sizeof(struct kv_node));

    for (i = 0; i < htab->cap; ++i)
        kv_node_init(&htab->table[i]);

    return htab;
}

void hash_table_free(struct hash_table *htab)
{
    hash_table_clear(htab);
    free(htab->table);
    free(htab);
}

static void do_rehash(struct hash_table *htab)
{
    int i;
    struct kv_node list;

    kv_node_init(&list);

    for (i = 0; i < htab->cap; ++i) {
        struct kv_node *head = &htab->table[i];
        while (!kv_node_empty(head)) {
            struct kv_node *n = head->next;
            kv_node_del(n);
            kv_node_add(n, &list);
        }
    }

    htab->cap = htab->cap << 1;
    htab->table = xrealloc(htab->table, htab->cap * sizeof(struct kv_node));

    for (i = 0; i < htab->cap; ++i)
        kv_node_init(&htab->table[i]);

    while (!kv_node_empty(&list)) {
        struct kv_node *n = list.next;
        i = htab->hash(n) % htab->cap;
        kv_node_del(n);
        kv_node_add(n, &htab->table[i]);
    }
}

static inline void rehash_if_needed(struct hash_table *htab)
{
    if (htab->len < htab->cap)
        return;
    do_rehash(htab);
}

void hash_table_put(struct hash_table *htab, struct kv_node *node)
{
    int i;

    rehash_if_needed(htab);

    i = htab->hash(node) % htab->cap;
    kv_node_add(node, &htab->table[i]);
    htab->len++;
}

void hash_table_del(struct hash_table *htab, struct kv_node *node)
{
    kv_node_del(node);
    htab->len--;
    if (htab->free)
        htab->free(node);
}

struct kv_node *hash_table_get(struct hash_table *htab, struct kv_node *node)
{
    int i;
    struct kv_node *head, *n;

    i = htab->hash(node) % htab->cap;
    head = &htab->table[i];

    for (n = head->next; n != head; n = n->next)
        if (htab->equal(node, n))
            return n;
    return NULL;
}

void hash_table_for_each(struct hash_table *htab, visit_func_t visit)
{
    int i;

    for (i = 0; i < htab->cap; ++i) {
        struct kv_node *head, *n;
        head = &htab->table[i];
        for (n = head->next; n != head; n = n->next)
            visit(n);
    }
}

void hash_table_clear(struct hash_table *htab)
{
    int i;

    for (i = 0; i < htab->cap; ++i) {
        struct kv_node *head = &htab->table[i];
        while (!kv_node_empty(head)) {
            struct kv_node *n = head->next;
            kv_node_del(n);
            if (htab->free)
                htab->free(n);
        }
    }

    htab->cap = 1;
    htab->len = 0;
    htab->table = xrealloc(htab->table, htab->cap * sizeof(struct kv_node));

    for (i = 0; i < htab->cap; ++i)
        kv_node_init(&htab->table[i]);
}

int hash_table_size(struct hash_table *htab)
{
    return htab->len;
}

unsigned int hash_string(const char *s)
{
    int i, len = strlen(s);
    unsigned int h = 0;

    for (i = 0; i < len; ++i)
        h = 31 * h + (unsigned char)s[i];
    return h;
}
