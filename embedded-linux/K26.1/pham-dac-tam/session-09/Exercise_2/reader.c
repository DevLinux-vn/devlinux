#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>
#include "device_cfg.h"

#define CONFIG_FILE "/tmp/device.cfg"

volatile sig_atomic_t should_exit = 0;

void signal_handler(int sig) {
    (void)sig;
    should_exit = 1;
}

int main() {
    // Register signal handler
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    
    // Open file for reading
    int fd = open(CONFIG_FILE, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    
    // Map file into memory (read-only)
    device_cfg_t *cfg = mmap(NULL, sizeof(device_cfg_t),
                             PROT_READ, MAP_SHARED, fd, 0);
    if (cfg == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(1);
    }
    
    close(fd);  // Can close after successful mmap
    
    printf("[Config Reader] Polling %s every 2s...\n", CONFIG_FILE);
    if (fflush(stdout) == EOF) {
        fprintf(stderr, "[Config Reader] fflush failed\n");
    }
    
    while (!should_exit) {
        printf("baud_rate=%d  sampling_rate=%d Hz  log_level=", cfg->baud_rate, cfg->sampling_rate_hz);
        
        switch (cfg->log_level) {
            case 0: printf("OFF"); break;
            case 1: printf("ERROR"); break;
            case 2: printf("INFO"); break;
            case 3: printf("DEBUG"); break;
            default: printf("UNKNOWN"); break;
        }
        printf("\n");
        
        if (fflush(stdout) == EOF) {
            fprintf(stderr, "[Config Reader] fflush failed\n");
        }
        
        sleep(2);
    }
    
    printf("\n[Config Reader] Shutting down...\n");
    
    if (munmap(cfg, sizeof(device_cfg_t)) == -1) {
        perror("munmap");
    }
    
    printf("[Config Reader] Goodbye.\n");
    
    return 0;
}
