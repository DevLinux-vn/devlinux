#include "intrusive_list.h"
#include "unity.h"
#include <stdint.h>

/**
 * @brief Set up the test environment before each test case.
 */
void setUp();

/**
 * @brief Clean up the test environment after each test case.
 */
void tearDown();

/**
 * @brief Test the CONTAINER_OF macro using the correct member offset.
 */
void test_container_of_macro_offset();

/**
 * @brief Test singly-linked list push and pop operations using LIFO order.
 */
void test_slist_push_and_pop_lifo();

/**
 * @brief Test singly-linked list append operation using FIFO order.
 */
void test_slist_append_fifo();

/**
 * @brief Test removing the head node from the singly-linked list.
 */
void test_slist_remove_head();

/**
 * @brief Test removing a middle node from the singly-linked list.
 */
void test_slist_remove_middle();

/**
 * @brief Test that popping from an empty list returns NULL.
 */
void test_slist_pop_empty_returns_null();

int32_t main() {
    UNITY_BEGIN();

    RUN_TEST(test_container_of_macro_offset);
    RUN_TEST(test_slist_push_and_pop_lifo);
    RUN_TEST(test_slist_append_fifo);
    RUN_TEST(test_slist_remove_head);
    RUN_TEST(test_slist_remove_middle);
    RUN_TEST(test_slist_pop_empty_returns_null);

    return UNITY_END();
}

void setUp() {

}

void tearDown() {

}

void test_container_of_macro_offset() {
    deferred_task_t task = {.task_id = 1U};
    slist_node_t *p_node = &task.node;

    const deferred_task_t *p_result = CONTAINER_OF(p_node, deferred_task_t, node);

    if (NULL == p_result) {
        return;
    }

    TEST_ASSERT_EQUAL_UINT(task.task_id, p_result->task_id);

}

void test_slist_push_and_pop_lifo() {
    deferred_task_t tasks[3] = {
        {
            .task_id = 1U
        },
        {
            .task_id = 2U
        },
        {
            .task_id = 3U
        }
    };

    slist_node_t *p_head = NULL;

    for (size_t i = 0U; i < 3U; i++) {
        slist_push(&p_head, &tasks[i].node);
    }

    /* Test pop turn 1*/
    slist_node_t *p_test = slist_pop(&p_head);
    TEST_ASSERT_NOT_NULL(p_test);

    const deferred_task_t *p_result = CONTAINER_OF(p_test, deferred_task_t, node);
    TEST_ASSERT_EQUAL_UINT(tasks[2].task_id, p_result->task_id);

    /* Test pop turn 2*/
    p_test = slist_pop(&p_head);
    TEST_ASSERT_NOT_NULL(p_test);

    p_result = CONTAINER_OF(p_test, deferred_task_t, node);
    TEST_ASSERT_EQUAL_UINT(tasks[1].task_id, p_result->task_id);

    /* Test pop turn 3*/
    p_test = slist_pop(&p_head);
    TEST_ASSERT_NOT_NULL(p_test);

    p_result = CONTAINER_OF(p_test, deferred_task_t, node);
    TEST_ASSERT_EQUAL_UINT(tasks[0].task_id, p_result->task_id);

    /* Test pop turn 4*/
    p_test = slist_pop(&p_head);
    TEST_ASSERT_NULL(p_test);
}

void test_slist_append_fifo() {
    deferred_task_t tasks[3] = {
        {
            .task_id = 1U
        },
        {
            .task_id = 2U
        },
        {
            .task_id = 3U
        }
    };

    slist_node_t *p_head = NULL;

    for (size_t i = 0U; i < 3U; i++) {
        slist_append(&p_head, &tasks[i].node);
    }

    /* Test pop turn 1*/
    slist_node_t *p_test = slist_pop(&p_head);
    TEST_ASSERT_NOT_NULL(p_test);

    const deferred_task_t *p_result = CONTAINER_OF(p_test, deferred_task_t, node);
    TEST_ASSERT_EQUAL_UINT(tasks[0].task_id, p_result->task_id);

    /* Test pop turn 2*/
    p_test = slist_pop(&p_head);
    TEST_ASSERT_NOT_NULL(p_test);

    p_result = CONTAINER_OF(p_test, deferred_task_t, node);
    TEST_ASSERT_EQUAL_UINT(tasks[1].task_id, p_result->task_id);

    /* Test pop turn 3*/
    p_test = slist_pop(&p_head);
    TEST_ASSERT_NOT_NULL(p_test);

    p_result = CONTAINER_OF(p_test, deferred_task_t, node);
    TEST_ASSERT_EQUAL_UINT(tasks[2].task_id, p_result->task_id);

    /* Test pop turn 4*/
    p_test = slist_pop(&p_head);
    TEST_ASSERT_NULL(p_test);
}

void test_slist_remove_head() {
    deferred_task_t tasks[3] = {
        {
            .task_id = 1U
        },
        {
            .task_id = 2U
        },
        {
            .task_id = 3U
        }
    };

    slist_node_t *p_head = NULL;

    for (size_t i = 0U; i < 3U; i++) {
        slist_push(&p_head, &tasks[i].node);
    }  

    bool result = slist_remove(&p_head, &tasks[2].node);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_PTR(&tasks[1].node, p_head);

}

void test_slist_remove_middle() {
    deferred_task_t tasks[3] = {
        {
            .task_id = 1U
        },
        {
            .task_id = 2U
        },
        {
            .task_id = 3U
        }
    };

    slist_node_t *p_head = NULL;

    for (size_t i = 0U; i < 3U; i++) {
        slist_push(&p_head, &tasks[i].node);
    }  

    bool result = slist_remove(&p_head, &tasks[1].node);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_PTR(&tasks[2].node, p_head);
    TEST_ASSERT_EQUAL_PTR(tasks[2].node.p_next, &tasks[0].node);
}

void test_slist_pop_empty_returns_null() {
    deferred_task_t tasks = {.task_id = 1U};

    slist_node_t *p_head = NULL;
    slist_push(&p_head, &tasks.node);

    /*Test pop turn 1*/
    const slist_node_t *p_node = slist_pop(&p_head);

    TEST_ASSERT_NOT_NULL(p_node);

    /*Test pop turn 2: List is empty*/
    p_node = slist_pop(&p_head);

    TEST_ASSERT_NULL(p_node);
}