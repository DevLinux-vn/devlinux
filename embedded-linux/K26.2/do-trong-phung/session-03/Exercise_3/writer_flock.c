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
    if (argc != 2) {
        fprintf(stderr, "Usage: %s \"message text\"\n", argv[0]);
        return EXIT_FAILURE;
    }

    int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    if (flock(fd, LOCK_EX) == -1) {
        perror("flock LOCK_EX");
        close(fd);
        return EXIT_FAILURE;
    }

    time_t now = time(NULL);
    if (now == (time_t)-1) {
        perror("time");
        flock(fd, LOCK_UN);
        close(fd);
        return EXIT_FAILURE;
    }

    struct tm local_time;
    if (localtime_r(&now, &local_time) == NULL) {
        perror("localtime_r");
        flock(fd, LOCK_UN);
        close(fd);
        return EXIT_FAILURE;
    }

    char time_buffer[32];

    if (strftime(time_buffer,
                 sizeof(time_buffer),
                 "%Y-%m-%d %H:%M:%S",
                 &local_time) == 0) {
        fprintf(stderr, "strftime failed\n");
        flock(fd, LOCK_UN);
        close(fd);
        return EXIT_FAILURE;
    }

    char log_buffer[LOG_BUFFER_SIZE];

    int len = snprintf(
        log_buffer,
        sizeof(log_buffer),
        "[PID:%ld] [%s] [INFO] %s\n",
        (long)getpid(),
        time_buffer,
        argv[1]
    );

    if (len < 0 || (size_t)len >= sizeof(log_buffer)) {
        fprintf(stderr, "Log message is too long\n");
        flock(fd, LOCK_UN);
        close(fd);
        return EXIT_FAILURE;
    }

    ssize_t written = write(fd, log_buffer, (size_t)len);

    if (written == -1) {
        perror("write");
        flock(fd, LOCK_UN);
        close(fd);
        return EXIT_FAILURE;
    }

    if (written != len) {
        fprintf(stderr, "Incomplete write\n");
        flock(fd, LOCK_UN);
        close(fd);
        return EXIT_FAILURE;
    }

    if (flock(fd, LOCK_UN) == -1) {
        perror("flock LOCK_UN");
        close(fd);
        return EXIT_FAILURE;
    }

    if (close(fd) == -1) {
        perror("close");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}