/**
 * @file    test_sorted_list.c
 * @brief   Unity unit tests for the bounded, timestamp-sorted sensor event list.
 */

#include "sorted_list.h"
#include "unity.h"

#include <stdbool.h>
#include <stdint.h>

void setUp(void)
{
    /* No common setup required. */
}

void tearDown(void)
{
    /* No common teardown required. */
}

/**
 * @brief Verify that out-of-order insertions are stored in ascending
 *        timestamp order.
 */
void test_sorted_insert_maintains_order(void)
{
    event_node_t *p_head = NULL;

    event_node_t *p_node = create_event_node(100U, 1U, 10);
    TEST_ASSERT_NOT_NULL(p_node);
    TEST_ASSERT_TRUE(sorted_insert(&p_head, p_node, 5U));
    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(50U, 2U, 20), 5U));
    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(200U, 3U, 30), 5U));
    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(150U, 4U, 40), 5U));

    TEST_ASSERT_EQUAL_UINT32(4U, get_list_length(p_head));

    TEST_ASSERT_NOT_NULL(p_head);
    TEST_ASSERT_EQUAL_UINT32(50U, p_head->timestamp);

    TEST_ASSERT_NOT_NULL(p_head->p_next);
    TEST_ASSERT_EQUAL_UINT32(100U, p_head->p_next->timestamp);

    TEST_ASSERT_NOT_NULL(p_head->p_next->p_next);
    TEST_ASSERT_EQUAL_UINT32(150U, p_head->p_next->p_next->timestamp);

    TEST_ASSERT_NOT_NULL(p_head->p_next->p_next->p_next);
    TEST_ASSERT_EQUAL_UINT32(200U, p_head->p_next->p_next->p_next->timestamp);

    TEST_ASSERT_NULL(p_head->p_next->p_next->p_next->p_next);

    free_list(&p_head);
    TEST_ASSERT_NULL(p_head);
}

/**
 * @brief Verify that insertion beyond max_capacity evicts the oldest
 *        event, which is the head of the sorted list.
 */
void test_sorted_insert_enforces_capacity(void)
{
    event_node_t *p_head = NULL;

    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(100U, 1U, 10), 3U));
    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(50U, 2U, 20), 3U));
    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(200U, 3U, 30), 3U));

    TEST_ASSERT_EQUAL_UINT32(3U, get_list_length(p_head));
    TEST_ASSERT_EQUAL_UINT32(50U, p_head->timestamp);

    /* Fourth insertion exceeds capacity.
     * Sorted order before eviction would be: 50, 100, 150, 200.
     * The oldest event (50) must be removed.
     */
    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(150U, 4U, 40), 3U));

    TEST_ASSERT_EQUAL_UINT32(3U, get_list_length(p_head));
    TEST_ASSERT_NOT_NULL(p_head);
    TEST_ASSERT_EQUAL_UINT32(100U, p_head->timestamp);
    TEST_ASSERT_EQUAL_UINT32(150U, p_head->p_next->timestamp);
    TEST_ASSERT_EQUAL_UINT32(200U, p_head->p_next->p_next->timestamp);
    TEST_ASSERT_NULL(p_head->p_next->p_next->p_next);

    free_list(&p_head);
}

/**
 * @brief Verify deletion when the matching node is the head.
 */
void test_delete_head_node(void)
{
    event_node_t *p_head = NULL;

    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(10U, 1U, 100), 5U));
    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(20U, 2U, 200), 5U));
    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(30U, 3U, 300), 5U));

    TEST_ASSERT_TRUE(delete_event_by_id(&p_head, 1U));

    TEST_ASSERT_EQUAL_UINT32(2U, get_list_length(p_head));
    TEST_ASSERT_NOT_NULL(p_head);
    TEST_ASSERT_EQUAL_UINT8(2U, p_head->sensor_id);
    TEST_ASSERT_EQUAL_UINT32(20U, p_head->timestamp);

    free_list(&p_head);
}

/**
 * @brief Verify deletion when the matching node is in the middle.
 */
void test_delete_middle_node(void)
{
    event_node_t *p_head = NULL;

    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(10U, 1U, 100), 5U));
    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(20U, 2U, 200), 5U));
    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(30U, 3U, 300), 5U));

    TEST_ASSERT_TRUE(delete_event_by_id(&p_head, 2U));

    TEST_ASSERT_EQUAL_UINT32(2U, get_list_length(p_head));
    TEST_ASSERT_NOT_NULL(p_head);
    TEST_ASSERT_EQUAL_UINT8(1U, p_head->sensor_id);
    TEST_ASSERT_NOT_NULL(p_head->p_next);
    TEST_ASSERT_EQUAL_UINT8(3U, p_head->p_next->sensor_id);
    TEST_ASSERT_NULL(p_head->p_next->p_next);

    free_list(&p_head);
}

/**
 * @brief Verify deletion when the matching node is the tail.
 */
void test_delete_tail_node(void)
{
    event_node_t *p_head = NULL;

    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(10U, 1U, 100), 5U));
    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(20U, 2U, 200), 5U));
    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(30U, 3U, 300), 5U));

    TEST_ASSERT_TRUE(delete_event_by_id(&p_head, 3U));

    TEST_ASSERT_EQUAL_UINT32(2U, get_list_length(p_head));
    TEST_ASSERT_NOT_NULL(p_head);
    TEST_ASSERT_EQUAL_UINT8(1U, p_head->sensor_id);
    TEST_ASSERT_NOT_NULL(p_head->p_next);
    TEST_ASSERT_EQUAL_UINT8(2U, p_head->p_next->sensor_id);
    TEST_ASSERT_NULL(p_head->p_next->p_next);

    free_list(&p_head);
}

/**
 * @brief Verify that deleting an ID not present in the list fails safely
 *        and does not modify the list.
 */
void test_delete_non_existent_id(void)
{
    event_node_t *p_head = NULL;

    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(10U, 1U, 100), 5U));
    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(20U, 2U, 200), 5U));

    TEST_ASSERT_FALSE(delete_event_by_id(&p_head, 99U));

    TEST_ASSERT_EQUAL_UINT32(2U, get_list_length(p_head));
    TEST_ASSERT_EQUAL_UINT8(1U, p_head->sensor_id);
    TEST_ASSERT_EQUAL_UINT8(2U, p_head->p_next->sensor_id);

    free_list(&p_head);
}

/**
 * @brief Verify that free_list releases all nodes and resets the caller's
 *        head pointer to NULL.
 */
void test_free_list_nulls_head(void)
{
    event_node_t *p_head = NULL;

    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(10U, 1U, 100), 5U));
    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(20U, 2U, 200), 5U));
    TEST_ASSERT_TRUE(sorted_insert(&p_head, create_event_node(30U, 3U, 300), 5U));

    TEST_ASSERT_EQUAL_UINT32(3U, get_list_length(p_head));

    free_list(&p_head);

    TEST_ASSERT_NULL(p_head);
    TEST_ASSERT_EQUAL_UINT32(0U, get_list_length(p_head));
}

int main(void)
{
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
