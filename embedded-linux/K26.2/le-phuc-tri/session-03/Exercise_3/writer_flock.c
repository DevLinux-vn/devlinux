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
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
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
     *
     * O_WRONLY : write only
     * O_APPEND : every write goes to the end of the file
     * O_CREAT  : create if it does not exist
     */
    fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);

    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    /*
     * Acquire exclusive lock.
     * This blocks until no other flock writer holds the lock.
     */
    if (flock(fd, LOCK_EX) == -1) {
        perror("flock LOCK_EX");
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
        flock(fd, LOCK_UN);
        close(fd);
        return EXIT_FAILURE;
    }

    /*
     * Format:
     * [PID:12345] [2025-05-21 14:02:33] [INFO] message
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

        flock(fd, LOCK_UN);
        close(fd);
        return EXIT_FAILURE;
    }

    /*
     * Write one complete log line while the lock is held.
     */
    ssize_t written = write(fd, buffer, length);

    if (written != length) {
        perror("write");
        flock(fd, LOCK_UN);
        close(fd);
        return EXIT_FAILURE;
    }

    /*
     * Release lock.
     */
    if (flock(fd, LOCK_UN) == -1) {
        perror("flock LOCK_UN");
        close(fd);
        return EXIT_FAILURE;
    }

    close(fd);

    return EXIT_SUCCESS;
}
