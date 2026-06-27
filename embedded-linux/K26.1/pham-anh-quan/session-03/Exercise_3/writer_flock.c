/*
 * Comparison Table:
 * +--------------------+-------------------------+---------------------------------+
 * | Property           | flock                   | fcntl                           |
 * +--------------------+-------------------------+---------------------------------+
 * | Lock granularity   | Whole file only         | Byte range supported            |
 * | Works over NFS     | No                      | Yes                             |
 * | Inherited across   | Yes                     | No                              |
 * | Best used when     | Simple local file lock  | Network FS or byte-range locking|
 * +--------------------+-------------------------+---------------------------------+
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/file.h>
#include <string.h>

#define LOG_FILE "system.log"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"message text\"\n", argv[0]);
        return 1;
    }

    // Open file with O_WRONLY | O_APPEND | O_CREAT, permissions: 0644
    int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) {
        perror("Error opening log file");
        return 1;
    }

    // Acquire an exclusive lock using flock
    if (flock(fd, LOCK_EX) < 0) {
        perror("flock failed to acquire lock");
        close(fd);
        return 1;
    }

    // Format timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

    // Prepare log line format: [PID:12345] [2025-05-21 14:02:33] [INFO] message
    char log_buf[512];
    snprintf(log_buf, sizeof(log_buf), "[PID:%d] [%s] [INFO] %s\n", getpid(), time_str, argv[1]);

    // Write log line
    if (write(fd, log_buf, strlen(log_buf)) < 0) {
        perror("Write failed");
    }

    // Release the lock
    flock(fd, LOCK_UN);
    close(fd);

    return 0;
}