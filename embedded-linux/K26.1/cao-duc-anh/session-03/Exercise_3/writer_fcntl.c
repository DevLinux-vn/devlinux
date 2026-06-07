/*
 * writer_fcntl.c — append one log line to system.log using fcntl() locking.
 *
 * Comparison: flock vs fcntl
 * +-----------------------+-----------------------+----------------------------+
 * | Property              | flock                 | fcntl                      |
 * +-----------------------+-----------------------+----------------------------+
 * | Lock granularity      | Whole file only       | Byte range supported       |
 * | Works over NFS        | No                    | Yes                        |
 * | Inherited across fork | Yes                   | No                         |
 * | Best used when        | Simple local file     | Network FS or byte-range   |
 * |                       | locking               | locking                    |
 * +-----------------------+-----------------------+----------------------------+
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"message\"\n", argv[0]);
        return 1;
    }

    int fd = open("system.log", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct flock fl = {
        .l_type   = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start  = 0,
        .l_len    = 0,
    };
    fcntl(fd, F_SETLKW, &fl);

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    char line[256];
    int len = snprintf(line, sizeof(line),
                       "[PID:%-5d] [%s] [INFO] %s\n",
                       (int)getpid(), timestamp, argv[1]);

    write(fd, line, len);

    fl.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &fl);
    close(fd);
    return 0;
}
