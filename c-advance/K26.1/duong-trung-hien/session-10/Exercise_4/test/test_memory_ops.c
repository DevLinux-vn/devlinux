#include "memory_ops.h"
#include "unity.h"

/**
 * @brief Sets up the test environment before each test case.
 */
void setUp();

/**
 * @brief Cleans up the test environment after each test case.
 */
void tearDown();

/**
 * @brief Tests that a structure is correctly initialized to zero.
 */
void test_struct_zero_init();

/**
 * @brief Tests that memcpy correctly copies binary data containing null bytes.
 */
void test_memcpy_handles_null_bytes();

/**
 * @brief Tests that strncpy cannot correctly handle binary data containing null bytes.
 */
void test_strncpy_fails_on_null_bytes();

/**
 * @brief Tests that memcmp correctly detects differences between binary data.
 */
void test_memcmp_detects_difference();

/**
 * @brief Tests that strcmp fails to detect differences after an embedded null byte.
 */
void test_strcmp_fails_to_detect_difference();

/**
 * @brief Tests that memmove correctly handles overlapping memory regions.
 */
void test_memmove_handles_overlap();

int32_t main() {
    UNITY_BEGIN();

    RUN_TEST(test_struct_zero_init);
    RUN_TEST(test_memcpy_handles_null_bytes);
    RUN_TEST(test_strncpy_fails_on_null_bytes);
    RUN_TEST(test_memcmp_detects_difference);
    RUN_TEST(test_strcmp_fails_to_detect_difference);
    RUN_TEST(test_memmove_handles_overlap);

    return UNITY_END();
}

void setUp() {

}

void tearDown() {

}

void test_struct_zero_init() {
    sensor_data_t data = {
        .id = 12,
        .temp = 215
    };
    sensor_data_zero_init(&data);

    TEST_ASSERT_EQUAL_UINT8(0, data.id);
    TEST_ASSERT_EQUAL_UINT8(0, data.temp);
}

void test_memcpy_handles_null_bytes() {
    uint8_t payload[8] = {
    0x01, 0x00, 0xFF, 0x00,
    0x10, 0x20, 0x30, 0x40
    };
    char dst[8];

    binary_memcpy(dst, &payload, sizeof(payload));

    TEST_ASSERT_EQUAL_MEMORY(dst, payload, sizeof(payload));
}

void test_strncpy_fails_on_null_bytes() {
    uint8_t payload[8] = {
    0x01, 0x00, 0xFF, 0x00,
    0x10, 0x20, 0x30, 0x40
    };
    char dst[8];

    binary_strncpy(dst, (const char *)&payload, sizeof(payload));
    int8_t result = memcmp(dst, payload, sizeof(payload));

    TEST_ASSERT_NOT_EQUAL_INT(0, result);
}

void test_memcmp_detects_difference() {
    uint8_t id1[] = {0, 1, 0, 5};
    uint8_t id2[] = {0, 1, 0, 9};
    int8_t result = binary_memcmp(&id1, &id2, sizeof(id1));

    TEST_ASSERT_NOT_EQUAL_INT(0, result); 
}

void test_strcmp_fails_to_detect_difference() {
    uint8_t id1[] = {0, 1, 0, 5};
    uint8_t id2[] = {0, 1, 0, 9};
    int8_t result = binary_strcmp((const char*)&id1, (const char *)&id2);

    TEST_ASSERT_EQUAL_INT(0, result);
}

void test_memmove_handles_overlap() {
    char buf[16] = "123456789";
    binary_memmove(&buf[5], &buf[3], 6);

    TEST_ASSERT_EQUAL_STRING("12345456789", buf);
}