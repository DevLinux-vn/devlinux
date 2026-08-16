/*
 * Comparison: flock vs fcntl
 *
 * Property             | flock                  | fcntl
 * ----------------------|------------------------|----------------------------
 * Lock granularity      | Whole file only        | Byte range supported
 * Works over NFS        | No                     | Yes
 * Inherited across fork | Yes                    | No
 * Best used when        | Simple local file lock | Network FS or byte-range lock
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/file.h>

#define LOG_FILE "system.log"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s \"message text\"\n", argv[0]);
        return 1;
    }

    int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    /* Acquire exclusive lock on the whole file. Blocks until available. */
    if (flock(fd, LOCK_EX) == -1) {
        perror("flock");
        close(fd);
        return 1;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timebuf[32];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", t);

    char line[512];
    int len = snprintf(line, sizeof(line), "[PID:%d] [%s] [INFO] %s\n",
                        getpid(), timebuf, argv[1]);

    if (write(fd, line, (size_t)len) != len) {
        perror("write");
    }

    /* Release the lock before closing. */
    if (flock(fd, LOCK_UN) == -1) {
        perror("flock unlock");
    }

    close(fd);
    return 0;
}
