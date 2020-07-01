#include "hashtable.h"
#include "vector.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifndef __printflike
#define __printflike(fmtarg, firstvararg) __attribute__((__format__(__printf__, fmtarg, firstvararg)))
#endif

static void __printflike(1, 2) logf(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
}

static void test_vector()
{
    vector(int) vi;

    memset(&vi, 0, sizeof(vi));

    vector_push_back(&vi, 1);
    vector_push_back(&vi, 2);
    vector_push_back(&vi, 3);
    vector_push_back(&vi, 4);
    vector_push_back(&vi, 5);

    logf("=========== test vector ============");

    for (int i = 0; i < vi.len; i++)
        logf("vi[%d]=%d", i, vi.arr[i]);

    logf("vi.len = %d", vi.len);
    logf("vi.cap = %d", vi.cap);
}

struct mynode {
    char key[32];
    int value;
    struct kv_node node;
};

#define mynode_entry(n) kv_node_entry(n, struct mynode, node)

static unsigned int hash(struct kv_node *n)
{
    struct mynode *s = mynode_entry(n);
    return hash_string(s->key);
}

static int equal(struct kv_node *lhs, struct kv_node *rhs)
{
    struct mynode *l = mynode_entry(lhs);
    struct mynode *r = mynode_entry(rhs);
    return strcmp(l->key, r->key) == 0;
}

static void mynode_free(struct kv_node *n)
{
    struct mynode *s = mynode_entry(n);
    free(s);
}

static void visit(struct kv_node *node)
{
    struct mynode *n = mynode_entry(node);
    printf("key: %s, value: %d\n", n->key, n->value);
}

static void test_hashtable()
{
    struct hash_table *ht = hash_table_new(hash, equal, mynode_free);

    for (int i = 0; i < 5; i++) {
        struct mynode *n = malloc(sizeof(struct mynode));
        sprintf(n->key, "key_%d", i);
        n->value = i;
        hash_table_put(ht, &n->node);
    }

    logf("=========== test hash table ===========");
    logf("size: %d", hash_table_size(ht));

    hash_table_for_each(ht, visit);

    struct mynode a, *p;
    strcpy(a.key, "key_2");
    p = mynode_entry(hash_table_get(ht, &a.node));
    logf("==> key: %s, value: %d", p->key, p->value);

    hash_table_del(ht, &p->node);

    hash_table_for_each(ht, visit);

    hash_table_free(ht);
}

int main()
{
    test_vector();
    test_hashtable();
}
