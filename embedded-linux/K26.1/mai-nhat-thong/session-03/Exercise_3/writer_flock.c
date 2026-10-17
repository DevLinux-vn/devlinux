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
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/file.h>
#include <string.h>

#define LOG_FILE "system.log"
#define LOG_BUFFER_SIZE 512
#define SIMULATED_DELAY_US 50000

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    if (flock(fd, LOCK_EX) < 0) {
        perror("flock LOCK_EX");
        (void)close(fd);
        return EXIT_FAILURE;
    }

    time_t rawtime;
    struct tm *timeinfo;
    char time_buffer[20];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    if (timeinfo == NULL) {
        perror("localtime");
        (void)flock(fd, LOCK_UN);
        (void)close(fd);
        return EXIT_FAILURE;
    }
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    char log_buffer[LOG_BUFFER_SIZE];
    int len = snprintf(log_buffer, sizeof(log_buffer), "[PID:%d] [%s] [%s] %s\n",
                       getpid(), time_buffer, "INFO", argv[1]);

    usleep(SIMULATED_DELAY_US);

    if (write(fd, log_buffer, len) != len) {
        perror("write");
    }

    /* Đảm bảo dữ liệu ghi trực tiếp xuống đĩa lưu trữ vật lý trước khi nhả khóa */
    if (fsync(fd) < 0) {
        perror("fsync");
    }

    if (flock(fd, LOCK_UN) < 0) {
        perror("flock LOCK_UN failed");
    }

    if (close(fd) < 0) {
        perror("close fd failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}