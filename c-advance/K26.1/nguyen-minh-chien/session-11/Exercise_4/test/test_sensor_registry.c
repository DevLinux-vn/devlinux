#include "sensor_registry.h"

#include "unity.h"

#include <stdlib.h>

/**
 * @brief Sorted sensor registry used by unit tests.
 */
static const sensor_metadata_t test_registry[] =
{
    {1001U, "Ambient_Temp", -0.150f},
    {1007U, "Temp_Inlet",   -0.340f},
    {1023U, "Voltage_Bus",   0.000f},
    {1034U, "Pressure_B",    0.095f},
    {1042U, "Pressure_A",    0.120f},
    {1055U, "Current_Motor", -0.080f},
    {1088U, "Humidity_Ext",  0.210f},
    {1099U, "Flow_Main",     0.050f}
};

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
 * @brief Verify comparator returns less-than result.
 */
static void test_comparator_less_than(void)
{
    sensor_metadata_t sensor_a =
    {
        .sensor_id = 1001U,
        .name = "A",
        .calibration_offset = 0.0f
    };

    sensor_metadata_t sensor_b =
    {
        .sensor_id = 1002U,
        .name = "B",
        .calibration_offset = 0.0f
    };

    TEST_ASSERT_TRUE(compare_by_id(&sensor_a, &sensor_b) < 0);
}

/**
 * @brief Verify comparator returns greater-than result.
 */
static void test_comparator_greater_than(void)
{
    sensor_metadata_t sensor_a =
    {
        .sensor_id = 1002U,
        .name = "A",
        .calibration_offset = 0.0f
    };

    sensor_metadata_t sensor_b =
    {
        .sensor_id = 1001U,
        .name = "B",
        .calibration_offset = 0.0f
    };

    TEST_ASSERT_TRUE(compare_by_id(&sensor_a, &sensor_b) > 0);
}

/**
 * @brief Verify comparator returns equality result.
 */
static void test_comparator_equal(void)
{
    sensor_metadata_t sensor_a =
    {
        .sensor_id = 1001U,
        .name = "A",
        .calibration_offset = 0.0f
    };

    sensor_metadata_t sensor_b =
    {
        .sensor_id = 1001U,
        .name = "B",
        .calibration_offset = 1.0f
    };

    TEST_ASSERT_EQUAL_INT(0, compare_by_id(&sensor_a, &sensor_b));
}

/**
 * @brief Verify an existing middle sensor can be found.
 */
static void test_find_sensor_existing(void)
{
    const sensor_metadata_t *p_sensor;

    p_sensor = find_sensor(
        test_registry,
        sizeof(test_registry) / sizeof(test_registry[0]),
        1055U);

    TEST_ASSERT_NOT_NULL(p_sensor);
    TEST_ASSERT_EQUAL_UINT32(1055U, p_sensor->sensor_id);
    TEST_ASSERT_EQUAL_STRING("Current_Motor", p_sensor->name);
}

/**
 * @brief Verify search returns NULL for an unknown sensor ID.
 */
static void test_find_sensor_not_found(void)
{
    const sensor_metadata_t *p_sensor;

    p_sensor = find_sensor(
        test_registry,
        sizeof(test_registry) / sizeof(test_registry[0]),
        9999U);

    TEST_ASSERT_NULL(p_sensor);
}

/**
 * @brief Verify first element can be found.
 */
static void test_find_sensor_first_element(void)
{
    const sensor_metadata_t *p_sensor;

    p_sensor = find_sensor(
        test_registry,
        sizeof(test_registry) / sizeof(test_registry[0]),
        1001U);

    TEST_ASSERT_NOT_NULL(p_sensor);
    TEST_ASSERT_EQUAL_UINT32(1001U, p_sensor->sensor_id);
}

/**
 * @brief Verify last element can be found.
 */
static void test_find_sensor_last_element(void)
{
    const sensor_metadata_t *p_sensor;

    p_sensor = find_sensor(
        test_registry,
        sizeof(test_registry) / sizeof(test_registry[0]),
        1099U);

    TEST_ASSERT_NOT_NULL(p_sensor);
    TEST_ASSERT_EQUAL_UINT32(1099U, p_sensor->sensor_id);
}

/**
 * @brief Unit test application entry point.
 *
 * @return Unity test result.
 */
int main(void)
{
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