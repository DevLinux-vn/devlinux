#include "sorted_list.h"
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
 * @brief Test that sorted insertion maintains the correct timestamp order.
 */
void test_sorted_insert_maintains_order();

/**
 * @brief Test that sorted insertion does not exceed the maximum list capacity.
 */
void test_sorted_insert_enforces_capacity();

/**
 * @brief Test deleting the head node from the event list.
 */
void test_delete_head_node();

/**
 * @brief Test deleting a middle node from the event list.
 */
void test_delete_middle_node();

/**
 * @brief Test deleting the tail node from the event list.
 */
void test_delete_tail_node();

/**
 * @brief Test deleting an event with a sensor ID that does not exist.
 */
void test_delete_non_existent_id();

/**
 * @brief Test that freeing the list sets the head pointer to NULL.
 */
void test_free_list_nulls_head();

int32_t main() {
    UNITY_BEGIN();

    RUN_TEST(test_sorted_insert_maintains_order);
    RUN_TEST(test_sorted_insert_enforces_capacity);
    RUN_TEST(test_delete_head_node);
    RUN_TEST(test_delete_middle_node);
    RUN_TEST(test_delete_tail_node);
    RUN_TEST(test_delete_non_existent_id);
    RUN_TEST(test_free_list_nulls_head);

    return UNITY_END();
}

void setUp() {

}

void tearDown() {

}

void test_sorted_insert_maintains_order() {
    event_node_t *p_head = NULL;

    sorted_insert(&p_head, create_event_node(50U, 1U, 300), 3U);
    sorted_insert(&p_head, create_event_node(20U, 2U, 200), 3U);
    sorted_insert(&p_head, create_event_node(100U, 3U, 100), 3U);

    TEST_ASSERT_EQUAL_UINT(20U, p_head->timestamp);
    TEST_ASSERT_EQUAL_UINT(50U, p_head->p_next->timestamp);
    TEST_ASSERT_EQUAL_UINT(100U, p_head->p_next->p_next->timestamp);

    free_list(&p_head);
}

void test_sorted_insert_enforces_capacity() {
    event_node_t *p_head = NULL;

    sorted_insert(&p_head, create_event_node(50U, 1U, 300), 3U);
    sorted_insert(&p_head, create_event_node(20U, 2U, 200), 3U);
    sorted_insert(&p_head, create_event_node(100U, 3U, 100), 3U);
    sorted_insert(&p_head, create_event_node(80U, 4U, 100), 3U);

    TEST_ASSERT_EQUAL_UINT(3U, get_list_length(p_head));

    TEST_ASSERT_EQUAL_UINT(50U, p_head->timestamp);

    free_list(&p_head);
}

void test_delete_head_node() {
    event_node_t *p_head = NULL;

    sorted_insert(&p_head, create_event_node(50U, 1U, 300), 3U);
    sorted_insert(&p_head, create_event_node(20U, 2U, 200), 3U);
    sorted_insert(&p_head, create_event_node(100U, 3U, 100), 3U);

    bool result = delete_event_by_id(&p_head, 2U);

    TEST_ASSERT_TRUE(result);

    TEST_ASSERT_EQUAL_UINT(2U, get_list_length(p_head));

    TEST_ASSERT_EQUAL_UINT(50U, p_head->timestamp);
    TEST_ASSERT_EQUAL_UINT(100U, p_head->p_next->timestamp);

    free_list(&p_head);
}

void test_delete_middle_node() {
    event_node_t *p_head = NULL;

    sorted_insert(&p_head, create_event_node(50U, 1U, 300), 3U);
    sorted_insert(&p_head, create_event_node(20U, 2U, 200), 3U);
    sorted_insert(&p_head, create_event_node(100U, 3U, 100), 3U);

    bool result = delete_event_by_id(&p_head, 1U);

    TEST_ASSERT_TRUE(result);

    TEST_ASSERT_EQUAL_UINT(2U, get_list_length(p_head));

    TEST_ASSERT_EQUAL_UINT(20U, p_head->timestamp);
    TEST_ASSERT_EQUAL_UINT(100U, p_head->p_next->timestamp);

    free_list(&p_head);
}

void test_delete_tail_node() {
    event_node_t *p_head = NULL;

    sorted_insert(&p_head, create_event_node(50U, 1U, 300), 3U);
    sorted_insert(&p_head, create_event_node(20U, 2U, 200), 3U);
    sorted_insert(&p_head, create_event_node(100U, 3U, 100), 3U);

    bool result = delete_event_by_id(&p_head, 3U);

    TEST_ASSERT_TRUE(result);

    TEST_ASSERT_EQUAL_UINT(2U, get_list_length(p_head));

    TEST_ASSERT_EQUAL_UINT(20U, p_head->timestamp);
    TEST_ASSERT_EQUAL_UINT(50U, p_head->p_next->timestamp);

    free_list(&p_head);
}

void test_delete_non_existent_id() {
    event_node_t *p_head = NULL;

    sorted_insert(&p_head, create_event_node(50U, 1U, 300), 3U);
    sorted_insert(&p_head, create_event_node(20U, 2U, 200), 3U);
    sorted_insert(&p_head, create_event_node(100U, 3U, 100), 3U);

    bool result = delete_event_by_id(&p_head, 99U);

    TEST_ASSERT_FALSE(result);

    TEST_ASSERT_EQUAL_UINT(3U, get_list_length(p_head));

    TEST_ASSERT_EQUAL_UINT(20U, p_head->timestamp);
    TEST_ASSERT_EQUAL_UINT(50U, p_head->p_next->timestamp);
    TEST_ASSERT_EQUAL_UINT(100U, p_head->p_next->p_next->timestamp);

    free_list(&p_head);
}

void test_free_list_nulls_head() {
    event_node_t *p_head = NULL;

    sorted_insert(&p_head, create_event_node(50U, 1U, 300), 3U);
    sorted_insert(&p_head, create_event_node(20U, 2U, 200), 3U);
    sorted_insert(&p_head, create_event_node(100U, 3U, 100), 3U);

    free_list(&p_head);

    TEST_ASSERT_NULL(p_head);
}
