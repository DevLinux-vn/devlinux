#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "device_cfg.h"
#include <signal.h>

static device_cfg_t *g_cfg = NULL;
const char *log_level_name(int level) {
    switch (level) {
        case 0: return "OFF";
        case 1: return "ERROR";
        case 2: return "INFO";
        case 3: return "DEBUG";
        default: return "UNKNOWN";
    }
}

void cleanup(int sig) {
    (void)sig;

    printf("\n[Config Reader] Cleaning up. Goodbye.\n");

    if (g_cfg != NULL) {
        if (munmap(g_cfg, sizeof(device_cfg_t)) == -1) {
            perror("munmap");
        }
    }

    exit(0);
}

int main(void) {
    signal(SIGINT, cleanup);

    int fd = open(CFG_PATH, O_RDONLY);

    if (fd == -1) {
        perror("open");
        printf("Config file does not exist. Run config-writer first.\n");
        exit(1);
    }

    device_cfg_t *cfg = mmap(NULL, sizeof(device_cfg_t),
                             PROT_READ,
                             MAP_SHARED, fd, 0);

                             
    g_cfg = cfg;
    
    if (cfg == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(1);
    }

    close(fd);

    printf("[Config Reader] Polling %s every 2s...\n", CFG_PATH);

    while (1) {
        printf("baud_rate=%d  sampling_rate=%d Hz  log_level=%s\n",
               cfg->baud_rate,
               cfg->sampling_rate_hz,
               log_level_name(cfg->log_level));

        sleep(2);
    }

    if (munmap(cfg, sizeof(device_cfg_t)) == -1) {
        perror("munmap");
        exit(1);
    }

    return 0;
}