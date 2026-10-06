#include "config.h"
#include "alert.h"
#include "common_types.h"
#include "logger.h"
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CONFIG_LINE_SIZE (64U)
#define CONFIG_READ_BUFFER_SIZE (128U)

static void config_set_defaults(alert_config_t *p_config);

static int16_t config_clamp_value(long value, int16_t min_value,
                                  int16_t max_value);

static status_t config_parse_line(char *p_line, alert_config_t *p_config);

static status_t config_parse_value(const char *p_key, const char *p_value,
                                   int16_t min_value, int16_t max_value,
                                   int16_t *p_destination);

static status_t config_write_all(int fd, const char *p_buf, size_t length);

static void config_set_defaults(alert_config_t *p_config) {
  p_config->temp_max = CONFIG_DEFAULT_TEMP_MAX;
  p_config->temp_min = CONFIG_DEFAULT_TEMP_MIN;
  p_config->humi_max = CONFIG_DEFAULT_HUMI_MAX;
  p_config->humi_min = CONFIG_DEFAULT_HUMI_MIN;
}

static int16_t config_clamp_value(long value, int16_t min_value,
                                  int16_t max_value) {
  if (value < (long)min_value) {
    return min_value;
  }

  if (value > (long)max_value) {
    return max_value;
  }

  return (int16_t)value;
}

static status_t config_parse_value(const char *p_key, const char *p_value,
                                   int16_t min_value, int16_t max_value,
                                   int16_t *p_destination) {
  char *p_end = NULL;
  long value;

  if ((p_key == NULL) || (p_value == NULL) || (p_destination == NULL)) {
    return STATUS_ERR_NULL_PTR;
  }

  errno = 0;
  value = strtol(p_value, &p_end, 10);

  if ((p_end == p_value) || (errno == ERANGE) || (*p_end != '\0')) {
    LOG_WARN("Malformed value for key '%s': %s", p_key, p_value);
    return STATUS_ERR_INVALID_PARAM;
  }

  if (value < (long)min_value) {
    LOG_WARN("Value for %s below range, clamping", p_key);
  } else if (value > (long)max_value) {
    LOG_WARN("Value for %s above range, clamping", p_key);
  }

  *p_destination = config_clamp_value(value, min_value, max_value);

  return STATUS_OK;
}

static status_t config_parse_line(char *p_line, alert_config_t *p_config) {
  char *p_separator;
  char *p_key;
  char *p_value;
  size_t key_length;

  if ((p_line == NULL) || (p_config == NULL)) {
    return STATUS_ERR_NULL_PTR;
  }

  p_separator = strchr(p_line, '=');

  if (p_separator == NULL) {
    LOG_WARN("Malformed config line: %s", p_line);
    return STATUS_ERR_INVALID_PARAM;
  }

  *p_separator = '\0';

  p_key = p_line;
  p_value = p_separator + 1;

  while ((*p_key == ' ') || (*p_key == '\t')) {
    ++p_key;
  }

  key_length = strlen(p_key);

  while ((key_length > 0U) && ((p_key[key_length - 1U] == ' ') ||
                               (p_key[key_length - 1U] == '\t'))) {
    p_key[key_length - 1U] = '\0';
    --key_length;
  }

  while ((*p_value == ' ') || (*p_value == '\t')) {
    ++p_value;
  }

  key_length = strlen(p_value);

  while ((key_length > 0U) && ((p_value[key_length - 1U] == ' ') ||
                               (p_value[key_length - 1U] == '\t'))) {
    p_value[key_length - 1U] = '\0';
    --key_length;
  }

  if (strcmp(p_key, "temp_max") == 0) {
    return config_parse_value(p_key, p_value, CONFIG_TEMP_LIMIT_MIN,
                              CONFIG_TEMP_LIMIT_MAX, &p_config->temp_max);
  }

  if (strcmp(p_key, "temp_min") == 0) {
    return config_parse_value(p_key, p_value, CONFIG_TEMP_LIMIT_MIN,
                              CONFIG_TEMP_LIMIT_MAX, &p_config->temp_min);
  }

  if (strcmp(p_key, "humi_max") == 0) {
    return config_parse_value(p_key, p_value, CONFIG_HUMI_LIMIT_MIN,
                              CONFIG_HUMI_LIMIT_MAX, &p_config->humi_max);
  }

  if (strcmp(p_key, "humi_min") == 0) {
    return config_parse_value(p_key, p_value, CONFIG_HUMI_LIMIT_MIN,
                              CONFIG_HUMI_LIMIT_MAX, &p_config->humi_min);
  }

  LOG_WARN("Unknown config key: %s", p_key);

  return STATUS_ERR_INVALID_PARAM;
}

status_t config_load(const char *p_file_path, alert_config_t *p_config) {
  int fd;
  char read_buffer[CONFIG_READ_BUFFER_SIZE];
  char line_buffer[CONFIG_LINE_SIZE];
  size_t line_length = 0U;
  ssize_t bytes_read;
  status_t status = STATUS_OK;

  if ((p_file_path == NULL) || (p_config == NULL)) {
    return STATUS_ERR_NULL_PTR;
  }

  config_set_defaults(p_config);

  fd = open(p_file_path, O_RDONLY);

  if (fd < 0) {
    if (errno == ENOENT) {
      LOG_WARN("%s", "Config file not found, using defaults");
      return STATUS_OK;
    }

    LOG_ERROR("%s", "Failed to open config file");
    return STATUS_ERR_INVALID_PARAM;
  }

  do {
    bytes_read = read(fd, read_buffer, sizeof(read_buffer));

    if (bytes_read < 0) {
      LOG_ERROR("%s", "Failed to read config file");
      status = STATUS_ERR_INVALID_PARAM;
      break;
    }

    for (ssize_t index = 0; index < bytes_read; ++index) {
      const char character = read_buffer[index];

      if (character == '\n') {
        line_buffer[line_length] = '\0';

        if (line_length > 0U) {
          (void)config_parse_line(line_buffer, p_config);
        }

        line_length = 0U;
      } else if (line_length < (CONFIG_LINE_SIZE - 1U)) {
        line_buffer[line_length++] = character;
      } else {
        LOG_WARN("%s", "Config line too long, skipping");

        line_length = 0U;

        while ((index + 1 < bytes_read) && (read_buffer[index + 1] != '\n')) {
          ++index;
        }
      }
    }
  } while (bytes_read > 0);

  if ((status == STATUS_OK) && (line_length > 0U)) {
    line_buffer[line_length] = '\0';
    (void)config_parse_line(line_buffer, p_config);
  }

  if (close(fd) != 0) {
    LOG_ERROR("%s", "Failed to close config file");
    status = STATUS_ERR_INVALID_PARAM;
  }

  if (status == STATUS_OK) {
    LOG_INFO("Config loaded: temp_max=%d.%dC, temp_min=%d.%dC, "
             "humi_max=%d.%d%%, humi_min=%d.%d%%",
             (int)(p_config->temp_max / 10), (int)(p_config->temp_max % 10),
             (int)(p_config->temp_min / 10), (int)(p_config->temp_min % 10),
             (int)(p_config->humi_max / 10), (int)(p_config->humi_max % 10),
             (int)(p_config->humi_min / 10), (int)(p_config->humi_min % 10));
  }

  return status;
}

static status_t config_write_all(int fd, const char *p_buf, size_t length) {
  size_t total_written = 0U;

  while (total_written < length) {
    const ssize_t written =
        write(fd, &p_buf[total_written], length - total_written);

    if (written <= 0) {
      return STATUS_ERR_INVALID_PARAM;
    }

    total_written += (size_t)written;
  }

  return STATUS_OK;
}

status_t config_save(const char *p_file_path, const alert_config_t *p_config) {
  int fd;
  char line_buffer[CONFIG_LINE_SIZE];
  int written;
  status_t status = STATUS_OK;

  if ((p_file_path == NULL) || (p_config == NULL)) {
    return STATUS_ERR_NULL_PTR;
  }

  fd = open(p_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);

  if (fd < 0) {
    LOG_ERROR("%s", "Failed to open config file for writing");
    return STATUS_ERR_INVALID_PARAM;
  }

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  written = snprintf(line_buffer, sizeof(line_buffer), "temp_max=%d\n",
                     (int)p_config->temp_max);

  if ((written < 0) || ((size_t)written >= sizeof(line_buffer))) {
    status = STATUS_ERR_INVALID_PARAM;
  } else {
    status = config_write_all(fd, line_buffer, (size_t)written);
  }

  if (status == STATUS_OK) {
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    written = snprintf(line_buffer, sizeof(line_buffer), "temp_min=%d\n",
                       (int)p_config->temp_min);

    if ((written < 0) || ((size_t)written >= sizeof(line_buffer))) {
      status = STATUS_ERR_INVALID_PARAM;
    } else {
      status = config_write_all(fd, line_buffer, (size_t)written);
    }
  }

  if (status == STATUS_OK) {
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    written = snprintf(line_buffer, sizeof(line_buffer), "humi_max=%d\n",
                       (int)p_config->humi_max);

    if ((written < 0) || ((size_t)written >= sizeof(line_buffer))) {
      status = STATUS_ERR_INVALID_PARAM;
    } else {
      status = config_write_all(fd, line_buffer, (size_t)written);
    }
  }

  if (status == STATUS_OK) {
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    written = snprintf(line_buffer, sizeof(line_buffer), "humi_min=%d\n",
                       (int)p_config->humi_min);

    if ((written < 0) || ((size_t)written >= sizeof(line_buffer))) {
      status = STATUS_ERR_INVALID_PARAM;
    } else {
      status = config_write_all(fd, line_buffer, (size_t)written);
    }
  }

  if (close(fd) != 0) {
    status = STATUS_ERR_INVALID_PARAM;
  }

  return status;
}