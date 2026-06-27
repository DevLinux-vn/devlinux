#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

// Biến toàn cục đếm số lần đọc (Dùng volatile sig_atomic_t theo đúng gợi ý của đề bài)
volatile sig_atomic_t reading_count = 0;

/* ── 1. Handler cho SIGINT (Ctrl+C) ── */
void handle_sigint(int sig) {
    // Không dùng printf ở đây để đảm bảo an toàn tín hiệu (Async-Signal-Safe)
    const char msg[] = "\n[WARN] Received SIGINT, ignoring...\n";
    write(STDOUT_FILENO, msg, strlen(msg));
}

/* ── 2. Handler cho SIGTERM (Tắt máy đúng cách) ── */
void handle_sigterm(int sig) {
    const char msg[] = "\n[INFO] Received SIGTERM, shutting down gracefully...\n";
    write(STDOUT_FILENO, msg, strlen(msg));
    _exit(EXIT_SUCCESS); // Thoát chương trình an toàn ngay lập tức
}

/* ── 3. Handler cho SIGUSR1 (In báo cáo trạng thái) ── */
void handle_sigusr1(int sig) {
    // Vì cần in biến số n, ta dùng một mẹo nhỏ với write để chuyển số thành chữ 
    // nhằm giữ tính an toàn (hoặc có thể dùng printf nếu chấp nhận rủi ro nhẹ)
    char buffer[100];
    int len = snprintf(buffer, sizeof(buffer), "\n[REPORT] Total readings so far: %d\n", (int)reading_count);
    write(STDOUT_FILENO, buffer, len);
}

int main(void) {
    printf("=== Sensor Daemon Simulator ===\n");
    printf("PID = %d (Dùng PID này để gửi tín hiệu từ terminal khác)\n\n", getpid());

    // Đăng ký 3 signal handler thay đổi default action
    signal(SIGINT,  handle_sigint);
    signal(SIGTERM, handle_sigterm);
    signal(SIGUSR1, handle_sigusr1);

    int temperature = 25; // Giá trị nhiệt độ giả lập ban đầu

    // Vòng lặp vô hạn (Main loop)
    while (1) {
        // Tăng biến đếm số lần đọc cảm biến
        reading_count++;
        
        // Giả lập nhiệt độ biến động ngẫu nhiên quanh mức 25-30 độ
        temperature = 25 + (reading_count % 6); 

        // In dòng log trạng thái của cảm biến mỗi giây
        printf("[INFO] Sensor reading #%d: temperature=%d°C\n", (int)reading_count, temperature);
        fflush(stdout); // Ép sạch bộ đệm để log hiện ra ngay lập tức

        sleep(1); // Nghỉ 1 giây
    }

    return 0;
}