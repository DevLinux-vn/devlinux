/**
 * @file    test_string_memory.c
 * @brief   Unity test suite for string_memory.c.
 */
#include "unity.h"
#include "string_memory.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief   Unity setup hook, run before each test.
 */
void setUp(void)
{
    /* Nothing to set up. */
}

/**
 * @brief   Unity teardown hook, run after each test.
 */
void tearDown(void)
{
    /* Nothing to tear down. */
}

/**
 * @brief   heap_string_copy() must copy the content correctly.
 */
void test_heap_string_copy_valid(void)
{
    char *p_copy = heap_string_copy("firmware");

    TEST_ASSERT_NOT_NULL(p_copy);
    TEST_ASSERT_EQUAL_STRING("firmware", p_copy);

    free(p_copy);
}

/**
 * @brief   heap_string_copy(NULL) must safely return NULL.
 */
void test_heap_string_copy_null_input(void)
{
    char *p_copy = heap_string_copy(NULL);

    TEST_ASSERT_NULL(p_copy);
}

/**
 * @brief   sizeof() must include the null terminator; strlen() must not.
 */
void test_stack_array_sizeof_vs_strlen(void)
{
    char stack_str[] = "firmware";

    TEST_ASSERT_EQUAL_UINT32(9U, (uint32_t)sizeof(stack_str));
    TEST_ASSERT_EQUAL_UINT32(8U, (uint32_t)strlen(stack_str));
}

/**
 * @brief   get_memory_region() must classify a string literal's own
 *          address as "Read-Only Data".
 *
 * @details The literal is used as its own reference address, so its
 *          distance to the "Read-Only Data" reference is exactly 0,
 *          making this test deterministic regardless of the actual
 *          runtime memory layout.
 */
void test_string_literal_is_readonly_region(void)
{
    static const char *p_literal = "test_literal";
    char stack_var = 0;
    char *p_heap = (char *)malloc(1U);

    TEST_ASSERT_NOT_NULL(p_heap);

    memory_region_set_references((const void *)p_literal,
                                  (const void *)&stack_var,
                                  (const void *)p_heap);

    TEST_ASSERT_EQUAL_STRING("Read-Only Data", get_memory_region((const void *)p_literal));

    free(p_heap);
}

/**
 * @brief   Test runner entry point.
 *
 * @return  Number of failed tests (0 on success).
 */
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_heap_string_copy_valid);
    RUN_TEST(test_heap_string_copy_null_input);
    RUN_TEST(test_stack_array_sizeof_vs_strlen);
    RUN_TEST(test_string_literal_is_readonly_region);

    return UNITY_END();
}