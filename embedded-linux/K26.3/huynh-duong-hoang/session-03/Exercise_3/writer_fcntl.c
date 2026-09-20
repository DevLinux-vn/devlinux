#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define LOG_FILE "system.log"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <message text>\n", argv[0]);
        return 1;
    }

    int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) {
        perror("open failed");
        return 1;
    }

    struct flock fl = {
        .l_type   = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start  = 0,
        .l_len    = 0,
    };

    if (fcntl(fd, F_SETLKW, &fl) < 0) {
        perror("fcntl F_SETLKW failed");
        close(fd);
        return 1;
    }

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

    char buffer[1024];
    int len = snprintf(buffer, sizeof(buffer), "[PID:%d] [%s] [INFO] %s\n",
                       getpid(), time_str, argv[1]);

    if (len > 0) {
        if (write(fd, buffer, len) != len) {
            perror("write log failed");
        }
    }

    fl.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &fl) < 0) {
        perror("fcntl F_SETLK unlock failed");
    }

    close(fd);
    return 0;
}
