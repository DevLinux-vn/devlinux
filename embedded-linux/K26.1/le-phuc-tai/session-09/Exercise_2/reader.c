/**
 * @file reader.c
 * @brief Periodically samples and outputs shared memory config file attributes.
 * @date 2026-07-03
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include "device_cfg.h"

#define INTERVAL_SEC 2

volatile sig_atomic_t g_keep_running = 1;

static void handle_sigint(int signum) {
    (void)signum;
    g_keep_running = 0;
}

static const char *get_log_level_str(int level) {
    switch (level) {
        case 0: return "OFF";
        case 1: return "ERROR";
        case 2: return "INFO";
        case 3: return "DEBUG";
        default: return "UNKNOWN";
    }
}

int main(void) {
    setbuf(stdout, NULL);

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("CRITICAL: Failed to register SIGINT");
        return EXIT_FAILURE;
    }

    int fd = open(CONFIG_FILE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("CRITICAL: Cannot open config file. Start writer process first");
        return EXIT_FAILURE;
    }

    device_cfg_t *cfg = (device_cfg_t *)mmap(NULL, sizeof(device_cfg_t), PROT_READ, MAP_SHARED, fd, 0);
    if (cfg == MAP_FAILED) {
        perror("CRITICAL: mmap failed");
        close(fd);
        return EXIT_FAILURE;
    }
    close(fd);

    printf("[Config Reader] Polling %s every %ds...\n", CONFIG_FILE_PATH, INTERVAL_SEC);

    while (g_keep_running) {
        printf("baud_rate=%d  sampling_rate=%d Hz  log_level=%s\n",
               cfg->baud_rate, cfg->sampling_rate_hz, get_log_level_str(cfg->log_level));
        sleep(INTERVAL_SEC);
    }

    printf("\n[Config Reader] Unmapping memory core...\n");
    if (munmap(cfg, sizeof(device_cfg_t)) < 0) perror("ERROR: munmap failed");

    return EXIT_SUCCESS;
}