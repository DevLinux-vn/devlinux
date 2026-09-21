#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int fd;
    char time_buf[64];
    char log_buf[512];
    time_t now;
    struct tm tm_info;

    if (argc != 2) {
        printf("Usage: %s \"message text\"\n", argv[0]);
        return 1;
    }

    /* Mở hoặc tạo file log */
    fd = open("system.log",
              O_WRONLY | O_APPEND | O_CREAT,
              0644);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    /* Cấu hình khóa */
    struct flock fl = {
        .l_type   = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start  = 0,
        .l_len    = 0
    };

    /* Khóa file - chờ nếu process khác đang giữ lock */
    if (fcntl(fd, F_SETLKW, &fl) == -1) {
        perror("fcntl lock");
        close(fd);
        return 1;
    }

    /* Lấy thời gian hiện tại */
    time(&now);
    localtime_r(&now, &tm_info);

    strftime(time_buf,
             sizeof(time_buf),
             "%Y-%m-%d %H:%M:%S",
             &tm_info);

    /* Tạo dòng log */
    int len = snprintf(log_buf,
                       sizeof(log_buf),
                       "[PID:%d] [%s] [INFO] %s\n",
                       getpid(),
                       time_buf,
                       argv[1]);

    /* Ghi log */
    if (write(fd, log_buf, len) != len) {
        perror("write");
    }

    /* Mở khóa */
    fl.l_type = F_UNLCK;

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("fcntl unlock");
    }

    close(fd);

    return 0;
}