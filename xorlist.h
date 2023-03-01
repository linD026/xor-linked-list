/*
 * xor linked list
 */
#ifndef __XORLIST_H__
#define __XORLIST_H__

#include <assert.h>

#if !defined(__GNUC__)
static_assert(0, "gnuc undefine!");
#endif

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct _xorlist_node {
    struct _xorlist_node *cmp;
} xor_node_t;

typedef struct _xor_list_struct {
    xor_node_t head;
    xor_node_t tail;
    uint32_t cnt;
} xor_list_t;

#define _VA(a) ((unsigned long)(a))
#define compress_addr(a, b) ((xor_node_t *)(_VA(a) ^ _VA(b)))

/*
 * Get the compressed address stored at node->cmp with neighbor.
 * The value you want to decompress must passed by node->cmp.
 * For example:
 *  A -> B -> C -> D
 * Want to get the address of D by B node and C node wil be like
 *  D = address_of(B, C->cmp) or D = address_of(C->cmp, B)
 */
static inline xor_node_t *address_of(xor_node_t *n1, xor_node_t *n2)
{
    assert(n1 && n2);
    return compress_addr(n1, n2);
}

#define container_of(ptr, type, member)                           \
    __extension__({                                               \
        const __typeof__(((type *)0)->member) *__pmember = (ptr); \
        (type *)((char *)__pmember - offsetof(type, member));     \
    })

#define xorlist_for_each(node, rp, rn, list)                        \
    for (rp = &(list)->head, node = rp->cmp; node != &(list)->tail; \
         rn = address_of(rp, node->cmp), rp = node, node = rn)

#define xorlist_for_each_prev(node, rp, rn, list)                   \
    for (rp = &(list)->tail, node = rp->cmp; node != &(list)->head; \
         rn = address_of(rp, node->cmp), rp = node, node = rn)

#define xorlist_for_each_from(node, pos1, pos2, rp, rn, list) \
    for (rp = pos2, node = pos1; node != &(list)->tail;       \
         rn = address_of(rp, node->cmp), rp = node, node = rn)

#define xorlist_for_each_from_prev(node, pos1, pos2, rp, rn, list) \
    for (rp = pos1, node = pos2; node != &(list)->head;            \
         rn = address_of(rp, node->cmp), rp = node, node = rn)

/*
 *  xorlist delete function prototype
 *  Note that when the delete function success is must return 0.
 */
#define xorlist_delete_prototype(name, node) \
    int _xorlist_delete_##name(xor_node_t *node)

#define xorlist_delete_call(name) _xorlist_delete_##name

static inline xor_node_t *xornode_init(xor_node_t *n)
{
    assert(n);
    n->cmp = NULL;

    return n;
}

#define XORNODE_INIT(n) \
    do {                \
        (n).cmp = NULL; \
    } while (0)

#define XORLIST_INIT(h)           \
    do {                          \
        (h).head.cmp = &(h).tail; \
        (h).tail.cmp = &(h).head; \
        (h).cnt = 0;              \
    } while (0)

int xorlist_add(xor_list_t *list, xor_node_t *n)
{
    xor_node_t *real_prev, *real_next;

    if (!n) {
        perror("NULL pointer");
        return -EAGAIN;
    }

    real_prev = &list->head;
    real_next = real_prev->cmp;

    real_prev->cmp = n;
    n->cmp = compress_addr(real_prev, real_next);
    real_next->cmp = compress_addr(n, address_of(real_prev, real_next->cmp));
    list->cnt++;

    return 0;
}

/*
 * nn -> n -> target -> an -> ana
 * ana <- an <- target <- n <- nn
 */
void xorlist_remove(xor_list_t *list, xor_node_t *n, xor_node_t *target)
{
    xor_node_t *nn, *an, *ana;

    assert(list && n && target);
    assert(&list->head != target && &list->tail != target);
    nn = address_of(target, n->cmp);
    an = address_of(n, target->cmp);
    ana = address_of(target, an->cmp);
    n->cmp = compress_addr(nn, an);
    an->cmp = compress_addr(n, ana);
    list->cnt--;
}

int xorlist_del(xor_list_t *list, xor_node_t *n, xor_node_t *target,
                int (*delete_func)(xor_node_t *))
{
    assert(delete_func);
    xorlist_remove(list, n, target);
    return delete_func(target);
}

int xorlist_destroy(xor_list_t *list, int (*delete_func)(xor_node_t *))
{
    xor_node_t *real_prev, *node, *real_next;
    xor_node_t *tmp;

    assert(delete_func);

    real_prev = &list->head;
    node = real_prev->cmp;

    while (node != &list->tail) {
        real_next = address_of(real_prev, node->cmp);
        tmp = node;
        real_prev = node;
        node = real_next;

        if (delete_func(tmp) != 0)
            perror("delete function failed");
    }

    return 0;
}

#endif /* __XORLIST_H__ */
