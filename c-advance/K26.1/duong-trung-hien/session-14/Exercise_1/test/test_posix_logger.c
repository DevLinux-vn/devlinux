#include "posix_logger.h"
#include "unity.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h> 

static const char *TEST_LOG_PATH = "/tmp/test_posix_logger.log";

void setUp(void) {
  /* Remove test file before each test */
  unlink(TEST_LOG_PATH);
}

void tearDown(void) { unlink(TEST_LOG_PATH); }

void test_write_all_basic(void) {
  int32_t fd = open(TEST_LOG_PATH, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  TEST_ASSERT_TRUE(fd >= 0);

  const char *msg = "hello";
  ssize_t written = posix_write_all(fd, (const uint8_t *)msg, strlen(msg));
  TEST_ASSERT_EQUAL_INT(5, (int)written);
  close(fd);

  /* Read back and verify */
  fd = open(TEST_LOG_PATH, O_RDONLY);
  char buf[16] = {0};
  ssize_t n = read(fd, buf, sizeof(buf) - 1U);
  buf[n] = '\0'; /* Must add '\0' manually after read()! */
  TEST_ASSERT_EQUAL_STRING("hello", buf);
  close(fd);
}

void test_log_open_creates_file(void) {
  int32_t fd = posix_log_open(TEST_LOG_PATH);
  TEST_ASSERT_TRUE(fd >= 0);
  posix_log_close(fd);

  /* Verify file was created by opening it read-only */
  int32_t fd_verify = open(TEST_LOG_PATH, O_RDONLY);
  TEST_ASSERT_TRUE(fd_verify >= 0);
  close(fd_verify);
}

void test_log_write_formats_correctly(void) {
  int32_t fd = posix_log_open(TEST_LOG_PATH);
  TEST_ASSERT_TRUE(fd >= 0);

  int32_t result = posix_log_write(fd, 1U, 23.5f, 55U);
  TEST_ASSERT_EQUAL_INT(0, result);
  posix_log_close(fd);

  /* Read back and verify format */
  int32_t fd_r = open(TEST_LOG_PATH, O_RDONLY);
  char buf[128] = {0};
  ssize_t n = read(fd_r, buf, sizeof(buf) - 1U);
  buf[n] = '\0';
  TEST_ASSERT_NOT_NULL(strstr(buf, "[00001]"));
  TEST_ASSERT_NOT_NULL(strstr(buf, "SENSOR"));
  close(fd_r);
}

void test_set_nonblocking_flag(void) {
  int32_t fd = posix_log_open(TEST_LOG_PATH);
  TEST_ASSERT_TRUE(fd >= 0);

  int32_t result = posix_set_nonblocking(fd);
  TEST_ASSERT_EQUAL_INT(0, result);

  /* Verify O_NONBLOCK is set */
  int32_t flags = fcntl(fd, F_GETFL, 0);
  TEST_ASSERT_TRUE((flags & O_NONBLOCK) != 0);
  posix_log_close(fd);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_write_all_basic);
  RUN_TEST(test_log_open_creates_file);
  RUN_TEST(test_log_write_formats_correctly);
  RUN_TEST(test_set_nonblocking_flag);
  return UNITY_END();
}