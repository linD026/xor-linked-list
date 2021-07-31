/*
 * test function benchmark
 */
#include "xorlist.h"
#include <stdio.h>

struct test_node {
    int value;
    xor_node_t xornode;
};

xorlist_delete_prototype(test_delete, _node)
{
    struct test_node *node = container_of(_node, struct test_node, xornode);
    free(node);
    return 0;
}

int main(void)
{
    xor_list_t *list = xorlist_new();
    xor_node_t *p1;
    xor_node_t *p2;
    for (int i = 0; i < 10; i++) {
        struct test_node *new = malloc(sizeof(struct test_node));
        xornode_init(&new->xornode);
        new->value = i;
        xorlist_add(list, &new->xornode);
        if (i == 5)
            p1 = &new->xornode;
        if (i == 6)
            p2 = &new->xornode;
    }

    xor_node_t *real_prev, *real_next, *node;
    struct test_node *tmp;
    int i = 0;
    printf("xorlist_for_each test\n");
    xorlist_for_each(node, real_prev, real_next, list)
    {
        printf("node [%d] %d\n", i++,
               container_of(node, struct test_node, xornode)->value);
    }

    i = 0;
    printf("xorlist_for_from test\n");
    xorlist_for_each_from(node, p1, p2, real_prev, real_next, list)
    {
        printf("node %d\n",
               container_of(node, struct test_node, xornode)->value);
    }

    i = 0;
    printf("xorlist_for_each_from_prev test\n");
    xorlist_for_each_from_prev(node, p1, p2, real_prev, real_next, list)
    {
        printf("node [%d] %d\n", i++,
               container_of(node, struct test_node, xornode)->value);
    }

    i = 0;
    printf("xorlist_for_each_prev test\n");
    xorlist_for_each_prev(node, real_prev, real_next, list)
    {
        printf("node [%d] %d\n", i++,
               container_of(node, struct test_node, xornode)->value);
    }

    printf("xorlist_del test\n");
    xorlist_del(list, p2, p1, xorlist_delete_call(test_delete));
    i = 0;
    xorlist_for_each(node, real_prev, real_next, list)
    {
        printf("node [%d] %d\n", i++,
               container_of(node, struct test_node, xornode)->value);
    }

    xorlist_destroy(list, xorlist_delete_call(test_delete));

    return 0;
}
