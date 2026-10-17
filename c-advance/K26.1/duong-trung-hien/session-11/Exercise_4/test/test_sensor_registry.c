#include "sensor_registry.h"
#include "unity.h"
#include <stdint.h>

#define ARR_SIZE(arr)   (sizeof(arr) / sizeof(arr[0]))

/**
 * @brief Sets up the test environment before each test.
 *
 * This function is called by the Unity test framework before
 * executing each test case.
 */
void setUp();

/**
 * @brief Cleans up the test environment after each test.
 *
 * This function is called by the Unity test framework after
 * executing each test case.
 */
void tearDown();

/**
 * @brief Tests the comparator when the first sensor ID is less than
 *        the second sensor ID.
 *
 * Verifies that compare_by_id() returns a negative value.
 */
void test_comparator_less_than();

/**
 * @brief Tests the comparator when the first sensor ID is greater than
 *        the second sensor ID.
 *
 * Verifies that compare_by_id() returns a positive value.
 */
void test_comparator_greater_than();

/**
 * @brief Tests the comparator when both sensor IDs are equal.
 *
 * Verifies that compare_by_id() returns zero.
 */
void test_comparator_equal();

/**
 * @brief Tests finding an existing sensor in the registry.
 *
 * Verifies that find_sensor() returns a pointer to the correct
 * sensor when the target ID exists.
 */
void test_find_sensor_existing();

/**
 * @brief Tests searching for a sensor that does not exist.
 *
 * Verifies that find_sensor() returns NULL when the target ID
 * is not present in the registry.
 */
void test_find_sensor_not_found();

/**
 * @brief Tests finding the first sensor in the sorted registry.
 *
 * Verifies that find_sensor() correctly returns the first element
 * when its sensor ID is searched.
 */
void test_find_sensor_first_element();

/**
 * @brief Tests finding the last sensor in the sorted registry.
 *
 * Verifies that find_sensor() correctly returns the last element
 * when its sensor ID is searched.
 */
void test_find_sensor_last_element();

int32_t main() {
    UNITY_BEGIN();

    RUN_TEST(test_comparator_less_than);
    RUN_TEST(test_comparator_greater_than);
    RUN_TEST(test_comparator_equal);
    RUN_TEST(test_find_sensor_existing);
    RUN_TEST(test_find_sensor_not_found);
    RUN_TEST(test_find_sensor_first_element);
    RUN_TEST(test_find_sensor_last_element);

    return UNITY_END();
}

void setUp() {

}

void tearDown() {

}

void test_comparator_less_than() {
    const sensor_metadata_t a = {
        .sensor_id = 1042U
    };
    const sensor_metadata_t b = {
        .sensor_id = 1020U
    };

    int result = compare_by_id(&b, &a);

    TEST_ASSERT_EQUAL_INT(LOWER, result);
}

void test_comparator_greater_than() {
    const sensor_metadata_t a = {
        .sensor_id = 1042U
    };
    const sensor_metadata_t b = {
        .sensor_id = 1020U
    };

    int result = compare_by_id(&a, &b);

    TEST_ASSERT_EQUAL_INT(HIGHER, result);
}

void test_comparator_equal() {
    const sensor_metadata_t a = {
        .sensor_id = 1042U
    };
    const sensor_metadata_t b = {
        .sensor_id = 1042U
    };

    int result = compare_by_id(&a, &b);

    TEST_ASSERT_EQUAL_INT(EQUAL, result);
}

void test_find_sensor_existing() {
    sensor_metadata_t sensors[] = {
        {1007U, "Temp_Inlet", -0.340f},
        {1042U, "Pressure_A", 0.120f},
        {1099U, "Flow_Main", 0.050f}
    };
    size_t count = ARR_SIZE(sensors);

    const sensor_metadata_t *p_result = find_sensor(sensors, count, 1042U);

    TEST_ASSERT_NOT_NULL(p_result);
}

void test_find_sensor_not_found() {
    sensor_metadata_t sensors[] = {
        {1007U, "Temp_Inlet", -0.340f},
        {1042U, "Pressure_A", 0.120f},
        {1099U, "Flow_Main", 0.050f}
    };
    size_t count = ARR_SIZE(sensors);

    const sensor_metadata_t *p_result = find_sensor(sensors, count, 9999U);

    TEST_ASSERT_NULL(p_result);
}

void test_find_sensor_first_element() {
    sensor_metadata_t sensors[] = {
        {1007U, "Temp_Inlet", -0.340f},
        {1042U, "Pressure_A", 0.120f},
        {1099U, "Flow_Main", 0.050f}
    };
    size_t count = ARR_SIZE(sensors);

    const sensor_metadata_t *p_result = find_sensor(sensors, count, sensors[0].sensor_id);

    TEST_ASSERT_NOT_NULL(p_result);

    if (NULL == p_result) {
        return;
    }

    TEST_ASSERT_EQUAL_UINT32(sensors[0].sensor_id, p_result->sensor_id);
    TEST_ASSERT_EQUAL_STRING(sensors[0].name, p_result->name);
    TEST_ASSERT_EQUAL_FLOAT(sensors[0].calibration_offset, p_result->calibration_offset);
}

void test_find_sensor_last_element() {
    sensor_metadata_t sensors[] = {
        {1007U, "Temp_Inlet", -0.340f},
        {1042U, "Pressure_A", 0.120f},
        {1099U, "Flow_Main", 0.050f}
    };
    size_t count = ARR_SIZE(sensors);

    const sensor_metadata_t *p_result = find_sensor(sensors, count, sensors[count - 1].sensor_id);

    TEST_ASSERT_NOT_NULL(p_result);

    if (NULL == p_result) {
        return;
    }

    TEST_ASSERT_EQUAL_UINT32(sensors[count - 1].sensor_id, p_result->sensor_id);
    TEST_ASSERT_EQUAL_STRING(sensors[count - 1].name, p_result->name);
    TEST_ASSERT_EQUAL_FLOAT(sensors[count - 1].calibration_offset, p_result->calibration_offset);
}