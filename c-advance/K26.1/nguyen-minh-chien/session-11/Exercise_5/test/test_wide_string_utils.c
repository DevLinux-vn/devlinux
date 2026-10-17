#include "wide_string_utils.h"

#include "unity.h"

#include <stddef.h>
#include <wchar.h>

/**
 * @brief Unity setup function.
 */
void setUp(void)
{
}

/**
 * @brief Unity teardown function.
 */
void tearDown(void)
{
}

/**
 * @brief Verify wcslen counts wide characters correctly.
 */
static void test_wcslen_counts_characters(void)
{
    const wchar_t text[] = L"Hello";

    TEST_ASSERT_EQUAL_UINT64(5U, wcslen(text));
}

/**
 * @brief Verify equal wide strings compare as equal.
 */
static void test_wcscmp_equal(void)
{
    TEST_ASSERT_EQUAL_INT(
        0,
        wide_string_compare(L"ABC", L"ABC"));
}

/**
 * @brief Verify different wide strings do not compare as equal.
 */
static void test_wcscmp_not_equal(void)
{
    TEST_ASSERT_TRUE(
        wide_string_compare(L"ABC", L"DEF") < 0);
}

/**
 * @brief Verify wide string copy works correctly.
 */
static void test_wcscpy_copies_correctly(void)
{
    wchar_t destination[32] = L"";

    int result = wide_string_copy(
        destination,
        sizeof(destination) / sizeof(destination[0]),
        L"Héllo");

    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(0, wcscmp(L"Héllo", destination));
}

/**
 * @brief Verify wchar_t size on Linux.
 *
 * This exercise expects Linux, where wchar_t is normally 4 bytes.
 */
static void test_wchar_size_is_4_bytes(void)
{
    TEST_ASSERT_EQUAL_UINT64(4U, sizeof(wchar_t));
}

/**
 * @brief Unit test entry point.
 *
 * @return Unity test result.
 */
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