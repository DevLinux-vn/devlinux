#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <time.h>
#include <sys/timerfd.h>

#define DEVICE_PATH "/dev/led_blink"
#define BLINK_PERIOD_SEC 5

static int dev_fd = -1;
static int timer_fd = -1;
static volatile sig_atomic_t keep_running = 1;

// Signal handler bắt Ctrl+C (SIGINT) và SIGTERM
static void handle_signal(int sig) {
    (void)sig;
    keep_running = 0;
}

// In timestamp kèm theo log
static void print_timestamp(void) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char buffer[26];
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("[%s] ", buffer);
}

// Đọc trạng thái thực tế của LED từ driver
static char read_led_status(int fd) {
    char status = '0';
    // Đưa con trỏ file về 0 trước khi đọc
    lseek(fd, 0, SEEK_SET);
    if (read(fd, &status, 1) < 0) {
        perror("[APP] Failed to read LED status");
        return -1;
    }
    return status;
}

int main(void) {
    struct sigaction sa;
    struct itimerspec its;
    uint64_t expirations;
    char current_state;
    char target_cmd;

    // 1. Cấu hình signal handler dọn dẹp khi nhận Ctrl+C
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    // 2. Mở file thiết bị /dev/led_blink với quyền O_RDWR
    dev_fd = open(DEVICE_PATH, O_RDWR);
    if (dev_fd < 0) {
        perror("[APP] Error opening " DEVICE_PATH);
        exit(EXIT_FAILURE);
    }

    // 3. Đọc ngay trạng thái ban đầu của LED và in ra
    current_state = read_led_status(dev_fd);
    if (current_state == -1) {
        close(dev_fd);
        exit(EXIT_FAILURE);
    }
    print_timestamp();
    printf("[APP] Initial LED status: %c\n", current_state);

    // 4. Tạo timerfd định kỳ 5s
    timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (timer_fd < 0) {
        perror("[APP] Failed to create timerfd");
        close(dev_fd);
        exit(EXIT_FAILURE);
    }

    its.it_value.tv_sec = BLINK_PERIOD_SEC;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = BLINK_PERIOD_SEC;
    its.it_interval.tv_nsec = 0;

    if (timerfd_settime(timer_fd, 0, &its, NULL) < 0) {
        perror("[APP] Failed to set timerfd time");
        close(timer_fd);
        close(dev_fd);
        exit(EXIT_FAILURE);
    }

    // 5. Vòng lặp điều khiển chính
    while (keep_running) {
        // Đợi timer nổ sau mỗi 5 giây
        ssize_t s = read(timer_fd, &expirations, sizeof(expirations));
        if (s < 0) {
            if (!keep_running) break; // Thoát nếu bị ngắt bởi signal
            perror("[APP] Timer read error");
            break;
        }

        // Đảo trạng thái: nếu đang '1' thì chuyển sang '0' và ngược lại
        target_cmd = (current_state == '1') ? '0' : '1';

        // Ghi lệnh ra driver
        lseek(dev_fd, 0, SEEK_SET);
        if (write(dev_fd, &target_cmd, 1) != 1) {
            perror("[APP] Failed to write command to LED");
            break;
        }

        // Đọc lại trạng thái để xác nhận phần cứng
        current_state = read_led_status(dev_fd);
        if (current_state == -1) break;

        // Báo lỗi mismatch nếu giá trị đọc về không khớp lệnh vừa ghi
        print_timestamp();
        if (current_state != target_cmd) {
            printf("[APP] WARNING: status mismatch! Expected %c but got %c\n", target_cmd, current_state);
        } else {
            if (current_state == '1') {
                printf("[APP] LED ON (status=1)\n");
            } else {
                printf("[APP] LED OFF (status=0)\n");
            }
        }
    }

    // 6. Đóng file descriptors trước khi thoát
    printf("\n[APP] Cleaning up and exiting...\n");
    if (timer_fd >= 0) close(timer_fd);
    if (dev_fd >= 0) close(dev_fd);

    return 0;
}