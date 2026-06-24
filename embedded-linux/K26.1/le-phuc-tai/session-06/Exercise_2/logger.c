#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

/* Tiền tố định dạng log level tích hợp thẳng với systemd journald */
#define LOG_ERR     "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO    "<6>"

#define RUN_DURATION_SEC 30
#define INTERVAL_SEC     2

int main(void) {
    /* Ép cả 2 luồng xuất không sử dụng bộ đệm (Phòng thủ mất log khi crash) */
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    srand(time(NULL));
    int elapsed_time = 0;
    int cycle = 1;

    while (elapsed_time < RUN_DURATION_SEC) {
        fprintf(stderr, LOG_INFO    "Service running normally, cycle %d\n", cycle);
        fprintf(stderr, LOG_WARNING "Memory usage high: %d%%\n", 80 + rand() % 15);
        fprintf(stderr, LOG_ERR     "Failed to connect to database, retry %d\n", cycle);

        sleep(INTERVAL_SEC);
        elapsed_time += INTERVAL_SEC;
        cycle++;
    }

    /* Ghi log cảnh báo cuối cùng trước khi kích hoạt ngắt phần cứng hủy tiến trình */
    fprintf(stderr, LOG_ERR "Critical internal error encountered. Triggering abort()...\n");
    
    /* Gửi tín hiệu SIGABRT để tạo core dump và crash chủ động theo đặc tả thiết kế */
    abort(); 

    return EXIT_SUCCESS; /* Không bao giờ đạt tới (Unreachable code) */
}