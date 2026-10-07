#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// Định nghĩa các mức độ log theo chuẩn systemd
#define LOG_ERR     "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO    "<6>"

int main() {
    // Tắt bộ đệm cho cả stdout và stderr để log được ghi ngay lập tức
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    int cycle = 1;
    int max_cycles = 15; // 15 vòng * 2 giây = 30 giây

    // Cứ 2 giây in ra 3 dòng log với 3 cấp độ khác nhau
    while (cycle <= max_cycles) {
        fprintf(stderr, LOG_INFO    "Service running normally, cycle %d\n", cycle);
        fprintf(stderr, LOG_WARNING "Memory usage high: %d%%\n", 80 + rand() % 15);
        fprintf(stderr, LOG_ERR     "Failed to connect to database, retry %d\n", cycle);
        
        sleep(2);
        cycle++;
    }

    // Sau 30 giây, gọi abort() để giả lập crash
    fprintf(stderr, LOG_ERR "Simulating fatal crash now!\n");
    abort();

    return 0;
}