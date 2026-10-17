/**
 * @file test_wide_string_utils.c
 * @brief Unity unit tests for wide string operations.
 */
#include "unity.h"
#include "wide_string_utils.h"

void setUp(void) {}
void tearDown(void) {}

void test_wcslen_counts_characters(void)
{
    wchar_t str[] = L"ABC";
    TEST_ASSERT_EQUAL_size_t(3, wcslen(str));
}

void test_wcscmp_equal(void)
{
    TEST_ASSERT_EQUAL_INT(0, wcscmp(L"Test", L"Test"));
}

void test_wcscmp_not_equal(void)
{
    /* ABC comes before DEF in character set */
    TEST_ASSERT_LESS_THAN_INT(0, wcscmp(L"ABC", L"DEF"));
}

void test_wcscpy_copies_correctly(void)
{
    wchar_t dest[10];
    safe_wcscpy(dest, 10, L"CopyMe");
    
    /* Unity lacks a dedicated TEST_ASSERT_EQUAL_WCHAR_ARRAY, so we use wcscmp */
    TEST_ASSERT_EQUAL_INT(0, wcscmp(L"CopyMe", dest));
}

void test_wchar_size_is_4_bytes(void)
{
    /* Validation specifically for the target Linux environment */
    TEST_ASSERT_EQUAL_size_t(4, sizeof(wchar_t));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_wcslen_counts_characters);
    RUN_TEST(test_wcscmp_equal);
    RUN_TEST(test_wcscmp_not_equal);
    RUN_TEST(test_wcscpy_copies_correctly);
    RUN_TEST(test_wchar_size_is_4_bytes);
    return UNITY_END();
}