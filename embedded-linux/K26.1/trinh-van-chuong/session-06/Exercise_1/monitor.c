#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Biến cờ để kiểm soát vòng lặp
volatile sig_atomic_t keep_running = 1;

// Hàm xử lý khi nhận được tín hiệu SIGTERM từ systemd
void handle_sigterm(int signum) {
    // In dòng chữ theo đúng yêu cầu đề bài ra stdout (sẽ nhảy vào journal)
    printf("Service shutting down...\n");
    keep_running = 0;
}

int main(void) {
    // 1. Gọi setbuf tại startup để log xuất hiện trong journal ngay lập tức
    setbuf(stdout, NULL);

    // 2. Đăng ký hàm xử lý tín hiệu SIGTERM
    struct sigaction action;
    action.sa_handler = handle_sigterm;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGTERM, &action, NULL);

    printf("Monitor service started successfully.\n");

    // 3. Vòng lặp vô hạn chạy mỗi 1 giây
    while (keep_running) {
        printf("Monitor checking status... (running cleanly)\n");
        sleep(1); 
    }

    // Thoát an toàn
    return 0;
}