#ifndef POSIX_LOGGER_H
#define POSIX_LOGGER_H

#include <stdint.h>
#include <sys/types.h> /* ssize_t */

/**
 * @brief Write ALL bytes to a file descriptor, handling partial writes.
 *
 * Retries on partial writes and EINTR. This is the safe pattern
 * for POSIX write() — never assume a single write() transfers all bytes.
 *
 * @param[in] fd     Open file descriptor.
 * @param[in] p_buf  Pointer to data buffer.
 * @param[in] total  Total number of bytes to write.
 * @return Total bytes written on success, -1 on error.
 */
ssize_t posix_write_all(int32_t fd, const uint8_t *p_buf, size_t total);

/**
 * @brief Open a log file using POSIX open() with append mode.
 *
 * Creates the file if it doesn't exist (permission 0644).
 * Opens in append mode so multiple runs append to the same log.
 *
 * @param[in] p_path  Path to the log file.
 * @return File descriptor (>= 0) on success, -1 on error.
 */
int32_t posix_log_open(const char *p_path);

/**
 * @brief Write a formatted sensor log entry to the file.
 *
 * Formats the entry with snprintf() into a local buffer, then writes
 * using posix_write_all(). Does NOT write the null terminator to the file.
 *
 * @param[in] fd          Open file descriptor.
 * @param[in] entry_id    Zero-padded 5-digit entry number.
 * @param[in] temperature Temperature reading (e.g., 23.5).
 * @param[in] humidity    Humidity percentage (e.g., 55).
 * @return 0 on success, -1 on error.
 */
int32_t posix_log_write(int32_t fd, uint32_t entry_id, float temperature,
                        uint32_t humidity);

/**
 * @brief Close the log file descriptor with error checking.
 * @param[in] fd  File descriptor to close.
 * @return 0 on success, -1 on error.
 */
int32_t posix_log_close(int32_t fd);

/**
 * @brief Set a file descriptor to non-blocking mode using fcntl().
 *
 * Uses F_GETFL/F_SETFL to add O_NONBLOCK without disturbing other flags.
 *
 * @param[in] fd  Open file descriptor.
 * @return 0 on success, -1 on error.
 */
int32_t posix_set_nonblocking(int32_t fd);

#endif /* POSIX_LOGGER_H */