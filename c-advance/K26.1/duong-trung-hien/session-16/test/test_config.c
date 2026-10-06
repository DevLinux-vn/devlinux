#include "alert.h"
#include "common_types.h"
#include "config.h"
#include "unity.h"
#include <stdio.h>
#include <unity_internals.h>

#define TEST_CONFIG_FILE "test_config.txt"
#define TEST_MALFORMED_CONFIG_FILE "test_config_malformed.txt"
#define TEST_CLAMP_CONFIG_FILE "test_config_clamp.txt"

void setUp(void);
void tearDown(void);

void test_config_save_load_round_trip(void);
void test_config_missing_file_uses_defaults(void);
void test_config_malformed_line_skipped(void);
void test_config_out_of_range_clamped(void);

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_config_save_load_round_trip);
  RUN_TEST(test_config_missing_file_uses_defaults);
  RUN_TEST(test_config_malformed_line_skipped);
  RUN_TEST(test_config_out_of_range_clamped);

  return UNITY_END();
}

void setUp(void) {
  (void)remove(TEST_CONFIG_FILE);
  (void)remove(TEST_MALFORMED_CONFIG_FILE);
  (void)remove(TEST_CLAMP_CONFIG_FILE);
}

void tearDown(void) {
  (void)remove(TEST_CONFIG_FILE);
  (void)remove(TEST_MALFORMED_CONFIG_FILE);
  (void)remove(TEST_CLAMP_CONFIG_FILE);
}

void test_config_save_load_round_trip(void) {
  const alert_config_t expected = {
      .temp_max = 400,
      .temp_min = 50,
      .humi_max = 900,
      .humi_min = 100,
  };

  alert_config_t actual;

  TEST_ASSERT_EQUAL(STATUS_OK, config_save(TEST_CONFIG_FILE, &expected));

  TEST_ASSERT_EQUAL(STATUS_OK, config_load(TEST_CONFIG_FILE, &actual));

  TEST_ASSERT_EQUAL_INT16(expected.temp_max, actual.temp_max);
  TEST_ASSERT_EQUAL_INT16(expected.temp_min, actual.temp_min);
  TEST_ASSERT_EQUAL_INT16(expected.humi_max, actual.humi_max);
  TEST_ASSERT_EQUAL_INT16(expected.humi_min, actual.humi_min);
}

void test_config_missing_file_uses_defaults(void) {
  alert_config_t config;

  TEST_ASSERT_EQUAL(STATUS_OK,
                    config_load("file_that_does_not_exist.txt", &config));

  TEST_ASSERT_EQUAL_INT16(CONFIG_DEFAULT_TEMP_MAX, config.temp_max);

  TEST_ASSERT_EQUAL_INT16(CONFIG_DEFAULT_TEMP_MIN, config.temp_min);

  TEST_ASSERT_EQUAL_INT16(CONFIG_DEFAULT_HUMI_MAX, config.humi_max);

  TEST_ASSERT_EQUAL_INT16(CONFIG_DEFAULT_HUMI_MIN, config.humi_min);
}

void test_config_malformed_line_skipped(void) {
  FILE *p_file;
  alert_config_t config;

  p_file = fopen(TEST_MALFORMED_CONFIG_FILE, "w");

  TEST_ASSERT_NOT_NULL(p_file);

  if (p_file != NULL) {
    const char *config_text = "temp_max=400\n"
                              "this_is_not_valid\n"
                              "humi_max=900\n";
    (void)fputs(config_text, p_file);
    (void)fclose(p_file);
  }

  TEST_ASSERT_EQUAL(STATUS_OK,
                    config_load(TEST_MALFORMED_CONFIG_FILE, &config));

  TEST_ASSERT_EQUAL_INT16(400, config.temp_max);
  TEST_ASSERT_EQUAL_INT16(900, config.humi_max);

  TEST_ASSERT_EQUAL_INT16(CONFIG_DEFAULT_TEMP_MIN, config.temp_min);

  TEST_ASSERT_EQUAL_INT16(CONFIG_DEFAULT_HUMI_MIN, config.humi_min);
}

void test_config_out_of_range_clamped(void) {
  FILE *p_file;
  alert_config_t config;

  p_file = fopen(TEST_CLAMP_CONFIG_FILE, "w");

  TEST_ASSERT_NOT_NULL(p_file);

  if (p_file != NULL) {
    const char *config_text = "temp_max=9999\n"
                              "temp_min=-9999\n"
                              "humi_max=9999\n"
                              "humi_min=-9999\n";
    (void)fputs(config_text, p_file);
    (void)fclose(p_file);
  }

  TEST_ASSERT_EQUAL(STATUS_OK, config_load(TEST_CLAMP_CONFIG_FILE, &config));

  TEST_ASSERT_EQUAL_INT16(CONFIG_TEMP_LIMIT_MAX, config.temp_max);

  TEST_ASSERT_EQUAL_INT16(CONFIG_TEMP_LIMIT_MIN, config.temp_min);

  TEST_ASSERT_EQUAL_INT16(CONFIG_HUMI_LIMIT_MAX, config.humi_max);

  TEST_ASSERT_EQUAL_INT16(CONFIG_HUMI_LIMIT_MIN, config.humi_min);
}