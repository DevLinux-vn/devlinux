/*
 * Comparison table:
 *
 * | Property           | flock                    | fcntl                       |
 * |--------------------|--------------------------|-----------------------------|
 * | Lock granularity   | Whole file only          | Byte range supported        |
 * | Works over NFS     | No                       | Yes                         |
 * | Inherited across   | Yes                      | No                          |
 * | fork               |                          |                             |
 * | Best used when     | Simple local file        | Network FS or byte-range    |
 * |                    | locking                  | locking                     |
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#define LOG_FILE "system.log"
#define LOG_BUFFER_SIZE 1024


int main(int argc, char *argv[])
{
    int fd;
    char buffer[LOG_BUFFER_SIZE];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s \"message text\"\n", argv[0]);
        return EXIT_FAILURE;
    }

    /*
     * Open the log file.
     */
    fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);

    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    /*
     * Configure an exclusive write lock.
     *
     * l_start = 0
     * l_len   = 0
     *
     * means the lock covers the entire file.
     */
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_pid = 0;

    /*
     * F_SETLKW = blocking lock.
     *
     * If another process owns the lock,
     * this call waits until the lock becomes available.
     */
    if (fcntl(fd, F_SETLKW, &fl) == -1) {
        perror("fcntl F_SETLKW");
        close(fd);
        return EXIT_FAILURE;
    }

    /*
     * Get current time.
     */
    time_t now = time(NULL);
    struct tm tm_info;

    if (localtime_r(&now, &tm_info) == NULL) {
        perror("localtime_r");

        fl.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &fl);

        close(fd);
        return EXIT_FAILURE;
    }

    /*
     * Format log line.
     */
    int length = snprintf(
        buffer,
        sizeof(buffer),
        "[PID:%d] [%04d-%02d-%02d %02d:%02d:%02d] [INFO] %s\n",
        getpid(),
        tm_info.tm_year + 1900,
        tm_info.tm_mon + 1,
        tm_info.tm_mday,
        tm_info.tm_hour,
        tm_info.tm_min,
        tm_info.tm_sec,
        argv[1]
    );

    if (length < 0 || length >= (int)sizeof(buffer)) {
        fprintf(stderr, "Message is too long.\n");

        fl.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &fl);

        close(fd);
        return EXIT_FAILURE;
    }

    /*
     * Write while holding the lock.
     */
    ssize_t written = write(fd, buffer, length);

    if (written != length) {
        perror("write");

        fl.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &fl);

        close(fd);
        return EXIT_FAILURE;
    }

    /*
     * Release the lock.
     */
    fl.l_type = F_UNLCK;

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("fcntl F_SETLK unlock");
        close(fd);
        return EXIT_FAILURE;
    }

    close(fd);

    return EXIT_SUCCESS;
}
