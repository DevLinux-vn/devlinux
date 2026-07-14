#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/* Biến cờ toàn cục đảm bảo tính nguyên tử khi nhận tín hiệu bất đồng bộ */
volatile sig_atomic_t g_keep_running = 1;

/**
 * @brief Hàm xử lý khi hệ thống gửi tín hiệu kết thúc (SIGTERM)
 */
static void handle_sigterm(int signum) {
    (void)signum;
    g_keep_running = 0; 
}

int main(void) {
    /* Tắt bộ đệm để log xuất hiện ngay lập tức trên systemd journal */
    setbuf(stdout, NULL);

    /* Cấu hình bắt tín hiệu SIGTERM bằng sigaction chuẩn POSIX */
    struct sigaction action;
    action.sa_handler = handle_sigterm;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGTERM, &action, NULL) < 0) {
        perror("CRITICAL: Failed to register SIGTERM handler");
        return EXIT_FAILURE;
    }

    while (g_keep_running) {
        printf("Monitor service is running normally...\n");
        sleep(1);
    }

    /* Giải phóng và kết thúc an toàn (Graceful Shutdown) */
    printf("Service shutting down...\n");
    return EXIT_SUCCESS;
}