/*
| Property                        | `flock`                      | `fcntl`                         |
|---------------------------------|------------------------------|---------------------------------|
| Lock granularity                | Whole file only              | Byte range supported            |
| Works over NFS                  | No                           | Yes                             |
| Inherited across `fork`         | Yes (child gets a copy)      | No (per open-file-description)  |
| Automatically released on crash | Yes                          | Yes                             |
| Best used when                  | Simple local file locking    | Network FS or byte-range locking|
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Cách sử dụng: %s \"nội dung_log\"\n", argv[0]);
        return 1;
    }

    // 1. Mở file log
    int fd = open("system.log", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) {
        perror("Lỗi mở file log");
        return 1;
    }

    // Cấu hình struct flock để khóa toàn bộ file
    struct flock fl;
    memset(&fl, 0, sizeof(fl));
    fl.l_type   = F_WRLCK;    // Khóa ghi độc quyền (Write Lock)
    fl.l_whence = SEEK_SET;   // Tính từ đầu file
    fl.l_start  = 0;          // Bắt đầu từ byte thứ 0
    fl.l_len    = 0;          // 0 có nghĩa là khóa toàn bộ file cho đến vô tận (EOF)

    // 2. Chiếm khóa bằng chế độ F_SETLKW
    if (fcntl(fd, F_SETLKW, &fl) < 0) {
        perror("Lỗi chiếm khóa fcntl");
        close(fd);
        return 1;
    }

    // BẮT ĐẦU VÙNG GĂNG (CRITICAL SECTION)
    time_t rawtime;
    struct tm *timeinfo;
    char time_str[20];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

    pid_t pid = getpid();

    char log_buffer[1024];
    snprintf(log_buffer, sizeof(log_buffer), "[PID:%d] [%s] [INFO] %s\n", pid, time_str, argv[1]);

    write(fd, log_buffer, strlen(log_buffer));
    // KẾT THÚC VÙNG GĂNG

    // 3. Giải phóng khóa (Cấu hình lại l_type thành F_UNLCK)
    fl.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &fl) < 0) {
        perror("Lỗi mở khóa fcntl");
    }

    close(fd);
    return 0;
}