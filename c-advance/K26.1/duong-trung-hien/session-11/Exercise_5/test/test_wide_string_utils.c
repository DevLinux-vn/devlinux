#include <stdint.h>
#include "wide_string_utils.h"
#include "unity.h"
#include <wchar.h>
#include <wctype.h>

/**
 * @brief Set up the test environment before each test case.
 */
void setUp();

/**
 * @brief Clean up the test environment after each test case.
 */
void tearDown();

/**
 * @brief Test that wcslen() correctly counts wide characters.
 */
void test_wcslen_counts_characters();

/**
 * @brief Test that wcscmp() returns zero for equal strings.
 */
void test_wcscmp_equal();

/**
 * @brief Test that wcscmp() detects different strings.
 */
void test_wcscmp_not_equal();

/**
 * @brief Test that wcscpy() correctly copies a wide-character string.
 */
void test_wcscpy_copies_correctly();

/**
 * @brief Test that wchar_t occupies 4 bytes on the target platform.
 */
void test_wchar_size_is_4_bytes();

int32_t main() {
    UNITY_BEGIN();

    RUN_TEST(test_wcslen_counts_characters);
    RUN_TEST(test_wcscmp_equal);
    RUN_TEST(test_wcscmp_not_equal);
    RUN_TEST(test_wcscpy_copies_correctly);
    RUN_TEST(test_wchar_size_is_4_bytes);

    return UNITY_END();
}

void setUp() {

}

void tearDown() {

}

void test_wcslen_counts_characters() {
    const wchar_t greeting[] = L"Héllo Wörld! π Ω";
    size_t length = wide_string_length(greeting);

    TEST_ASSERT_EQUAL_size_t(16U, length);
}

void test_wcscmp_equal() {
    int result = wide_string_compare(L"ABC", L"ABC");

    TEST_ASSERT_EQUAL_INT(0, result);
}

void test_wcscmp_not_equal() {
    int result = wide_string_compare(L"ABC", L"DCE");

    TEST_ASSERT_NOT_EQUAL_INT(0, result);
}

void test_wcscpy_copies_correctly() {
    const wchar_t greeting[] = L"Héllo Wörld! π Ω";
    wchar_t restored[40U];

    wide_string_copy(restored, 40U, greeting);
    int result = wide_string_compare(restored, greeting);

    TEST_ASSERT_EQUAL_INT(0, result);
}

void test_wchar_size_is_4_bytes() {
    size_t size = sizeof(wchar_t);

    TEST_ASSERT_EQUAL_size_t(4U, size);
}