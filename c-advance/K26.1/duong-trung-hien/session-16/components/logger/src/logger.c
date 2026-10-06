#include "logger.h"

#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LOGGER_MESSAGE_SIZE (256U)
#define LOGGER_LINE_SIZE (512U)

/**
 * @brief Converts a log level to its string representation.
 *
 * @param[in] level Log level to convert.
 * @return String representation of the specified log level.
 */
static const char *logger_level_to_string(log_level_t level);

static int s_log_fd = -1;
static log_level_t s_min_level;

void logger_init(log_level_t min_level, const char *p_file_path) {
  if (NULL == p_file_path) {
    return;
  }

  s_min_level = min_level;
  s_log_fd = open(p_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);

  if (s_log_fd < 0) {
    perror("logger: open");
  }
}

void logger_log(log_level_t level, const char *p_file, int line,
                const char *p_func, const char *p_fmt, ...) {
  if (NULL == p_file || NULL == p_func || NULL == p_fmt) {
    return;
  }

  if (level < s_min_level) {
    return;
  }

  if (s_log_fd < 0) {
    perror("logger: open");
  }

  char message[LOGGER_MESSAGE_SIZE];
  char log_line[LOGGER_LINE_SIZE];

  va_list args;
  va_start(args, p_fmt);

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  (void)vsnprintf(message, sizeof(message), p_fmt, args);

  va_end(args);

  const char *p_level_string = logger_level_to_string(level);

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  (void)snprintf(log_line, sizeof(log_line), "[%s] %s:%d (%s): %s\n",
                 p_level_string, p_file, line, p_func, message);

  (void)write(s_log_fd, log_line, strlen(log_line));
}

void logger_close(void) {
  if (s_log_fd >= 0) {
    (void)close(s_log_fd);
    s_log_fd = -1;
  }
}

static const char *logger_level_to_string(log_level_t level) {
  switch (level) {
  case LOG_LEVEL_DEBUG:
    return "DEBUG";

  case LOG_LEVEL_INFO:
    return "INFO";

  case LOG_LEVEL_WARN:
    return "WARN";

  case LOG_LEVEL_ERROR:
    return "ERROR";

  default:
    return "UNKNOWN";
  }
}