#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Cách dùng: %s \"nội dung tin nhắn\"\n", argv[0]);
        return 1;
    }

    // 1. Mở file log tương tự bài flock
    int fd = open("system.log", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // Cấu hình struct flock để khóa toàn bộ file
    struct flock fl;
    fl.l_type   = F_WRLCK;    // Khóa ghi (Khóa độc quyền)
    fl.l_whence = SEEK_SET;   // Điểm mốc: đầu file
    fl.l_start  = 0;          // Bắt đầu từ byte 0
    fl.l_len    = 0;          // l_len = 0 mang ý nghĩa đặc biệt: Khóa toàn bộ file cho đến vô cùng

    // 2. Chiếm khóa độc quyền sử dụng fcntl (chế độ chặn F_SETLKW)
    if (fcntl(fd, F_SETLKW, &fl) == -1) {
        perror("fcntl - lock");
        close(fd);
        return 1;
    }

    // --- BẮT ĐẦU VÙNG TRANH CHẤP (CRITICAL SECTION) ---
    time_t rawtime;
    struct tm *timeinfo;
    char time_str[20];
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

    char log_buffer[512];
    snprintf(log_buffer, sizeof(log_buffer), "[PID:%d] [%s] [INFO] %s\n", getpid(), time_str, argv[1]);

    if (write(fd, log_buffer, strlen(log_buffer)) < 0) {
        perror("write");
    }

    usleep(10000); 
    // --- KẾT THÚC VÙNG TRANH CHẤP ---

    // 3. Giải phóng khóa bằng cách đổi l_type sang F_UNLCK
    fl.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("fcntl - unlock");
    }

    close(fd);
    return 0;
}