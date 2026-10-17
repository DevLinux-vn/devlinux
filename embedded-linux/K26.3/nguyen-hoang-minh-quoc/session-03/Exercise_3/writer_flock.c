#define _POSIX_C_SOURCE 200809L

/*
 * | Property | flock | fcntl |
 * |---|---|---|
 * | Lock granularity | Whole file only | Byte range supported |
 * | Works over NFS | No | Yes |
 * | Inherited across fork | Yes | No |
 * | Best used when | Simple local file locking | Network FS or byte-range locking |
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <time.h>
#include <unistd.h>

#define LOG_FILE "system.log"

static ssize_t write_full(int fd, const void *buf, size_t count)
{
    const char *p = buf;
    size_t written = 0;

    while (written < count) {
        ssize_t n = write(fd, p + written, count - written);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        written += (size_t)n;
    }

    return (ssize_t)written;
}

static void make_timestamp(char *buf, size_t size)
{
    time_t now = time(NULL);
    struct tm tm_now;

    localtime_r(&now, &tm_now);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", &tm_now);
}

int main(int argc, char *argv[])
{
    int fd;
    char timestamp[32];
    char line[512];
    int len;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s \"message text\"\n", argv[0]);
        return 1;
    }

    fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    if (flock(fd, LOCK_EX) < 0) {
        perror("flock LOCK_EX");
        close(fd);
        return 1;
    }

    make_timestamp(timestamp, sizeof(timestamp));
    len = snprintf(line, sizeof(line), "[PID:%ld] [%s] [INFO] %s\n",
                   (long)getpid(), timestamp, argv[1]);

    if (len < 0 || (size_t)len >= sizeof(line)) {
        fprintf(stderr, "Log message is too long.\n");
        flock(fd, LOCK_UN);
        close(fd);
        return 1;
    }

    if (write_full(fd, line, (size_t)len) != len) {
        perror("write");
        flock(fd, LOCK_UN);
        close(fd);
        return 1;
    }

    if (flock(fd, LOCK_UN) < 0) {
        perror("flock LOCK_UN");
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}
