/**
 * @file reader.c
 * @brief Config Reader utilizing memory-mapped file (mmap)
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include "device_cfg.h"

#define CFG_FILE "/tmp/device.cfg"

static volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
}

const char* get_log_level_str(int level) {
    switch(level) {
        case 0: return "OFF";
        case 1: return "ERROR";
        case 2: return "INFO";
        case 3: return "DEBUG";
        default: return "UNKNOWN";
    }
}

int main(void) {
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    int fd = open(CFG_FILE, O_RDONLY);
    if (fd < 0) {
        perror("open failed. Did you start the writer?");
        exit(EXIT_FAILURE);
    }

    /* Map read-only */
    device_cfg_t *cfg = mmap(NULL, sizeof(device_cfg_t), PROT_READ, MAP_SHARED, fd, 0);
    if (cfg == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);

    printf("[Config Reader] Polling %s every 2s...\n", CFG_FILE);

    while (keep_running) {
        printf("baud_rate=%d  sampling_rate=%d Hz  log_level=%s\n", 
               cfg->baud_rate, cfg->sampling_rate_hz, get_log_level_str(cfg->log_level));
        
        unsigned int rem = 2;
        while(rem > 0 && keep_running) rem = sleep(rem);
    }

    munmap((void *)cfg, sizeof(device_cfg_t));
    return EXIT_SUCCESS;
}