#include "unity.h"
#include "string_memory.h"
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

/**
 * @brief Initializes the test environment before each test case.
 */
void setUp(void);

/**
 * @brief Cleans up the test environment after each test case.
 */
void tearDown(void);

/**
 * @brief Tests copying a valid string to a heap-allocated buffer.
 */
void test_heap_string_copy_valid(void);

/**
 * @brief Tests heap string copy behavior when the input pointer is NULL.
 */
void test_heap_string_copy_null_input(void);

/**
 * @brief Tests the difference between sizeof and strlen for a stack array.
 */
void test_stack_array_sizeof_vs_strlen(void);

/**
 * @brief Tests whether a string literal is identified as read-only data.
 */
void test_string_literal_is_readonly_region(void);

int32_t main() {
    UNITY_BEGIN();

    RUN_TEST(test_heap_string_copy_valid);
    RUN_TEST(test_heap_string_copy_null_input);
    RUN_TEST(test_stack_array_sizeof_vs_strlen);
    RUN_TEST(test_string_literal_is_readonly_region);

    return UNITY_END();
}

void setUp() {

}

void tearDown() {

}

void test_heap_string_copy_valid() {
    char *p_des = heap_string_copy("DuongTrungHien");

    TEST_ASSERT_NOT_NULL(p_des);
    TEST_ASSERT_EQUAL_STRING("DuongTrungHien", p_des);

    free(p_des);
    p_des = NULL;
}

void test_heap_string_copy_null_input() {
    char *p_des = heap_string_copy(NULL);

    TEST_ASSERT_NULL(p_des);

    free(p_des);
    p_des = NULL;
}

void test_stack_array_sizeof_vs_strlen() {
    char stack_arr[] = "Hien";

    TEST_ASSERT_EQUAL_UINT8(5U, sizeof(stack_arr));
    TEST_ASSERT_EQUAL_UINT8(4U, strlen(stack_arr));
}

void test_string_literal_is_readonly_region() {
    const char *p_rodata = "TrungHien";

    TEST_ASSERT_EQUAL_STRING(RODATA_LABEL, get_memory_region(p_rodata));
}