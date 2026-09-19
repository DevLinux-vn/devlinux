#include "posix_logger.h"
#include <errno.h> /* errno */
#include <fcntl.h> /* O_WRONLY - O_CREAT - O_APPEND */
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h> 

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

  return (size_t)total_written;
}

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

int32_t posix_log_write(int32_t fd, uint32_t entry_id, float temperature,
                        uint32_t humidity) {
  char buffer[64];

  int32_t len = snprintf(buffer, sizeof(buffer),
                         "[%05u] SENSOR: temp=%.1fC humidity=%u%%", entry_id,
                         temperature, humidity);

  if (len < 0) {
    return -1;
  }

  if (posix_write_all(fd, (const uint8_t *)buffer, len) == -1) {
    return -1;
  }

  return 0;
}

int32_t posix_log_close(int32_t fd) {
  if (close(fd) == -1) {
    return -1;
  }

  return 0;
}

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