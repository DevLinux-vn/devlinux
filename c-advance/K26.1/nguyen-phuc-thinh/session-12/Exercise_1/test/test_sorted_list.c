#include "unity.h"
#include "sorted_list.h"

void setUp(void) {}
void tearDown(void) {}

void test_sorted_insert_maintains_order(void) {
    event_node_t *p_head = NULL;
    sorted_insert(&p_head, create_event_node(200, 1, 0), 5);
    sorted_insert(&p_head, create_event_node(100, 2, 0), 5);
    
    TEST_ASSERT_NOT_NULL(p_head);
    TEST_ASSERT_EQUAL_UINT32(100, p_head->timestamp);
    TEST_ASSERT_EQUAL_UINT32(200, p_head->p_next->timestamp);
    free_list(&p_head);
}

void test_sorted_insert_enforces_capacity(void) {
    event_node_t *p_head = NULL;
    sorted_insert(&p_head, create_event_node(100, 1, 0), 2);
    sorted_insert(&p_head, create_event_node(200, 2, 0), 2);
    sorted_insert(&p_head, create_event_node(300, 3, 0), 2);
    
    TEST_ASSERT_EQUAL_UINT32(2, get_list_length(p_head));
    TEST_ASSERT_EQUAL_UINT32(200, p_head->timestamp);
    free_list(&p_head);
}

void test_delete_head_node(void) {
    event_node_t *p_head = NULL;
    sorted_insert(&p_head, create_event_node(100, 1, 0), 5);
    sorted_insert(&p_head, create_event_node(200, 2, 0), 5);
    
    TEST_ASSERT_TRUE(delete_event_by_id(&p_head, 1));
    TEST_ASSERT_EQUAL_UINT32(200, p_head->timestamp);
    free_list(&p_head);
}

void test_delete_middle_node(void) {
    event_node_t *p_head = NULL;
    sorted_insert(&p_head, create_event_node(100, 1, 0), 5);
    sorted_insert(&p_head, create_event_node(200, 2, 0), 5);
    sorted_insert(&p_head, create_event_node(300, 3, 0), 5);
    
    TEST_ASSERT_TRUE(delete_event_by_id(&p_head, 2));
    TEST_ASSERT_EQUAL_UINT32(300, p_head->p_next->timestamp);
    free_list(&p_head);
}

void test_delete_tail_node(void) {
    event_node_t *p_head = NULL;
    sorted_insert(&p_head, create_event_node(100, 1, 0), 5);
    sorted_insert(&p_head, create_event_node(200, 2, 0), 5);
    
    TEST_ASSERT_TRUE(delete_event_by_id(&p_head, 2));
    TEST_ASSERT_NULL(p_head->p_next);
    free_list(&p_head);
}

void test_delete_non_existent_id(void) {
    event_node_t *p_head = NULL;
    sorted_insert(&p_head, create_event_node(100, 1, 0), 5);
    
    TEST_ASSERT_FALSE(delete_event_by_id(&p_head, 99));
    free_list(&p_head);
}

void test_free_list_nulls_head(void) {
    event_node_t *p_head = NULL;
    sorted_insert(&p_head, create_event_node(100, 1, 0), 5);
    
    free_list(&p_head);
    TEST_ASSERT_NULL(p_head);
}

int main(void) {
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