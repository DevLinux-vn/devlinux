#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#include "device_cfg.h"

static const char *log_level_name(int level)
{
    switch (level) {
        case 0: return "OFF";
        case 1: return "ERROR";
        case 2: return "INFO";
        case 3: return "DEBUG";
        default: return "UNKNOWN";
    }
}

int main(void)
{
    int fd = open(CFG_FILE_PATH, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "[Reader] Cannot open %s: %s\n", CFG_FILE_PATH, strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("[Reader] fstat");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (st.st_size < (off_t)sizeof(device_cfg_t)) {
        fprintf(stderr, "[Reader] Config file too small / not initialized yet.\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    device_cfg_t *cfg = mmap(NULL, sizeof(device_cfg_t),
                              PROT_READ, MAP_SHARED, fd, 0);
    if (cfg == MAP_FAILED) {
        perror("[Reader] mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd); 

    printf("[Config Reader] Polling %s every 2s...\n", CFG_FILE_PATH);

    while (1) {
        printf("baud_rate=%d  sampling_rate=%d Hz  log_level=%s\n",
               cfg->baud_rate, cfg->sampling_rate_hz,
               log_level_name(cfg->log_level));
        fflush(stdout);
        sleep(2);
    }
    
    munmap(cfg, sizeof(device_cfg_t));
    return 0;
}
