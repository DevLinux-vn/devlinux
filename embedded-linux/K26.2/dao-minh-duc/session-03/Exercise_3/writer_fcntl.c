/*
 * Comparison:
 *
 * | Property               | flock                         | fcntl                    |
 * |------------------------|-------------------------------|--------------------------|
 * | Lock granularity       | Whole file only               | Byte range supported     |
 * | Works over NFS         | No                            | Yes                      |
 * | Inherited across fork  | Yes                           | No                       |
 * | Best used when         | Simple local file locking     | Network FS or            |
 * |                        |                               | byte-range locking       |
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define LOG_FILE "system.log"
#define LOG_BUFFER_SIZE 1024

static int lock_file(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_pid = 0;

    return fcntl(fd, F_SETLKW, &fl);
}

static int unlock_file(int fd)
{
    struct flock fl;

    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_pid = 0;

    return fcntl(fd, F_SETLK, &fl);
}

int main(int argc, char *argv[])
{
    int fd;
    char log_buffer[LOG_BUFFER_SIZE];
    time_t now;
    struct tm tm_info;
    ssize_t bytes_written;
    int len;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s \"message text\"\n", argv[0]);
        return EXIT_FAILURE;
    }

    fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    if (lock_file(fd) == -1) {
        perror("fcntl F_SETLKW");
        close(fd);
        return EXIT_FAILURE;
    }

    now = time(NULL);
    if (now == (time_t)-1) {
        perror("time");
        unlock_file(fd);
        close(fd);
        return EXIT_FAILURE;
    }

    if (localtime_r(&now, &tm_info) == NULL) {
        perror("localtime_r");
        unlock_file(fd);
        close(fd);
        return EXIT_FAILURE;
    }

    len = snprintf(
        log_buffer,
        sizeof(log_buffer),
        "[PID:%ld] [%04d-%02d-%02d %02d:%02d:%02d] [INFO] %s\n",
        (long)getpid(),
        tm_info.tm_year + 1900,
        tm_info.tm_mon + 1,
        tm_info.tm_mday,
        tm_info.tm_hour,
        tm_info.tm_min,
        tm_info.tm_sec,
        argv[1]
    );

    if (len < 0 || len >= (int)sizeof(log_buffer)) {
        fprintf(stderr, "Message is too long\n");
        unlock_file(fd);
        close(fd);
        return EXIT_FAILURE;
    }

    bytes_written = write(fd, log_buffer, len);
    if (bytes_written == -1) {
        perror("write");
        unlock_file(fd);
        close(fd);
        return EXIT_FAILURE;
    }

    if (bytes_written != len) {
        fprintf(stderr, "Partial write\n");
        unlock_file(fd);
        close(fd);
        return EXIT_FAILURE;
    }

    if (unlock_file(fd) == -1) {
        perror("fcntl F_SETLK");
        close(fd);
        return EXIT_FAILURE;
    }

    if (close(fd) == -1) {
        perror("close");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
