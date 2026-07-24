#include <stdio.h>
#include <stdlib.h>
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
    // 1. open() + mmap(PROT_READ, MAP_SHARED)
    int fd = open("/tmp/device.cfg", O_RDONLY);
    if (fd == -1) {
        perror("open failed. Ensure writer.c has run at least once to create the file");
        exit(EXIT_FAILURE);
    }

    device_cfg_t *cfg = (device_cfg_t *)mmap(NULL, sizeof(device_cfg_t),
                                             PROT_READ, MAP_SHARED, fd, 0);
    if (cfg == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd); // fd can be closed after a successful mmap

    printf("[Config Reader] Polling /tmp/device.cfg every 2s...\n");

    // Loop every 2 seconds
    while (1) {
        printf("baud_rate=%-8d sampling_rate=%-3d Hz  log_level=%s\n", 
               cfg->baud_rate, cfg->sampling_rate_hz, log_level_str(cfg->log_level));
        sleep(2);
    }

    munmap(cfg, sizeof(device_cfg_t));
    return 0;
}