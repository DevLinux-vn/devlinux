#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "device_cfg.h"

#define CONFIG_FILE "/tmp/device.cfg"

int main() {
    // Open or create file
    int fd = open(CONFIG_FILE, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    
    // Truncate to config size
    if (ftruncate(fd, sizeof(device_cfg_t)) == -1) {
        perror("ftruncate");
        close(fd);
        exit(1);
    }
    
    // Map file into memory
    device_cfg_t *cfg = mmap(NULL, sizeof(device_cfg_t),
                             PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (cfg == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(1);
    }
    
    close(fd);  // Can close after successful mmap
    
    // Initialize default config if this is the first run
    if (cfg->baud_rate == 0) {
        cfg->baud_rate = 9600;
        cfg->sampling_rate_hz = 100;
        cfg->log_level = 2;  // INFO
        msync(cfg, sizeof(device_cfg_t), MS_SYNC);
    }
    
    printf("[Config Writer] Loaded %s\n", CONFIG_FILE);
    
    char input[256];
    
    while (1) {
        printf("\nCurrent: baud_rate=%d sampling_rate=%d log_level=%d\n",
               cfg->baud_rate, cfg->sampling_rate_hz, cfg->log_level);
        
        printf("Select field to update [baud/rate/log/quit]: ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        // Remove trailing newline
        size_t len = strlen(input);
        if (input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        
        if (strcmp(input, "quit") == 0) {
            break;
        }
        
        if (strcmp(input, "baud") == 0) {
            printf("Select baud rate [9600/115200/460800]: ");
            fflush(stdout);
            fgets(input, sizeof(input), stdin);
            int baud = atoi(input);
            if (baud == 9600 || baud == 115200 || baud == 460800) {
                cfg->baud_rate = baud;
                msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                printf("[Updated] baud_rate = %d\n", baud);
            } else {
                printf("[Error] Invalid baud rate\n");
            }
        } else if (strcmp(input, "rate") == 0) {
            printf("Enter sampling rate (1-1000 Hz): ");
            fflush(stdout);
            fgets(input, sizeof(input), stdin);
            int rate = atoi(input);
            if (rate >= 1 && rate <= 1000) {
                cfg->sampling_rate_hz = rate;
                msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                printf("[Updated] sampling_rate = %d\n", rate);
            } else {
                printf("[Error] Invalid sampling rate\n");
            }
        } else if (strcmp(input, "log") == 0) {
            printf("Select log level [0=OFF/1=ERROR/2=INFO/3=DEBUG]: ");
            fflush(stdout);
            fgets(input, sizeof(input), stdin);
            int level = atoi(input);
            if (level >= 0 && level <= 3) {
                cfg->log_level = level;
                msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                printf("[Updated] log_level = %d\n", level);
            } else {
                printf("[Error] Invalid log level\n");
            }
        } else {
            printf("[Error] Unknown field\n");
        }
    }
    
    munmap(cfg, sizeof(device_cfg_t));
    printf("[Config Writer] Goodbye.\n");
    
    return 0;
}
