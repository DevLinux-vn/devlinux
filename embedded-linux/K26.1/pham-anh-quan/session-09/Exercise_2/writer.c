#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "device_cfg.h"

const char *log_level_str(int level) {
    switch(level) {
        case 0: return "OFF";
        case 1: return "ERROR";
        case 2: return "INFO";
        case 3: return "DEBUG";
        default: return "UNKNOWN";
    }
}

int main() {
    // 1. open() + ftruncate() + mmap()
    int fd = open("/tmp/device.cfg", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    // Ensure the file is large enough to hold the config struct
    if (ftruncate(fd, sizeof(device_cfg_t)) == -1) {
        perror("ftruncate failed");
        close(fd);
        exit(EXIT_FAILURE);
    }

    device_cfg_t *cfg = (device_cfg_t *)mmap(NULL, sizeof(device_cfg_t),
                                             PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (cfg == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // fd can be closed after a successful mmap
    close(fd);

    // Initialize default values if file is newly created (all zeros)
    if (cfg->baud_rate == 0) {
        cfg->baud_rate = 9600;
        cfg->sampling_rate_hz = 100;
        cfg->log_level = 2;
        msync(cfg, sizeof(device_cfg_t), MS_SYNC);
    }

    printf("[Config Writer] Loaded /tmp/device.cfg\n");

    char choice[32];
    while (1) {
        printf("Current: baud_rate=%d sampling_rate=%d log_level=%d\n\n", 
               cfg->baud_rate, cfg->sampling_rate_hz, cfg->log_level);
        printf("Select field to update [baud/rate/log/quit]: ");
        if (scanf("%31s", choice) != 1) break;

        if (strcmp(choice, "quit") == 0) {
            break;
        } else if (strcmp(choice, "baud") == 0) {
            int new_baud;
            printf("Select baud rate [9600/115200/460800]: ");
            if (scanf("%d", &new_baud) == 1) {
                if (new_baud == 9600 || new_baud == 115200 || new_baud == 460800) {
                    cfg->baud_rate = new_baud;
                    msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] baud_rate = %d\n", new_baud);
                } else {
                    printf("Invalid baud rate selection.\n");
                }
            }
        } else if (strcmp(choice, "rate") == 0) {
            int new_rate;
            printf("Enter an integer (1-1000): ");
            if (scanf("%d", &new_rate) == 1) {
                if (new_rate >= 1 && new_rate <= 1000) {
                    cfg->sampling_rate_hz = new_rate;
                    msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] sampling_rate_hz = %d\n", new_rate);
                } else {
                    printf("Rate out of range (1-1000).\n");
                }
            }
        } else if (strcmp(choice, "log") == 0) {
            int new_log;
            printf("Choose from {0=OFF, 1=ERROR, 2=INFO, 3=DEBUG}: ");
            if (scanf("%d", &new_log) == 1) {
                if (new_log >= 0 && new_log <= 3) {
                    cfg->log_level = new_log;
                    msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] log_level = %d (%s)\n", new_log, log_level_str(new_log));
                } else {
                    printf("Invalid log level.\n");
                }
            }
        } else {
            printf("Unknown field.\n");
        }
        printf("\n------------------------------------------------\n");
    }

    munmap(cfg, sizeof(device_cfg_t));
    return 0;
}