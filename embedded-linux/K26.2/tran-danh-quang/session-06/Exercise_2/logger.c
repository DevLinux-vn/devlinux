#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define LOG_ERR     "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO    "<6>"

int main(void) {
    if (setvbuf(stdout, NULL, _IONBF, 0) != 0) {
        fprintf(stderr, "setvbuf stdout failed: %s\n", strerror(errno));
        return 1;
    }
    if (setvbuf(stderr, NULL, _IONBF, 0) != 0) {
        fprintf(stderr, "setvbuf stderr failed: %s\n", strerror(errno));
        return 1;
    }

    int cycle = 0;
    time_t start_time = time(NULL);
    if (start_time == (time_t)-1) {
        fprintf(stderr, LOG_ERR "Failed to get current time\n");
        return 1;
    }
    // Chấp nhận cho bài này — time-based seed đủ cho rand() demo
    srand((unsigned int)start_time);

    while (1) {
        cycle++;
        // Return value không check vì bài không yêu cầu xử lý lỗi write
        // Trong production, nên check: if (fprintf(...) < 0) { ... }
        if (fprintf(stderr, LOG_INFO "Service running normally, cycle %d\n", cycle) < 0) { perror("fprintf"); }
        if (fprintf(stderr, LOG_WARNING "Memory usage high: %d%%\n", 80 + rand() % 15) < 0) { perror("fprintf"); }
        if (fprintf(stderr, LOG_ERR "Failed to connect to database, retry %d\n", cycle) < 0) { perror("fprintf"); }

        sleep(2);

        // Simulate crash after 30s to test systemd restart behavior
        if (time(NULL) - start_time >= 30) {
            fprintf(stderr, LOG_ERR "Simulating a crash after 30 seconds...\n");
            abort();
        }
    }

    return 0;
}
