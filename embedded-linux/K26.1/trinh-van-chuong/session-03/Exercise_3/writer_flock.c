/*
| Đặc tính / Thuộc tính           | flock                                  | fcntl                                          |
|---------------------------------|----------------------------------------|------------------------------------------------|
| Phạm vi khóa (Granularity)       | Chỉ khóa toàn bộ file                  | Hỗ trợ khóa linh hoạt theo dải byte (Range)    |
| Hoạt động qua mạng (NFS)        | Không hỗ trợ ổn định trên mạng NFS     | Hỗ trợ tốt qua môi trường mạng (NFS)           |
| Kế thừa qua lệnh fork()         | Có (Tiến trình con sẽ nhận bản sao)     | Không (Khóa gắn liền với tiến trình gọi lệnh)  |
| Tự động giải phóng khi crash    | Có (Khi tiến trình chết, khóa tự nhả)  | Có (Khi tiến trình chết, khóa tự nhả)          |
| Trường hợp sử dụng tốt nhất     | Khóa file cục bộ đơn giản, ghi log     | Hệ thống file mạng, Database cần khóa phân vùng|
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/file.h> // Bắt buộc phải có để dùng flock

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Cách dùng: %s \"nội dung tin nhắn\"\n", argv[0]);
        return 1;
    }

    // 1. Mở file log ở chế độ Chỉ Ghi, Ghi nối đuôi, Tạo mới nếu chưa có
    int fd = open("system.log", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // 2. Chiếm khóa độc quyền (Exclusive Lock) - Tiến trình sẽ ĐỢI ở đây nếu file đang bị khóa
    if (flock(fd, LOCK_EX) < 0) {
        perror("flock");
        close(fd);
        return 1;
    }

    // --- BẮT ĐẦU VÙNG TRANH CHẤP (CRITICAL SECTION) ---
    
    // Lấy thời gian hiện tại của hệ thống
    time_t rawtime;
    struct tm *timeinfo;
    char time_str[20];
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    // Định dạng thời gian: YYYY-MM-DD HH:MM:SS
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Định dạng chuỗi log hoàn chỉnh bao gồm PID + Thời gian + Tin nhắn
    char log_buffer[512];
    snprintf(log_buffer, sizeof(log_buffer), "[PID:%d] [%s] [INFO] %s\n", getpid(), time_str, argv[1]);

    // Ghi dữ liệu vào file log
    if (write(fd, log_buffer, strlen(log_buffer)) < 0) {
        perror("write");
    }

    // Giả lập xử lý một chút để thấy rõ cơ chế khóa chặn các tiến trình khác
    usleep(10000); 

    // --- KẾT THÚC VÙNG TRANH CHẤP ---

    // 3. Giải phóng khóa (Unlock)
    flock(fd, LOCK_UN);

    // 4. Đóng file descriptor
    close(fd);
    return 0;
}