#include "device_file_io.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>

int32_t reg_snapshot_write(const char *p_path, const uint32_t *p_regs, uint32_t count)
{
    if ((p_path == NULL) || (p_regs == NULL) || (count == 0U))
    {
        return -1;
    }

    int32_t fd = open(p_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        return -1;
    }

    size_t total_bytes = count * sizeof(uint32_t);
    size_t bytes_written = 0U;
    const uint8_t *p_buf = (const uint8_t *)p_regs;

    while (bytes_written < total_bytes)
    {
        ssize_t res = write(fd, p_buf + bytes_written, total_bytes - bytes_written);
        if (res < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            close(fd);
            return -1;
        }
        bytes_written += (size_t)res;
    }

    close(fd);
    return 0;
}

int32_t reg_snapshot_read(const char *p_path, uint32_t *p_regs, uint32_t max_count)
{
    if ((p_path == NULL) || (p_regs == NULL) || (max_count == 0U))
    {
        return -1;
    }

    int32_t fd = open(p_path, O_RDONLY);
    if (fd < 0)
    {
        return -1;
    }

    size_t max_bytes = max_count * sizeof(uint32_t);
    size_t bytes_read = 0U;
    uint8_t *p_buf = (uint8_t *)p_regs;

    while (bytes_read < max_bytes)
    {
        ssize_t res = read(fd, p_buf + bytes_read, max_bytes - bytes_read);
        if (res < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            close(fd);
            return -1;
        }
        if (res == 0)
        {
            break; /* EOF */
        }
        bytes_read += (size_t)res;
    }

    close(fd);
    return (int32_t)(bytes_read / sizeof(uint32_t));
}

void reg_snapshot_parse(uint32_t raw_reg, reg_fields_t *p_out)
{
    if (p_out != NULL)
    {
        p_out->enabled    = FIELD_GET(REG_ENABLED, raw_reg);
        p_out->mode       = FIELD_GET(REG_MODE, raw_reg);
        p_out->threshold  = FIELD_GET(REG_THRESHOLD, raw_reg);
        p_out->sensor_raw = FIELD_GET(REG_SENSOR_RAW, raw_reg);
    }
}