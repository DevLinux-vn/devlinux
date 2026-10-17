/*
| Property | flock | fcntl |
|---|---|---|
| Lock granularity | Whole file only | Byte range supported |
| Works over NFS | No | Yes |
| Inherited across fork | Yes | No |
| Best used when | Simple local file locking | Network FS or byte-range locking |
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/file.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s \"message text\"\n", argv[0]);
        return 1;
    }

    int fd = open("system.log", O_WRONLY | O_APPEND | O_CREAT, 0666);
    if (fd == -1) {
        perror("Failed to open system.log");
        return 1;
    }

    // Prepare time string
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[32];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);

    // Format log line
    char log_buf[512];
    int len = snprintf(log_buf, sizeof(log_buf), "[PID:%d] [%s] [INFO] %s\n", getpid(), time_buf, argv[1]);

    // Acquire exclusive lock using flock
    if (flock(fd, LOCK_EX) == -1) {
        perror("Failed to lock file");
        close(fd);
        return 1;
    }

    // Write to file
    write(fd, log_buf, len);

    // Release lock
    flock(fd, LOCK_UN);
    close(fd);

    return 0;
}