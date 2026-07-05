#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "device_cfg.h"
#include <signal.h>

#define INPUT_MAX 64
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

void sync_config(device_cfg_t *cfg) {
    if (msync(cfg, sizeof(device_cfg_t), MS_SYNC) == -1) {
        perror("msync");
        exit(1);
    }
}

void print_current(device_cfg_t *cfg) {
    printf("Current: baud_rate=%d sampling_rate=%d log_level=%d(%s)\n",
           cfg->baud_rate,
           cfg->sampling_rate_hz,
           cfg->log_level,
           log_level_name(cfg->log_level));
}

void cleanup(int sig) {
    (void)sig;

    printf("\n[Config Writer] Cleaning up. Goodbye.\n");

    if (g_cfg != NULL) {
        if (munmap(g_cfg, sizeof(device_cfg_t)) == -1) {
            perror("munmap");
        }
    }

    exit(0);
}

int main(void) {
    signal(SIGINT, cleanup);

    int fd = open(CFG_PATH, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    if (ftruncate(fd, sizeof(device_cfg_t)) == -1) {
        perror("ftruncate");
        close(fd);
        exit(1);
    }

    device_cfg_t *cfg = mmap(NULL, sizeof(device_cfg_t),
                             PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, 0);

    g_cfg = cfg;

    if (cfg == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(1);
    }

    close(fd);

    if (cfg->baud_rate == 0) {
        cfg->baud_rate = 9600;
        cfg->sampling_rate_hz = 100;
        cfg->log_level = 2;
        sync_config(cfg);
    }

    printf("[Config Writer] Loaded %s\n", CFG_PATH);

    char input[INPUT_MAX];

    while (1) {
        print_current(cfg);

        printf("\nSelect field to update [baud/rate/log/quit]: ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nEOF detected. Exiting.\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "quit") == 0) {
            break;
        }

        if (strcmp(input, "baud") == 0) {
            int baud;

            printf("Select baud rate [9600/115200/460800]: ");

            if (scanf("%d", &baud) != 1) {
                printf("Invalid input.\n");
                while (getchar() != '\n');
                continue;
            }

            while (getchar() != '\n');

            if (baud != 9600 && baud != 115200 && baud != 460800) {
                printf("Invalid baud rate.\n");
                continue;
            }

            cfg->baud_rate = baud;
            sync_config(cfg);

            printf("[Updated] baud_rate = %d\n", baud);
        }

        else if (strcmp(input, "rate") == 0) {
            int rate;

            printf("Enter sampling rate Hz [1-1000]: ");

            if (scanf("%d", &rate) != 1) {
                printf("Invalid input.\n");
                while (getchar() != '\n');
                continue;
            }

            while (getchar() != '\n');

            if (rate < 1 || rate > 1000) {
                printf("Invalid sampling rate.\n");
                continue;
            }

            cfg->sampling_rate_hz = rate;
            sync_config(cfg);

            printf("[Updated] sampling_rate_hz = %d\n", rate);
        }

        else if (strcmp(input, "log") == 0) {
            int level;

            printf("Select log level [0=OFF, 1=ERROR, 2=INFO, 3=DEBUG]: ");

            if (scanf("%d", &level) != 1) {
                printf("Invalid input.\n");
                while (getchar() != '\n');
                continue;
            }

            while (getchar() != '\n');

            if (level < 0 || level > 3) {
                printf("Invalid log level.\n");
                continue;
            }

            cfg->log_level = level;
            sync_config(cfg);

            printf("[Updated] log_level = %d(%s)\n",
                   level,
                   log_level_name(level));
        }

        else {
            printf("Unknown command.\n");
        }
    }

    if (munmap(cfg, sizeof(device_cfg_t)) == -1) {
        perror("munmap");
        exit(1);
    }

    printf("[Config Writer] Goodbye.\n");

    return 0;
}