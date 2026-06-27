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
#include <string.h>

#define LOG_FILE "system.log"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"message text\"\n", argv[0]);
        return 1;
    }

    int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) {
        perror("Error opening log file");
        return 1;
    }

    // Setup fcntl lock structure for blocking write lock
    struct flock fl;
    fl.l_type = F_WRLCK;    // Write lock (exclusive)
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;           // 0 means lock until end-of-file (whole file)

    // Acquire lock using F_SETLKW (blocking write lock)
    if (fcntl(fd, F_SETLKW, &fl) < 0) {
        perror("fcntl failed to acquire lock");
        close(fd);
        return 1;
    }

    // Format timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

    // Prepare log line
    char log_buf[512];
    snprintf(log_buf, sizeof(log_buf), "[PID:%d] [%s] [INFO] %s\n", getpid(), time_str, argv[1]);

    // Write log line
    if (write(fd, log_buf, strlen(log_buf)) < 0) {
        perror("Write failed");
    }

    // Release lock
    fl.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &fl) < 0) {
        perror("fcntl failed to release lock");
    }

    close(fd);
    return 0;
}