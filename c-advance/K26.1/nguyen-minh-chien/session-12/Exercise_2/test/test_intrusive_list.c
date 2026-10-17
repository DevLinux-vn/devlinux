/**
 * @file    test_intrusive_list.c
 * @brief   Unity unit tests for the intrusive list.
 */

#include "intrusive_list.h"
#include "unity.h"

#include <stdint.h>

/**
 * @brief Unity setup.
 */
void setUp(void)
{
}

/**
 * @brief Unity teardown.
 */
void tearDown(void)
{
}

/**
 * @brief Verify CONTAINER_OF recovers the parent address.
 */
void test_container_of_macro_offset(void)
{
    deferred_task_t task = {
        .task_id = 123U,
        .priority = 2U,
        .callback = NULL,
        .arg = 0x55AAU,
        .node = { .p_next = NULL }
    };

    slist_node_t *p_node = &task.node;
    deferred_task_t *p_recovered =
        CONTAINER_OF(p_node, deferred_task_t, node);

    TEST_ASSERT_EQUAL_PTR(&task, p_recovered);
    TEST_ASSERT_EQUAL_UINT32(123U, p_recovered->task_id);
    TEST_ASSERT_EQUAL_UINT8(2U, p_recovered->priority);
}

/**
 * @brief Verify LIFO push/pop behavior.
 */
void test_slist_push_and_pop_lifo(void)
{
    slist_node_t *p_head = NULL;
    slist_node_t node1 = { .p_next = NULL };
    slist_node_t node2 = { .p_next = NULL };
    slist_node_t node3 = { .p_next = NULL };

    slist_push(&p_head, &node1);
    slist_push(&p_head, &node2);
    slist_push(&p_head, &node3);

    TEST_ASSERT_EQUAL_PTR(&node3, slist_pop(&p_head));
    TEST_ASSERT_EQUAL_PTR(&node2, slist_pop(&p_head));
    TEST_ASSERT_EQUAL_PTR(&node1, slist_pop(&p_head));
    TEST_ASSERT_NULL(p_head);
}

/**
 * @brief Verify FIFO behavior when nodes are appended.
 */
void test_slist_append_fifo(void)
{
    slist_node_t *p_head = NULL;
    slist_node_t node1 = { .p_next = NULL };
    slist_node_t node2 = { .p_next = NULL };
    slist_node_t node3 = { .p_next = NULL };

    slist_append(&p_head, &node1);
    slist_append(&p_head, &node2);
    slist_append(&p_head, &node3);

    TEST_ASSERT_EQUAL_PTR(&node1, slist_pop(&p_head));
    TEST_ASSERT_EQUAL_PTR(&node2, slist_pop(&p_head));
    TEST_ASSERT_EQUAL_PTR(&node3, slist_pop(&p_head));
    TEST_ASSERT_NULL(p_head);
}

/**
 * @brief Verify removing the head updates p_head.
 */
void test_slist_remove_head(void)
{
    slist_node_t *p_head = NULL;
    slist_node_t node1 = { .p_next = NULL };
    slist_node_t node2 = { .p_next = NULL };
    slist_node_t node3 = { .p_next = NULL };

    slist_append(&p_head, &node1);
    slist_append(&p_head, &node2);
    slist_append(&p_head, &node3);

    TEST_ASSERT_TRUE(slist_remove(&p_head, &node1));
    TEST_ASSERT_EQUAL_PTR(&node2, p_head);
    TEST_ASSERT_NULL(node1.p_next);
}

/**
 * @brief Verify removing a middle node reconnects the chain.
 */
void test_slist_remove_middle(void)
{
    slist_node_t *p_head = NULL;
    slist_node_t node1 = { .p_next = NULL };
    slist_node_t node2 = { .p_next = NULL };
    slist_node_t node3 = { .p_next = NULL };

    slist_append(&p_head, &node1);
    slist_append(&p_head, &node2);
    slist_append(&p_head, &node3);

    TEST_ASSERT_TRUE(slist_remove(&p_head, &node2));
    TEST_ASSERT_EQUAL_PTR(&node1, p_head);
    TEST_ASSERT_EQUAL_PTR(&node3, node1.p_next);
    TEST_ASSERT_NULL(node2.p_next);
}

/**
 * @brief Verify pop on an empty list safely returns NULL.
 */
void test_slist_pop_empty_returns_null(void)
{
    slist_node_t *p_head = NULL;

    TEST_ASSERT_NULL(slist_pop(&p_head));
    TEST_ASSERT_NULL(p_head);
}

/**
 * @brief Unity test runner.
 *
 * @return Unity status.
 */
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_container_of_macro_offset);
    RUN_TEST(test_slist_push_and_pop_lifo);
    RUN_TEST(test_slist_append_fifo);
    RUN_TEST(test_slist_remove_head);
    RUN_TEST(test_slist_remove_middle);
    RUN_TEST(test_slist_pop_empty_returns_null);

    return UNITY_END();
}
