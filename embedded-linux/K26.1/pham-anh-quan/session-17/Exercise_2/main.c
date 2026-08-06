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

static void handle_signal(int sig) {
    (void)sig;
    keep_running = 0;
}

static void print_timestamp(void) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char buffer[26];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("[%s] ", buffer);
}

static char read_led_status(int fd) {
    char status = '0';
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

    // Signal handler cho Ctrl+C
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    // Mở thiết bị /dev/led_blink
    dev_fd = open(DEVICE_PATH, O_RDWR);
    if (dev_fd < 0) {
        perror("[APP] Error opening " DEVICE_PATH);
        exit(EXIT_FAILURE);
    }

    // Đọc trạng thái ban đầu
    current_state = read_led_status(dev_fd);
    if (current_state == (char)-1) {
        close(dev_fd);
        exit(EXIT_FAILURE);
    }
    print_timestamp();
    printf("[APP] Initial LED status: %c\n", current_state);

    // Tạo timerfd 5 giây
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

    // Vòng lặp bật/tắt LED
    while (keep_running) {
        ssize_t s = read(timer_fd, &expirations, sizeof(expirations));
        if (s < 0) {
            if (!keep_running) break;
            perror("[APP] Timer read error");
            break;
        }

        target_cmd = (current_state == '1') ? '0' : '1';

        lseek(dev_fd, 0, SEEK_SET);
        if (write(dev_fd, &target_cmd, 1) != 1) {
            perror("[APP] Failed to write command to LED");
            break;
        }

        current_state = read_led_status(dev_fd);
        if (current_state == (char)-1) break;

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

    printf("\n[APP] Cleaning up and exiting...\n");
    if (timer_fd >= 0) close(timer_fd);
    if (dev_fd >= 0) close(dev_fd);

    return 0;
}