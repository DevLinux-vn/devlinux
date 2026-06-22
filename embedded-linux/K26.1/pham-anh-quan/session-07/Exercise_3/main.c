#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main() {
    sigset_t block_set, old_set;

    // 1. Khởi tạo tập mask chứa tín hiệu SIGINT
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);

    printf("Transaction Logger started. Chạy mô phỏng lặp 5 lần...\n\n");

    // Vòng lặp chạy 5 lần theo đúng yêu cầu đề bài
    for (int i = 1; i <= 5; i++) {
        // =======================================================
        // GIAI ĐOẠN 1 — "Vùng an toàn" (Critical section)
        // =======================================================
        // a. Block signal SIGINT, lưu trạng thái cũ vào old_set
        sigprocmask(SIG_BLOCK, &block_set, &old_set);

        // b. In log bắt đầu và giả lập xử lý ghi log mất 3 giây
        printf("[SAFE] Writing transaction #%d ...\n", i);
        fflush(stdout);
        sleep(3);

        // c. In log hoàn thành
        printf("[SAFE] Transaction #%d committed.\n", i);
        fflush(stdout);

        // d. Sử dụng SIG_SETMASK để khôi phục (restore) lại trạng thái mask ban đầu
        sigprocmask(SIG_SETMASK, &old_set, NULL);

        // =======================================================
        // GIAI ĐOẠN 2 — "Vùng nghỉ" (Idle)
        // =======================================================
        // Giai đoạn này SIGINT không bị block nữa
        printf("[IDLE] Waiting for next transaction...\n");
        fflush(stdout);
        sleep(3);
    }

    // Kết thúc chương trình bình thường sau 5 vòng lặp thành công
    printf("\n[INFO] Toàn bộ 5 giao dịch đã được xử lý an toàn. Thoát chương trình với code 0.\n");
    return 0;
}