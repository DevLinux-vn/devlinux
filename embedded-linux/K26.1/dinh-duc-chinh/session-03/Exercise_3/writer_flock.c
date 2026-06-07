/*
 * Comparison table:
 * | Property              | flock           | fcntl                |
 * | Lock granularity      | Whole file only | Byte range supported |
 * | Works over NFS        | No              | Yes                  |
 * | Inherited across fork | Yes             | No                   |
 * | Best used when        | Simple local    | Network FS / range   |
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>     // flock
#include <time.h>

#define LOG_FILE "system.log"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"message\"\n", argv[0]);
        return 1;
    }

    int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) { perror("open"); return 1; }

    if (flock(fd, LOCK_EX) < 0) {
        perror("flock");
        close(fd); return 1;
    }

    char ts[32];
    time_t now = time(NULL);
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", localtime(&now));

    char line[512];
    int len = snprintf(line, sizeof(line),
                       "[PID:%d] [%s] [INFO] %s\n",
                       getpid(), ts, argv[1]);

    if (write(fd, line, len) != len) perror("write");

    flock(fd, LOCK_UN);
    close(fd);
    return 0;
}
