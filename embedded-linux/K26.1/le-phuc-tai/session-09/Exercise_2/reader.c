/**
 * @file reader.c
 * @brief Tracking daemon processing shared mapped configs using accurate POSIX nanosleep.
 * @date 2026-07-04
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <errno.h>
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
        perror("CRITICAL: Storage open failed. Execute writer binary first");
        return EXIT_FAILURE;
    }

    device_cfg_t *cfg = (device_cfg_t *)mmap(NULL, sizeof(device_cfg_t),
                                             PROT_READ, MAP_SHARED, fd, 0);
    if (cfg == MAP_FAILED) {
        perror("CRITICAL: Reader mmap attachment failed");
        close(fd);
        return EXIT_FAILURE;
    }
    close(fd);

    printf("[Config Reader] Polling %s every %ds...\n", CONFIG_FILE_PATH, INTERVAL_SEC);

    while (g_keep_running) {
        printf("baud_rate=%d  sampling_rate=%d Hz  log_level=%s\n",
               cfg->baud_rate, cfg->sampling_rate_hz, get_log_level_str(cfg->log_level));
        
        /* High-precision defensive sleep block mitigating EINTR signals */
        struct timespec requested_interval = { .tv_sec = INTERVAL_SEC, .tv_nsec = 0 };
        struct timespec remaining_interval;
        
        while (nanosleep(&requested_interval, &remaining_interval) < 0) {
            if (errno == EINTR) {
                if (!g_keep_running) break;
                requested_interval = remaining_interval; /* Continue sleeping the remainder time */
            } else {
                perror("ERROR: Precise nanosleep tracking crashed");
                break;
            }
        }
    }

    printf("\n[Config Reader] Unmapping node segments cleanly...\n");
    if (munmap(cfg, sizeof(device_cfg_t)) < 0) perror("ERROR: Reader munmap failed");

    return EXIT_SUCCESS;
}