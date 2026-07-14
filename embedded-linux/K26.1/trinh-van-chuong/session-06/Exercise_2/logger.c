#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Định nghĩa các mức độ log theo chuẩn hệ thống (sd-daemon)
#define LOG_ERR     "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO    "<6>"

int main(void) {
    // 1. Tắt hoàn toàn bộ nhớ đệm cho cả stdout và stderr ngay khi startup
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    int cycle = 1;
    int total_time = 0;

    // 2. Vòng lặp chạy và bắn log cứ mỗi 2 giây
    while (total_time < 30) {
        // Ghi đồng thời 3 dòng log với 3 cấp độ khác nhau vào stderr
        fprintf(stderr, LOG_INFO    "Service running normally, cycle %d\n", cycle);
        fprintf(stderr, LOG_WARNING "Memory usage high: %d%%\n", 80 + rand() % 15);
        fprintf(stderr, LOG_ERR     "Failed to connect to database, retry %d\n", cycle);

        sleep(2);
        cycle++;
        total_time += 2; // Cộng dồn thời gian đã chạy
    }

    // 3. Sau đúng 30 giây, gọi hàm abort() để giả lập lỗi sập hệ thống (crash)
    fprintf(stderr, LOG_ERR "Critical error encountered! Simulating a crash now...\n");
    abort();

    return 0;
}