/*
 * fcntl vs flock comparison:
 * | Property | flock | fcntl |
 * |----------|-------|-------|
 * | Lock granularity | Whole file only | Byte range supported |
 * | Works over NFS | No | Yes |
 * | Inherited across fork | Yes | No |
 * | Best used when | Simple local file locking | Network FS or byte-range locking |
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

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

    struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = 0;

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("fcntl lock");
        close(fd);
        return 1;
    }

    time_t current_time = time(NULL);
    struct tm *time_info = localtime(&current_time);

    char timestamp[32];

    strftime(timestamp,
             sizeof(timestamp),
             "%Y-%m-%d %H:%M:%S",
             time_info);

    char log_line[1024];

    int length = snprintf(log_line,
                          sizeof(log_line),
                          "[PID:%d] [%s] [INFO] %s\n",
                          getpid(),
                          timestamp,
                          argv[1]);

    ssize_t written = write(fd, log_line, length);
    if (written != length) {
        perror("write");
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        close(fd);
        return 1;
    }

    lock.l_type = F_UNLCK;

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("fcntl unlock");
        close(fd);
        return 1;
    }

    close(fd);

    return 0;
}