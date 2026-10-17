/*
| Property | flock |
|---|---|---|
| Lock granularity | Whole file only |
| Works over NFS | No |
| Inherited across fork | Yes (child gets a copy) |
| Automatically released on crash | Yes |
| Best used when | Simple local file locking |
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/file.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Cách dùng: %s <tin_nhan_log>\n", argv[0]);
        return 1;
    }

    int fd = open("system.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("Không mở được log file");
        return 1;
    }

    if (flock(fd, LOCK_EX) < 0) {
        perror("Lỗi chiếm khóa flock");
        close(fd);
        return 1;
    }

    time_t rawtime;
    struct tm *timeinfo;
    char time_str[20];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

    char log_buffer[512];
    snprintf(log_buffer, sizeof(log_buffer), "[PID:%d] [%s] [INFO] %s\n", getpid(), time_str, argv[1]);

    write(fd, log_buffer, strlen(log_buffer));
    
    //sleep(20); 

    flock(fd, LOCK_UN);

    close(fd);
    return 0;
}