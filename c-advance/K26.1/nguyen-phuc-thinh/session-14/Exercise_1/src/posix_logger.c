#include "posix_logger.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

ssize_t posix_write_all(int32_t fd, const uint8_t *p_buf, size_t total)
{
    if ((fd < 0) || (p_buf == NULL))
    {
        return -1;
    }

    size_t written = 0U;
    while (written < total)
    {
        ssize_t res = write(fd, p_buf + written, total - written);
        if (res < 0)
        {
            if (errno == EINTR)
            {
                continue; /* Interrupted by signal, retry immediately */
            }
            return -1;
        }
        written += (size_t)res;
    }
    return (ssize_t)written;
}

int32_t posix_log_open(const char *p_path)
{
    if (p_path == NULL)
    {
        return -1;
    }
    return open(p_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
}

int32_t posix_log_write(int32_t fd, uint32_t entry_id, float temperature, uint32_t humidity)
{
    if (fd < 0)
    {
        return -1;
    }

    char buf[128];
    int res = snprintf(buf, sizeof(buf), "[%05u] SENSOR: temp=%.1fC humidity=%u%%\n", entry_id, temperature, humidity);
    
    if ((res < 0) || ((size_t)res >= sizeof(buf)))
    {
        return -1;
    }

    ssize_t written = posix_write_all(fd, (const uint8_t *)buf, (size_t)res);
    if (written != (ssize_t)res)
    {
        return -1;
    }

    return 0;
}

int32_t posix_log_close(int32_t fd)
{
    if (fd < 0)
    {
        return -1;
    }
    return close(fd);
}

int32_t posix_set_nonblocking(int32_t fd)
{
    if (fd < 0)
    {
        return -1;
    }

    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        return -1;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        return -1;
    }

    return 0;
}