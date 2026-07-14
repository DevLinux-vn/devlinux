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
#include <sys/file.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Cách sử dụng: %s \"nội dung_log\"\n", argv[0]);
        return 1;
    }

    // 1. Mở file ở chế độ Chỉ Ghi, Ghi nối đuôi và Tạo mới nếu chưa có
    int fd = open("system.log", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) {
        perror("Lỗi mở file log");
        return 1;
    }

    // 2. Chiếm khóa ĐỘC QUYỀN (Exclusive Lock) - Hàm này sẽ đợi (block) cho đến khi file rảnh
    if (flock(fd, LOCK_EX) < 0) {
        perror("Lỗi chiếm khóa flock");
        close(fd);
        return 1;
    }

    // BẮT ĐẦU VÙNG GĂNG (CRITICAL SECTION) - Chỉ có duy nhất 1 tiến trình được đứng ở đây tại 1 thời điểm
    
    // Lấy thời gian hệ thống và định dạng chuỗi
    time_t rawtime;
    struct tm *timeinfo;
    char time_str[20];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Lấy PID (Process ID) của tiến trình hiện tại
    pid_t pid = getpid();

    // Định dạng chuỗi log theo chuẩn yêu cầu của đề bài
    char log_buffer[1024];
    snprintf(log_buffer, sizeof(log_buffer), "[PID:%d] [%s] [INFO] %s\n", pid, time_str, argv[1]);

    // Ghi chuỗi log xuống file
    write(fd, log_buffer, strlen(log_buffer));

    // KẾT THÚC VÙNG GĂNG

    // 3. Giải phóng khóa (Unlock)
    flock(fd, LOCK_UN);

    // 4. Đóng file descriptor
    close(fd);

    return 0;
}