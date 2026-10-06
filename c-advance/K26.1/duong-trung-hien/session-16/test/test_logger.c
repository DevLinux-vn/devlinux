#include "logger.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unity.h>
#include <unity_internals.h>

/**
 * @brief Reads the contents of the log file into a buffer.
 *
 * @param[out] p_buffer Buffer used to store the log file contents.
 * @param[in] buffer_size Size of the buffer in bytes.
 */
static void read_log_file(char *p_buffer, size_t buffer_size);

/**
 * @brief Sets up the test environment before each test case.
 */
void setUp(void);

/**
 * @brief Cleans up the test environment after each test case.
 */
void tearDown(void);

/**
 * @brief Verifies that messages below the minimum log level are filtered.
 */
void test_logger_filters_below_min_level(void);

/**
 * @brief Verifies that log messages are formatted correctly at INFO level.
 */
void test_logger_formats_info_message(void);

/**
 * @brief Verifies that log messages are written correctly to the log file.
 */
void test_logger_writes_to_file(void);

int32_t main() {
  UNITY_BEGIN();

  RUN_TEST(test_logger_filters_below_min_level);
  RUN_TEST(test_logger_formats_info_message);
  RUN_TEST(test_logger_writes_to_file);

  return UNITY_END();
}

static void read_log_file(char *p_buffer, size_t buffer_size) {
  FILE *p_file = fopen("test_logger.log", "r");
  size_t bytes_read;

  TEST_ASSERT_NOT_NULL(p_file);

  if (NULL == p_file) {
    return;
  }

  bytes_read = fread(p_buffer, 1U, buffer_size - 1U, p_file);
  p_buffer[bytes_read] = '\0';

  (void)fclose(p_file);
}

void setUp() {}

void tearDown() { logger_close(); }

void test_logger_filters_below_min_level() {
  char buffer[512] = {0};

  logger_init(LOG_LEVEL_INFO, "test_logger.log");

  LOG_DEBUG("%s", "debug message");
  LOG_INFO("%s", "info message");

  logger_close();

  read_log_file(buffer, sizeof(buffer));

  TEST_ASSERT_NULL(strstr(buffer, "debug message"));
  TEST_ASSERT_NOT_NULL(strstr(buffer, "info message"));
}

void test_logger_formats_info_message() {
  char buffer[512] = {0};

  logger_init(LOG_LEVEL_INFO, "test_logger.log");

  LOG_INFO("Temperature = %d", 42);

  logger_close();

  read_log_file(buffer, sizeof(buffer));

  TEST_ASSERT_NOT_NULL(strstr(buffer, "[INFO]"));
  TEST_ASSERT_NOT_NULL(strstr(buffer, "Temperature = 42"));
  TEST_ASSERT_NOT_NULL(strstr(buffer, "test_logger_formats_info_message"));
}

void test_logger_writes_to_file() {
  char buffer[512] = {0};

  logger_init(LOG_LEVEL_INFO, "test_logger.log");

  LOG_INFO("%s", "File output test");

  logger_close();

  read_log_file(buffer, sizeof(buffer));

  TEST_ASSERT_NOT_NULL(strstr(buffer, "File output test"));
}
