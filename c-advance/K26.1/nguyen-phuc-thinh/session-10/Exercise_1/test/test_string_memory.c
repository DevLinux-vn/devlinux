/**
 * @file test_string_memory.c
 * @brief Unity unit tests for string memory functions.
 */
#include "unity.h"
#include "string_memory.h"
#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_heap_string_copy_valid(void) 
{
    const char *p_src = "test";
    char *p_dest = heap_string_copy(p_src);
    TEST_ASSERT_NOT_NULL(p_dest);
    TEST_ASSERT_EQUAL_STRING(p_src, p_dest);
    free(p_dest);
}

void test_heap_string_copy_null_input(void) 
{
    char *p_dest = heap_string_copy(NULL);
    TEST_ASSERT_NULL(p_dest);
}

void test_stack_array_sizeof_vs_strlen(void) 
{
    char p_stack_arr[] = "abc";
    TEST_ASSERT_EQUAL_UINT32(4, sizeof(p_stack_arr));
    TEST_ASSERT_EQUAL_UINT32(3, strlen(p_stack_arr));
}

void test_string_literal_is_readonly_region(void) 
{
    const char *p_literal = "readonly";
    const char *p_region = get_memory_region(p_literal);
    TEST_ASSERT_EQUAL_STRING("Read-Only Data", p_region);
}

int main(void) 
{
    UNITY_BEGIN();
    RUN_TEST(test_heap_string_copy_valid);
    RUN_TEST(test_heap_string_copy_null_input);
    RUN_TEST(test_stack_array_sizeof_vs_strlen);
    RUN_TEST(test_string_literal_is_readonly_region);
    return UNITY_END();
}