
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

// Biến đếm số lần đọc, dùng volatile sig_atomic_t theo đúng Design Hints để an toàn khi dùng với signal handler
volatile sig_atomic_t reading_count = 0;

// Handler xử lý khi nhận SIGINT (Ctrl+C)
void handle_sigint(int sig) {
    // Tránh dùng printf trực tiếp trong handler thực tế vì không an toàn (reentrant), nhưng bài tập yêu cầu in log nên ta xuất ra chuẩn quy định
    printf("\n[WARN] Received SIGINT, ignoring...\n");
    fflush(stdout);
}

// Handler xử lý khi nhận SIGTERM (Tắt máy/Tắt tiến trình đúng cách)
void handle_sigterm(int sig) {
    printf("\n[INFO] Received SIGTERM, shutting down gracefully...\n");
    fflush(stdout);
    exit(0); // Kết thúc với exit code 0
}

// Handler xử lý khi nhận SIGUSR1 (In báo cáo trạng thái)
void handle_sigusr1(int sig) {
    printf("\n[REPORT] Total readings so far: %d\n", reading_count);
    fflush(stdout);
}

int main() {
    // Khởi tạo bộ sinh số ngẫu nhiên cho nhiệt độ giả lập
    srand(time(NULL));

    // 1. Đăng ký các signal handler thay đổi default action
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigterm);
    signal(SIGUSR1, handle_sigusr1);

    // In ra PID của tiến trình để ông dễ test gửi lệnh kill từ terminal khác
    printf("Sensor daemon started. PID: %d\n", getpid());
    printf("Mỗi giây sẽ in ra một dòng log nhiệt độ...\n\n");

    // 2. Main loop: Vòng lặp vô hạn mô phỏng sensor daemon
    while (1) {
        reading_count++;
        
        // Giả lập giá trị nhiệt độ ngẫu nhiên từ 25.0 đến 39.9 độ C
        float current_temp = 25.0 + (rand() % 150) / 10.0;

        // In ra dòng log định dạng chuẩn: [INFO] Sensor reading #<n>: temperature=<giá_tri>
        printf("[INFO] Sensor reading #%d: temperature=%.1f\n", reading_count, current_temp);
        fflush(stdout); // Đảm bảo log được đẩy ra ngay lập tức

        sleep(1); // Nghỉ 1 giây
    }

    return 0;
}
