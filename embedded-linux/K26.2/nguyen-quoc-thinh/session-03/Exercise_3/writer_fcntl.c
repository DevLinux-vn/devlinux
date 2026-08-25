/*
 * Property              | flock               | fcntl
 * ----------------------|---------------------|---------------------
 * Lock granularity      | Whole file only     | Byte range supported
 * Works over NFS        | No                  | Yes
 * Inherited across fork | Yes                 | No
 * Best used when        | Simple local file   | Network FS or
 *                        | locking             | byte-range locking
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define LOGFILE "system.log"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Cach dung: %s \"noi dung message\"\n", argv[0]);
        return 1;
    }

    int fd = open(LOGFILE, O_WRONLY | O_APPEND | O_CREAT, 0644);

    if (fd < 0) {
        perror("open system.log");
        return 1;
    }

    struct flock fl;

    memset(&fl, 0, sizeof(fl));

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;

    if (fcntl(fd, F_SETLKW, &fl) < 0) {
        perror("fcntl F_SETLKW");
        close(fd);
        return 1;
    }

    pid_t pid = getpid();

    time_t now = time(NULL);
    struct tm tm_info;

    localtime_r(&now, &tm_info);

    char time_buf[32];

    strftime(
        time_buf,
        sizeof(time_buf),
        "%Y-%m-%d %H:%M:%S",
        &tm_info
    );

    char line[512];

    int len = snprintf(
        line,
        sizeof(line),
        "[PID:%d] [%s] [INFO] %s\n",
        pid,
        time_buf,
        argv[1]
    );

    if (write(fd, line, (size_t)len) != len) {
        perror("write");
    }

    usleep(50000);

    fl.l_type = F_UNLCK;

    if (fcntl(fd, F_SETLK, &fl) < 0) {
        perror("fcntl F_SETLK unlock");
    }

    close(fd);

    return 0;
}