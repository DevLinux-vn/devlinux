/**
 * @file writer.c
 * @brief Config Writer utilizing memory-mapped file (mmap)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "device_cfg.h"

#define CFG_FILE "/tmp/device.cfg"

int main(void) {
    /* 1. Open or create the backing file */
    int fd = open(CFG_FILE, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    /* 2. Resize file to fit the exact struct size to prevent Bus Errors */
    if (ftruncate(fd, sizeof(device_cfg_t)) == -1) {
        perror("ftruncate failed");
        close(fd);
        exit(EXIT_FAILURE);
    }

    /* 3. Map the file into memory */
    device_cfg_t *cfg = mmap(NULL, sizeof(device_cfg_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (cfg == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    /* File descriptor is no longer needed after a successful mmap */
    close(fd);

    /* Initialize with default values if memory is currently zeroed out */
    if (cfg->baud_rate == 0) {
        cfg->baud_rate = 9600;
        cfg->sampling_rate_hz = 100;
        cfg->log_level = 2;
        msync(cfg, sizeof(device_cfg_t), MS_SYNC);
    }

    printf("[Config Writer] Loaded %s\n", CFG_FILE);

    char input[64];
    while (1) {
        printf("\nCurrent: baud_rate=%d sampling_rate=%d log_level=%d\n", 
               cfg->baud_rate, cfg->sampling_rate_hz, cfg->log_level);
        printf("Select field to update [baud/rate/log/quit]: ");
        
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "quit") == 0) break;

        if (strcmp(input, "baud") == 0) {
            printf("Select baud rate [9600/115200/460800]: ");
            if (fgets(input, sizeof(input), stdin)) {
                int val = atoi(input);
                if (val == 9600 || val == 115200 || val == 460800) {
                    cfg->baud_rate = val;
                    msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] baud_rate = %d\n", val);
                } else {
                    printf("Invalid baud rate.\n");
                }
            }
        } 
        else if (strcmp(input, "rate") == 0) {
            printf("Enter sampling rate (1-1000): ");
            if (fgets(input, sizeof(input), stdin)) {
                int val = atoi(input);
                if (val >= 1 && val <= 1000) {
                    cfg->sampling_rate_hz = val;
                    msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] sampling_rate_hz = %d\n", val);
                } else {
                    printf("Invalid sampling rate.\n");
                }
            }
        } 
        else if (strcmp(input, "log") == 0) {
            printf("Select log level [0=OFF, 1=ERROR, 2=INFO, 3=DEBUG]: ");
            if (fgets(input, sizeof(input), stdin)) {
                int val = atoi(input);
                if (val >= 0 && val <= 3) {
                    cfg->log_level = val;
                    msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] log_level = %d\n", val);
                } else {
                    printf("Invalid log level.\n");
                }
            }
        }
    }

    /* 4. Unmap memory region safely */
    if (munmap(cfg, sizeof(device_cfg_t)) == -1) perror("munmap failed");

    return EXIT_SUCCESS;
}