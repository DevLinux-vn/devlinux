#include "unity.h"
#include "intrusive_list.h"

void setUp(void) {}
void tearDown(void) {}

void test_container_of_macro_offset(void) {
    deferred_task_t task;
    TEST_ASSERT_EQUAL_PTR(&task, CONTAINER_OF(&task.node, deferred_task_t, node));
}

void test_slist_push_and_pop_lifo(void) {
    slist_node_t *p_head = NULL;
    slist_node_t node1 = {NULL}, node2 = {NULL};
    
    slist_push(&p_head, &node1);
    slist_push(&p_head, &node2);
    
    TEST_ASSERT_EQUAL_PTR(&node2, slist_pop(&p_head));
    TEST_ASSERT_EQUAL_PTR(&node1, slist_pop(&p_head));
    TEST_ASSERT_NULL(slist_pop(&p_head));
}

void test_slist_append_fifo(void) {
    slist_node_t *p_head = NULL;
    slist_node_t node1 = {NULL}, node2 = {NULL};
    
    slist_append(&p_head, &node1);
    slist_append(&p_head, &node2);
    
    TEST_ASSERT_EQUAL_PTR(&node1, slist_pop(&p_head));
    TEST_ASSERT_EQUAL_PTR(&node2, slist_pop(&p_head));
}

void test_slist_remove_head(void) {
    slist_node_t *p_head = NULL;
    slist_node_t node1 = {NULL}, node2 = {NULL};
    
    slist_push(&p_head, &node2);
    slist_push(&p_head, &node1);
    
    TEST_ASSERT_TRUE(slist_remove(&p_head, &node1));
    TEST_ASSERT_EQUAL_PTR(&node2, p_head);
}

void test_slist_remove_middle(void) {
    slist_node_t *p_head = NULL;
    slist_node_t n1={NULL}, n2={NULL}, n3={NULL};
    
    slist_push(&p_head, &n3);
    slist_push(&p_head, &n2);
    slist_push(&p_head, &n1);
    
    TEST_ASSERT_TRUE(slist_remove(&p_head, &n2));
    TEST_ASSERT_EQUAL_PTR(&n3, n1.p_next);
}

void test_slist_pop_empty_returns_null(void) {
    slist_node_t *p_head = NULL;
    TEST_ASSERT_NULL(slist_pop(&p_head));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_container_of_macro_offset);
    RUN_TEST(test_slist_push_and_pop_lifo);
    RUN_TEST(test_slist_append_fifo);
    RUN_TEST(test_slist_remove_head);
    RUN_TEST(test_slist_remove_middle);
    RUN_TEST(test_slist_pop_empty_returns_null);
    return UNITY_END();
}