#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include "device_cfg.h"

/* Robust wrapper to prevent stream freezing on signal interrupts */
static char *safe_fgets(char *str, int num, FILE *stream) {
    char *result;
    while (1) {
        result = fgets(str, num, stream);
        if (result != NULL) break;
        if (errno == EINTR) {
            clearerr(stream); /* Clears stream error indicator to resume I/O */
            continue;
        }
        break;
    }
    return result;
}

static int safe_scanf_int(int *value) {
    int res;
    while (1) {
        res = scanf("%d", value);
        if (res >= 0) break;
        if (res == EOF && errno == EINTR) {
            clearerr(stdin);
            continue;
        }
        break;
    }
    return res;
}

/* Retries flushing when interrupted to avoid partial file writes */
static int safe_msync(void *addr, size_t length, int flags) {
    int res;
    do {
        res = msync(addr, length, flags);
    } while (res < 0 && errno == EINTR);
    return res;
}

static void print_current_config(const device_cfg_t *cfg) {
    printf("Current: baud_rate=%d sampling_rate=%d log_level=%d\n",
           cfg->baud_rate, cfg->sampling_rate_hz, cfg->log_level);
}

static void execute_menu_interaction(device_cfg_t *cfg) {
    char choice[MENU_BUF_SIZE];
    
    while (1) {
        print_current_config(cfg);
        printf("\nSelect field to update [baud/rate/log/quit]: ");
        
        if (safe_fgets(choice, sizeof(choice), stdin) == NULL) break;
        choice[strcspn(choice, "\r\n")] = '\0';

        if (strcmp(choice, "quit") == 0) {
            break;
        } else if (strcmp(choice, "baud") == 0) {
            int new_baud = 0;
            printf("Select baud rate [9600/115200/460800]: ");
            if (safe_scanf_int(&new_baud) == 1) {
                if (new_baud == 9600 || new_baud == 115200 || new_baud == 460800) {
                    cfg->baud_rate = new_baud;
                    safe_msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] baud_rate = %d\n", new_baud);
                } else {
                    printf("ERROR: Invalid baud rate.\n");
                }
            }
            while (getchar() != '\n');
        } else if (strcmp(choice, "rate") == 0) {
            int new_rate = 0;
            printf("Enter sampling rate hz [1-1000]: ");
            if (safe_scanf_int(&new_rate) == 1) {
                if (new_rate >= 1 && new_rate <= 1000) {
                    cfg->sampling_rate_hz = new_rate;
                    safe_msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] sampling_rate_hz = %d\n", new_rate);
                } else {
                    printf("ERROR: Out of bounds (1-1000).\n");
                }
            }
            while (getchar() != '\n');
        } else if (strcmp(choice, "log") == 0) {
            int new_log = 0;
            printf("Select log level [0=OFF, 1=ERROR, 2=INFO, 3=DEBUG]: ");
            if (safe_scanf_int(&new_log) == 1) {
                if (new_log >= 0 && new_log <= 3) {
                    cfg->log_level = new_log;
                    safe_msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] log_level = %d\n", new_log);
                } else {
                    printf("ERROR: Invalid log level.\n");
                }
            }
            while (getchar() != '\n');
        } else {
            printf("Unknown command.\n");
        }
        printf("---------------------------------------------------\n");
    }
}

int main(void) {
    setbuf(stdout, NULL);
    int is_new_file = (access(CONFIG_FILE_PATH, F_OK) != 0);

    int fd = open(CONFIG_FILE_PATH, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("CRITICAL: Cannot open config file");
        return EXIT_FAILURE;
    }

    if (ftruncate(fd, sizeof(device_cfg_t)) < 0) {
        perror("CRITICAL: ftruncate failed");
        close(fd);
        return EXIT_FAILURE;
    }

    device_cfg_t *cfg = (device_cfg_t *)mmap(NULL, sizeof(device_cfg_t),
                                             PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (cfg == MAP_FAILED) {
        perror("CRITICAL: mmap failed");
        close(fd);
        return EXIT_FAILURE;
    }
    close(fd);

    /* Erase junk bytes and inject defaults if file was just created */
    if (is_new_file) {
        memset(cfg, 0, sizeof(device_cfg_t));
        cfg->baud_rate = 9600;
        cfg->sampling_rate_hz = 100;
        cfg->log_level = 2; /* Default: INFO */
        safe_msync(cfg, sizeof(device_cfg_t), MS_SYNC);
    }

    printf("[Config Writer] Loaded %s\n", CONFIG_FILE_PATH);
    execute_menu_interaction(cfg);

    if (munmap(cfg, sizeof(device_cfg_t)) < 0) perror("ERROR: munmap failed");
    return EXIT_SUCCESS;
}