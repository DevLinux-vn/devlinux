#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>

#include "device_cfg.h"

#define POLLING_INTERVAL_SEC 2

volatile sig_atomic_t g_is_running = 1;

static void handle_sigint(int signum) {
    (void)signum;
    g_is_running = 0;
}

/* Helper to convert log_level int to string */
static const char* get_log_level_str(int level) {
    switch(level) {
        case LOG_LEVEL_OFF:   return "OFF";
        case LOG_LEVEL_ERROR: return "ERROR";
        case LOG_LEVEL_INFO:  return "INFO";
        case LOG_LEVEL_DEBUG: return "DEBUG";
        default:              return "UNKNOWN";
    }
}

int main(void) {
    /* 1. Register SIGINT handler for graceful exit */
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    /* 2. Open the file (Read Only) */
    int fd = open(CONFIG_FILE_PATH, O_RDONLY);
    if (fd < 0) {
        if (errno == ENOENT) {
            fprintf(stderr, "[Reader] Config file %s does not exist. Run writer first.\n", CONFIG_FILE_PATH);
        } else {
            perror("[Reader] open failed");
        }
        exit(EXIT_FAILURE);
    }

    /* 3. Memory Map the file (Read Only) */
    device_cfg_t *cfg = mmap(NULL, sizeof(device_cfg_t), 
                             PROT_READ, MAP_SHARED, fd, 0);
    if (cfg == MAP_FAILED) {
        perror("[Reader] mmap failed");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    close(fd);

    printf("[Config Reader] Polling %s every %ds...\n", CONFIG_FILE_PATH, POLLING_INTERVAL_SEC);

    /* 4. Polling Loop */
    while (g_is_running) {
        /* Read directly from memory pointer */
        printf("baud_rate=%d  sampling_rate=%d Hz  log_level=%s\n",
               cfg->baud_rate, 
               cfg->sampling_rate_hz, 
               get_log_level_str(cfg->log_level));

        /* Safe sleep */
        unsigned int time_left = POLLING_INTERVAL_SEC;
        while (time_left > 0 && g_is_running) {
            time_left = sleep(time_left);
        }
    }

    /* 5. Cleanup */
    printf("\n[Config Reader] Shutting down...\n");
    if (munmap(cfg, sizeof(device_cfg_t)) == -1) {
        perror("[Reader] munmap failed");
    }

    return EXIT_SUCCESS;
}
