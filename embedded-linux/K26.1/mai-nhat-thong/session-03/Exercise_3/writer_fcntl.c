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

    struct flock fl = {
        .l_type   = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start  = 0,
        .l_len    = 0,
    };

    if (fcntl(fd, F_SETLKW, &fl) < 0) {
        perror("fcntl F_SETLKW");
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
        fl.l_type = F_UNLCK;
        (void)fcntl(fd, F_SETLK, &fl);
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

    /* Đảm bảo toàn bộ data được ghi ổn định xuống đĩa lưu trữ (disk) */
    if (fsync(fd) < 0) {
        perror("fsync");
    }

    /* Bổ sung kiểm tra lỗi khi gọi lệnh UNLOCK */
    fl.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &fl) < 0) {
        perror("fcntl F_SETLK unlock failed");
    }

    if (close(fd) < 0) {
        perror("close fd failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}