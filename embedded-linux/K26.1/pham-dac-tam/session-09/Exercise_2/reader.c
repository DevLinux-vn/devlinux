#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include "device_cfg.h"

#define CONFIG_FILE "/tmp/device.cfg"

int main() {
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
    
    while (1) {
        printf("baud_rate=%d  sampling_rate=%d Hz  log_level=", cfg->baud_rate, cfg->sampling_rate_hz);
        
        switch (cfg->log_level) {
            case 0: printf("OFF"); break;
            case 1: printf("ERROR"); break;
            case 2: printf("INFO"); break;
            case 3: printf("DEBUG"); break;
            default: printf("?"); break;
        }
        printf("\n");
        
        sleep(2);
    }
    
    return 0;
}
