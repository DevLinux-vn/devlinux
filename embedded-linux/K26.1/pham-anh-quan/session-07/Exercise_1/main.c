#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Khai báo biến đếm toàn cục dạng volatile sig_atomic_t theo gợi ý đề bài
// giúp đảm bảo an toàn khi biến này bị ghi/đọc đồng thời giữa vòng lặp chính và tín hiệu handler
volatile sig_atomic_t reading_count = 0;

// 1. Handler xử lý tín hiệu SIGINT (Ctrl+C)
void handle_sigint(int sig) {
    // Không cho thoát ngay, chỉ in cảnh báo
    printf("\n[WARN] Received SIGINT, ignoring...\n");
    fflush(stdout); // Đảm bảo dòng chữ được in ra màn hình ngay lập tức
}

// 2. Handler xử lý tín hiệu SIGTERM (Tắt máy đúng cách / Lệnh kill mặc định)
void handle_sigterm(int sig) {
    printf("\n[INFO] Received SIGTERM, shutting down gracefully...\n");
    fflush(stdout);
    exit(0); // Kết thúc chương trình thành công với mã lỗi 0
}

// 3. Handler xử lý tín hiệu SIGUSR1 (In báo cáo trạng thái giả lập)
void handle_sigusr1(int sig) {
    printf("\n[REPORT] Total readings so far: %d\n", reading_count);
    fflush(stdout);
}

int main() {
    // Đăng ký 3 hàm xử lý (handler) tương ứng với 3 loại tín hiệu thông qua hàm signal()
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigterm);
    signal(SIGUSR1, handle_sigusr1);

    int temperature = 25; // Khởi tạo giá trị nhiệt độ giả lập

    // Vòng lặp vô hạn mô phỏng một sensor daemon chạy nền
    while(1) {
        reading_count++; // Tăng số lần đọc cảm biến
        
        // In ra dòng log định dạng chuẩn: [INFO] Sensor reading #<n>: temperature=<giá_trị>
        printf("[INFO] Sensor reading #%d: temperature=%d\n", reading_count, temperature);
        fflush(stdout);

        // Giả lập nhiệt độ biến động nhẹ cho thực tế (tăng dần hoặc random nhẹ)
        temperature += (reading_count % 2 == 0) ? 1 : -1;

        sleep(1); // Dừng 1 giây trước khi lặp lại theo yêu cầu đề bài
    }

    return 0;
}