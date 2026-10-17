#ifndef POSIX_LOGGER_H
#define POSIX_LOGGER_H

#include <stdint.h>
#include <sys/types.h>

/**
 * @brief Write ALL bytes to a file descriptor, handling partial writes.
 * @param[in] fd      Open file descriptor.
 * @param[in] p_buf   Pointer to data buffer.
 * @param[in] total   Total number of bytes to write.
 * @return Total bytes written on success, -1 on error.
 */
ssize_t posix_write_all(int32_t fd, const uint8_t *p_buf, size_t total);

/**
 * @brief Open a log file using POSIX open() with append mode.
 * @param[in] p_path  Path to the log file.
 * @return File descriptor (>= 0) on success, -1 on error.
 */
int32_t posix_log_open(const char *p_path);

/**
 * @brief Write a formatted sensor log entry to the file.
 * @param[in] fd          Open file descriptor.
 * @param[in] entry_id    Zero-padded 5-digit entry number.
 * @param[in] temperature Temperature reading (e.g., 23.5).
 * @param[in] humidity    Humidity percentage (e.g., 55).
 * @return 0 on success, -1 on error.
 */
int32_t posix_log_write(int32_t fd, uint32_t entry_id, float temperature, uint32_t humidity);

/**
 * @brief Close the log file descriptor with error checking.
 * @param[in] fd  File descriptor to close.
 * @return 0 on success, -1 on error.
 */
int32_t posix_log_close(int32_t fd);

/**
 * @brief Set a file descriptor to non-blocking mode using fcntl().
 * @param[in] fd  Open file descriptor.
 * @return 0 on success, -1 on error.
 */
int32_t posix_set_nonblocking(int32_t fd);

#endif /* POSIX_LOGGER_H */