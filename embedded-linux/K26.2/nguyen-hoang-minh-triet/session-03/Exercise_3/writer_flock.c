/*
 * Comparison Table:
 *
 * | Property | `flock` | `fcntl` |
 * |---|---|---|
 * | Lock granularity | Whole file only | Byte range supported |
 * | Works over NFS | No | Yes |
 * | Inherited across `fork` | Yes | No |
 * | Best used when | Simple local file locking | Network FS or byte-range locking |
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>

#define LOG_FILE "system.log"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    /* Acquire exclusive file lock */
    if (flock(fd, LOCK_EX) == -1) {
        perror("flock LOCK_EX");
        close(fd);
        return EXIT_FAILURE;
    }

    /* Format current timestamp */
    time_t now = time(NULL);
    struct tm tm_info;
    localtime_r(&now, &tm_info);
    char time_buf[20];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &tm_info);

    /* Format and write log entry */
    char log_buf[512];
    int len = snprintf(log_buf, sizeof(log_buf), "[PID:%d] [%s] [INFO] %s\n",
                       getpid(), time_buf, argv[1]);

    if (len > 0) {
        if (write(fd, log_buf, (size_t)len) == -1) {
            perror("write");
        }
    }

    /* Release lock and close file */
    flock(fd, LOCK_UN);
    close(fd);

    return EXIT_SUCCESS;
}

