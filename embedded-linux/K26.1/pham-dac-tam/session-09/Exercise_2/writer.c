#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <ctype.h>
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
        if (msync(cfg, sizeof(device_cfg_t), MS_SYNC) == -1) {
            perror("msync (init)");
            munmap(cfg, sizeof(device_cfg_t));
            exit(1);
        }
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
            if (fgets(input, sizeof(input), stdin) == NULL) {
                printf("[Error] fgets failed\n");
                continue;
            }
            
            char *endptr;
            long baud = strtol(input, &endptr, 10);
            
            if (endptr == input || (*endptr != '\n' && *endptr != '\0')) {
                printf("[Error] Invalid input format\n");
                continue;
            }
            
            if (baud == 9600 || baud == 115200 || baud == 460800) {
                cfg->baud_rate = (int)baud;
                if (msync(cfg, sizeof(device_cfg_t), MS_SYNC) == -1) {
                    perror("msync (baud)");
                } else {
                    printf("[Updated] baud_rate = %d\n", cfg->baud_rate);
                }
            } else {
                printf("[Error] Invalid baud rate (choose 9600, 115200, or 460800)\n");
            }
        } else if (strcmp(input, "rate") == 0) {
            printf("Enter sampling rate (1-1000 Hz): ");
            fflush(stdout);
            if (fgets(input, sizeof(input), stdin) == NULL) {
                printf("[Error] fgets failed\n");
                continue;
            }
            
            char *endptr;
            long rate = strtol(input, &endptr, 10);
            
            if (endptr == input || (*endptr != '\n' && *endptr != '\0')) {
                printf("[Error] Invalid input format\n");
                continue;
            }
            
            if (rate >= 1 && rate <= 1000) {
                cfg->sampling_rate_hz = (int)rate;
                if (msync(cfg, sizeof(device_cfg_t), MS_SYNC) == -1) {
                    perror("msync (rate)");
                } else {
                    printf("[Updated] sampling_rate = %d\n", cfg->sampling_rate_hz);
                }
            } else {
                printf("[Error] Invalid sampling rate (must be 1-1000)\n");
            }
        } else if (strcmp(input, "log") == 0) {
            printf("Select log level [0=OFF/1=ERROR/2=INFO/3=DEBUG]: ");
            fflush(stdout);
            if (fgets(input, sizeof(input), stdin) == NULL) {
                printf("[Error] fgets failed\n");
                continue;
            }
            
            char *endptr;
            long level = strtol(input, &endptr, 10);
            
            if (endptr == input || (*endptr != '\n' && *endptr != '\0')) {
                printf("[Error] Invalid input format\n");
                continue;
            }
            
            if (level >= 0 && level <= 3) {
                cfg->log_level = (int)level;
                if (msync(cfg, sizeof(device_cfg_t), MS_SYNC) == -1) {
                    perror("msync (log)");
                } else {
                    printf("[Updated] log_level = %d\n", cfg->log_level);
                }
            } else {
                printf("[Error] Invalid log level (choose 0-3)\n");
            }
        } else {
            printf("[Error] Unknown field (use baud/rate/log/quit)\n");
        }
    }
    
    if (munmap(cfg, sizeof(device_cfg_t)) == -1) {
        perror("munmap");
    }
    printf("[Config Writer] Goodbye.\n");
    
    return 0;
}
