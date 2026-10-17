/*
| Property | `flock` | `fcntl` |
|---|---|---|
| Lock granularity | Whole file only | Byte range supported |
| Works over NFS | No | Yes |
| Inherited across `fork` | Yes | No |
| Best used when | Simple local file locking | Network FS or byte-range locking |
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/file.h>
#include <time.h>
#include <errno.h>

#define LOG_FILE "system.log"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"message text\"\n", argv[0]);
        return 1;
    }

    int fd;
    do {
        fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    } while (fd == -1 && errno == EINTR);

    if (fd == -1) {
        perror("open failed");
        return 1;
    }

    if (flock(fd, LOCK_EX) == -1) {
        perror("flock LOCK_EX failed");
        close(fd);
        return 1;
    }

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[32];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);

    pid_t pid = getpid();
    char log_msg[512];
    int len = snprintf(log_msg, sizeof(log_msg), "[PID:%d] [%s] [INFO] %s\n", pid, time_buf, argv[1]);
    
    if (len < 0 || len >= (int)sizeof(log_msg)) {
        fprintf(stderr, "Message formatting error or truncation.\n");
        flock(fd, LOCK_UN);
        close(fd);
        return 1;
    }

    ssize_t bytes_written;
    do {
        bytes_written = write(fd, log_msg, len);
    } while (bytes_written == -1 && errno == EINTR);

    if (bytes_written != len) {
        perror("write failed");
        flock(fd, LOCK_UN);
        close(fd);
        return 1;
    }

    if (flock(fd, LOCK_UN) == -1) {
        perror("flock LOCK_UN failed");
        close(fd);
        return 1;
    }

    if (close(fd) == -1) {
        perror("close failed");
        return 1;
    }

    return 0;
}
