#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main() {
    // Khởi tạo sigset_t chứa SIGINT theo hướng dẫn hints
    sigset_t block_set, old_set;
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);

    // Vòng lặp chạy liên tục 5 lần
    for (int i = 1; i <= 5; i++) {
        
        // --- GIAI ĐOẠN 1: "Vùng an toàn" (SAFE) ---
        // Thêm SIGINT vào mask hiện tại và lưu trạng thái cũ vào old_set
        sigprocmask(SIG_BLOCK, &block_set, &old_set);

        printf("[SAFE] Writing transaction #%d ...\n", i);
        fflush(stdout);

        // Giả lập ghi log mất thời gian bằng sleep(3)
        sleep(3);

        printf("[SAFE] Transaction #%d committed.\n", i);
        fflush(stdout);


        // --- GIAI ĐOẠN 2: "Vùng nghỉ" (IDLE) ---
        printf("[IDLE] Waiting for next transaction...\n");
        fflush(stdout);

        // Khôi phục lại trạng thái mask cũ sử dụng SIG_SETMASK theo đúng yêu cầu bài toán
        sigprocmask(SIG_SETMASK, &old_set, NULL);

        // Trong giai đoạn này SIGINT không bị block, nếu bấm Ctrl+C sẽ dính default action (thoát ngay)
        sleep(3);
    }

    // Kết thúc chương trình bình thường sau 5 lần lặp thành công
    return 0;
}