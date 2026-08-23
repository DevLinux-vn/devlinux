/**
 * @file test_sensor_registry.c
 * @brief Unity unit tests for the sensor registry module.
 */
#include "unity.h"
#include "sensor_registry.h"

void setUp(void) {}
void tearDown(void) {}

void test_comparator_less_than(void)
{
    sensor_metadata_t a = {1000, "", 0.0f};
    sensor_metadata_t b = {2000, "", 0.0f};
    TEST_ASSERT_EQUAL_INT(-1, compare_by_id(&a, &b));
}

void test_comparator_greater_than(void)
{
    sensor_metadata_t a = {3000, "", 0.0f};
    sensor_metadata_t b = {2000, "", 0.0f};
    TEST_ASSERT_EQUAL_INT(1, compare_by_id(&a, &b));
}

void test_comparator_equal(void)
{
    sensor_metadata_t a = {2000, "", 0.0f};
    sensor_metadata_t b = {2000, "", 0.0f};
    TEST_ASSERT_EQUAL_INT(0, compare_by_id(&a, &b));
}

void test_find_sensor_existing(void)
{
    sensor_metadata_t reg[3] = {
        {10, "A", 0.1f},
        {20, "B", 0.2f},
        {30, "C", 0.3f}
    };
    const sensor_metadata_t *p_res = find_sensor(reg, 3, 20);
    TEST_ASSERT_NOT_NULL(p_res);
    TEST_ASSERT_EQUAL_UINT32(20, p_res->sensor_id);
    TEST_ASSERT_EQUAL_STRING("B", p_res->name);
}

void test_find_sensor_not_found(void)
{
    sensor_metadata_t reg[3] = {
        {10, "A", 0.1f},
        {20, "B", 0.2f},
        {30, "C", 0.3f}
    };
    const sensor_metadata_t *p_res = find_sensor(reg, 3, 25);
    TEST_ASSERT_NULL(p_res);
}

void test_find_sensor_first_element(void)
{
    sensor_metadata_t reg[3] = {
        {10, "A", 0.1f},
        {20, "B", 0.2f},
        {30, "C", 0.3f}
    };
    const sensor_metadata_t *p_res = find_sensor(reg, 3, 10);
    TEST_ASSERT_NOT_NULL(p_res);
    TEST_ASSERT_EQUAL_UINT32(10, p_res->sensor_id);
}

void test_find_sensor_last_element(void)
{
    sensor_metadata_t reg[3] = {
        {10, "A", 0.1f},
        {20, "B", 0.2f},
        {30, "C", 0.3f}
    };
    const sensor_metadata_t *p_res = find_sensor(reg, 3, 30);
    TEST_ASSERT_NOT_NULL(p_res);
    TEST_ASSERT_EQUAL_UINT32(30, p_res->sensor_id);
}

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