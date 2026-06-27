#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(void) {
    printf("=== Transaction Logger Simulator ===\n");
    printf("PID = %d (Bấm Ctrl+C trong vùng [SAFE] để test pending, vùng [IDLE] để tắt máy)\n\n", getpid());

    // 1. Khởi tạo sigset_t chứa SIGINT (Ctrl+C)
    sigset_t block_set, old_set;
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);

    // 2. Vòng lặp 5 lần theo đúng yêu cầu của đề bài
    for (int i = 1; i <= 5; i++) {
        
        /* ---------------------------------------------------------
         * GIAI ĐOẠN 1 — "Vùng an toàn" (Critical Section)
         * --------------------------------------------------------- */
        // a. Block SIGINT và CHỤP ẢNH trạng thái cũ cất vào old_set
        if (sigprocmask(SIG_BLOCK, &block_set, &old_set) < 0) {
            perror("sigprocmask BLOCK failed");
            return EXIT_FAILURE;
        }

        // b. In log "SAFE" và giả lập xử lý giao dịch mất 3 giây
        printf("[SAFE] Writing transaction #%d ...\n", i);
        fflush(stdout);
        sleep(3);

        printf("[SAFE] Transaction #%d committed.\n", i);
        fflush(stdout);

        // c. RESTORE (Khôi phục) lại đúng trạng thái ban đầu từ bức ảnh old_set
        // Nếu trong 3 giây qua mày bấm Ctrl+C, chương trình sẽ SẬP NGAY TẠI DÒNG NÀY!
        if (sigprocmask(SIG_SETMASK, &old_set, NULL) < 0) {
            perror("sigprocmask SETMASK failed");
            return EXIT_FAILURE;
        }

        /* ---------------------------------------------------------
         * GIAI ĐOẠN 2 — "Vùng nghỉ" (Idle Zone)
         * --------------------------------------------------------- */
        // d. In log "IDLE" và ngủ 3 giây — Giai đoạn này SIGINT không bị block
        printf("[IDLE] Waiting for next transaction...\n\n");
        fflush(stdout);
        sleep(3); 
    }

    printf("Successfully processed all transactions. Exiting...\n");
    return EXIT_SUCCESS;
}