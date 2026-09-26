#include "device_file_io.h"
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @brief Writes a register snapshot to a file.
 *
 * Writes the specified number of 32-bit register values to a file.
 * The file is created if it does not exist and truncated if it already
 * exists. The write operation is retried when interrupted by a signal.
 *
 * @param[in] p_path Path to the snapshot file.
 * @param[in] p_regs Pointer to the array of register values.
 * @param[in] count Number of 32-bit register values to write.
 *
 * @return 0 on success.
 * @return -1 if an input parameter is invalid, the file cannot be opened,
 *         a write operation fails, or the file cannot be closed.
 */
int32_t reg_snapshot_write(const char *p_path, const uint32_t *p_regs,
                           uint32_t count) {
  if ((NULL == p_path) || (NULL == p_regs) || (count == 0U)) {
    return -1;
  }

  int fd;
  size_t total_bytes;
  size_t byte_writtent = 0U;
  ssize_t writtent;

  total_bytes = (size_t)count * sizeof(uint32_t);

  fd = open(p_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    return -1;
  }

  while (byte_writtent < total_bytes) {
    writtent = write(fd, (const uint8_t *)p_regs + byte_writtent,
                     total_bytes - byte_writtent);

    if (writtent > 0) {
      byte_writtent += (size_t)writtent;
    } else if (writtent == -1) {
      if (errno == EINTR) {
        continue;
      }

      (void)close(fd);
      return -1;
    } else {
      (void)close(fd);
      return -1;
    }
  }

  if (close(fd) < 0) {
    return -1;
  }

  return 0;
}

/**
 * @brief Reads a register snapshot from a file.
 *
 * Reads up to the specified maximum number of 32-bit register values
 * from the snapshot file. The read operation is retried when interrupted
 * by a signal.
 *
 * @param[in] p_path Path to the snapshot file.
 * @param[out] p_regs Pointer to the buffer that receives register values.
 * @param[in] max_count Maximum number of 32-bit register values to read.
 *
 * @return Number of complete 32-bit register values read on success.
 * @return -1 if an input parameter is invalid, the file cannot be opened,
 *         a read operation fails, or the file cannot be closed.
 */
int32_t reg_snapshot_read(const char *p_path, uint32_t *p_regs,
                          uint32_t max_count) {
  if ((NULL == p_path) || (NULL == p_regs) || (max_count == 0U)) {
    return -1;
  }

  int fd;
  size_t max_byte;
  size_t byte_read = 0U;
  ssize_t byte_read_now;

  max_byte = (size_t)max_count * sizeof(uint32_t);

  fd = open(p_path, O_RDONLY);
  if (fd < 0) {
    return -1;
  }

  while (byte_read < max_byte) {
    byte_read_now =
        read(fd, (uint8_t *)p_regs + byte_read, max_byte - byte_read);

    if (byte_read_now > 0) {
      byte_read += (size_t)byte_read_now;
    } else if (byte_read_now == 0U) {
      break;
    } else {
      if (errno == EINTR) {
        continue;
      }

      (void)close(fd);
      return -1;
    }
  }

  if (close(fd) < 0) {
    return -1;
  }

  return (int32_t)(byte_read / sizeof(uint32_t));
}

/**
 * @brief Parses a raw register value into individual fields.
 *
 * Extracts the enabled, mode, sensor raw value, and threshold fields
 * from the specified 32-bit register value and stores them in the
 * provided output structure.
 *
 * @param[in] raw_reg Raw 32-bit register value.
 * @param[out] p_out Pointer to the structure that receives the parsed fields.
 *
 * @return None.
 */
void reg_snapshot_parse(uint32_t raw_reg, reg_fields_t *p_out) {
  if (NULL == p_out) {
    return;
  }

  p_out->enabled = FIELD_GET(REG_ENABLED, raw_reg);
  p_out->mode = FIELD_GET(REG_MODE, raw_reg);
  p_out->sensor_raw = FIELD_GET(REG_SENSOR_RAW, raw_reg);
  p_out->threshold = FIELD_GET(REG_THRESHOLD, raw_reg);
}