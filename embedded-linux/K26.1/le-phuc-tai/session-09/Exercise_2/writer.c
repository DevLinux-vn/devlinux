#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "device_cfg.h"

static void print_current_config(const device_cfg_t *cfg) {
    printf("Current: baud_rate=%d sampling_rate=%d log_level=%d\n",
           cfg->baud_rate, cfg->sampling_rate_hz, cfg->log_level);
}

int main(void) {
    setbuf(stdout, NULL);

    int fd = open(CONFIG_FILE_PATH, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("CRITICAL: Cannot open config file");
        return EXIT_FAILURE;
    }

    /* Đặt kích thước file khớp chuẩn kích thước cấu trúc dữ liệu doanh nghiệp */
    if (ftruncate(fd, sizeof(device_cfg_t)) < 0) {
        perror("CRITICAL: ftruncate failed");
        close(fd);
        return EXIT_FAILURE;
    }

    device_cfg_t *cfg = (device_cfg_t *)mmap(NULL, sizeof(device_cfg_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (cfg == MAP_FAILED) {
        perror("CRITICAL: mmap failed");
        close(fd);
        return EXIT_FAILURE;
    }

    /* Đóng file descriptor ngay sau khi mmap thành công để tối ưu tài nguyên OS */
    close(fd);
    printf("[Config Writer] Loaded %s\n", CONFIG_FILE_PATH);

    char choice[32];
    while (1) {
        print_current_config(cfg);
        printf("\nSelect field to update [baud/rate/log/quit]: ");
        if (fgets(choice, sizeof(choice), stdin) == NULL) break;
        choice[strcspn(choice, "\r\n")] = '\0';

        if (strcmp(choice, "quit") == 0) {
            break;
        } else if (strcmp(choice, "baud") == 0) {
            int new_baud;
            printf("Select baud rate [9600/115200/460800]: ");
            if (scanf("%d", &new_baud) == 1) {
                if (new_baud == 9600 || new_baud == 115200 || new_baud == 460800) {
                    cfg->baud_rate = new_baud;
                    msync(cfg, sizeof(device_cfg_t), MS_SYNC); /* Ép dữ liệu ghi xuống đĩa ngay */
                    printf("[Updated] baud_rate = %d\n", new_baud);
                } else {
                    printf("ERROR: Invalid baud rate value.\n");
                }
            }
            while (getchar() != '\n'); /* Xóa bộ đệm stdin thừa */
        } else if (strcmp(choice, "rate") == 0) {
            int new_rate;
            printf("Enter sampling rate hz [1-1000]: ");
            if (scanf("%d", &new_rate) == 1) {
                if (new_rate >= 1 && new_rate <= 1000) {
                    cfg->sampling_rate_hz = new_rate;
                    msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] sampling_rate_hz = %d\n", new_rate);
                } else {
                    printf("ERROR: Value must be inside range 1-1000.\n");
                }
            }
            while (getchar() != '\n');
        } else if (strcmp(choice, "log") == 0) {
            int new_log;
            printf("Select log level [0=OFF, 1=ERROR, 2=INFO, 3=DEBUG]: ");
            if (scanf("%d", &new_log) == 1) {
                if (new_log >= 0 && new_log <= 3) {
                    cfg->log_level = new_log;
                    msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] log_level = %d\n", new_log);
                } else {
                    printf("ERROR: Invalid log level.\n");
                }
            }
            while (getchar() != '\n');
        } else {
            printf("Unknown command.\n");
        }
        printf("---------------------------------------------------\n");
    }

    if (munmap(cfg, sizeof(device_cfg_t)) < 0) {
        perror("ERROR: munmap failed");
    }
    return EXIT_SUCCESS;
}