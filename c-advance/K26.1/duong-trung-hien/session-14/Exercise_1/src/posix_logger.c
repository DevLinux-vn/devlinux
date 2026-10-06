#include "posix_logger.h"
#include <errno.h> /* errno */
#include <fcntl.h> /* O_WRONLY - O_CREAT - O_APPEND */
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h> 

/**
 * @brief Writes all bytes from a buffer to a file descriptor.
 *
 * Continues writing until the requested number of bytes has been written.
 * Retries the write operation if it is interrupted by a signal (EINTR).
 *
 * @param[in] fd File descriptor to write to.
 * @param[in] p_buf Pointer to the buffer containing data to write.
 * @param[in] total Number of bytes to write.
 *
 * @return Number of bytes written on success.
 * @return -1 if the buffer is NULL or a write error occurs.
 */
ssize_t posix_write_all(int32_t fd, const uint8_t *p_buf, size_t total) {
  if (NULL == p_buf) {
    return -1;
  }

  size_t total_written = 0;

  while (total_written < total) {
    ssize_t written = write(fd, (const uint8_t *)p_buf + total_written,
                            total - total_written);

    if (written > 0) {
      total_written += (size_t)written;
    } else if (written == -1) {
      if (errno == EINTR) {
        continue;
      }

      return -1;
    } else {
      return -1;
    }
  }

  return (ssize_t)total_written;
}

/**
 * @brief Opens a log file for writing.
 *
 * The file is opened in write-only mode. If the file does not exist,
 * it is created. New log entries are appended to the end of the file.
 *
 * @param[in] p_path Path to the log file.
 *
 * @return File descriptor on success.
 * @return -1 if the path is NULL or the file cannot be opened.
 */
int32_t posix_log_open(const char *p_path) {
  if (NULL == p_path) {
    return -1;
  }

  int32_t fd = open(p_path, O_WRONLY | O_CREAT | O_APPEND, 0644);

  if (fd == -1) {
    return -1;
  }

  return fd;
}

/**
 * @brief Writes a formatted sensor log entry.
 *
 * Formats the entry ID, temperature, and humidity into a log message
 * and writes the complete message to the specified file descriptor.
 *
 * @param[in] fd File descriptor used for logging.
 * @param[in] entry_id Sensor log entry identifier.
 * @param[in] temperature Sensor temperature in degrees Celsius.
 * @param[in] humidity Sensor humidity percentage.
 *
 * @return 0 on success.
 * @return -1 if formatting or writing fails.
 */
int32_t posix_log_write(int32_t fd, uint32_t entry_id, float temperature,
                        uint32_t humidity) {
  char buffer[64];

  int32_t len = snprintf(buffer, sizeof(buffer),
                         "[%05u] SENSOR: temp=%.1fC humidity=%u%%", entry_id,
                         temperature, humidity);

  if (len < 0) {
    return -1;
  }

  if (posix_write_all(fd, (const uint8_t *)buffer, (size_t)len) == -1) {
    return -1;
  }

  return 0;
}

/**
 * @brief Closes a log file descriptor.
 *
 * @param[in] fd File descriptor to close.
 *
 * @return 0 on success.
 * @return -1 if closing the file descriptor fails.
 */
int32_t posix_log_close(int32_t fd) {
  if (close(fd) == -1) {
    return -1;
  }

  return 0;
}

/**
 * @brief Sets a file descriptor to non-blocking mode.
 *
 * Retrieves the current file status flags and enables O_NONBLOCK
 * while preserving the existing flags.
 *
 * @param[in] fd File descriptor to configure.
 *
 * @return 0 on success.
 * @return -1 if retrieving or setting the file status flags fails.
 */
int32_t posix_set_nonblocking(int32_t fd) {
  int32_t flags;

  flags = fcntl(fd, F_GETFL);
  if (flags == -1) {
    return -1;
  }

  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
    return -1;
  }

  return 0;
}