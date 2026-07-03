/**
 * @file writer.c
 * @brief Handles human interactive tuning loops with safe file flushing mechanics.
 * @date 2026-07-04
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include "device_cfg.h"

/**
 * @brief Standard stream input acquisition engine handling persistent interrupt signals.
 */
static char *acquire_console_line_protected(char *str, int num, FILE *stream) {
    char *result;
    while (1) {
        result = fgets(str, num, stream);
        if (result != NULL) break;
        if (errno == EINTR) {
            clearerr(stream); 
            continue;
        }
        break;
    }
    return result;
}

/**
 * @brief Standard stream integer evaluation engine handling persistent interrupt signals.
 */
static int acquire_console_int_protected(int *value) {
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

static int dispatch_memory_flush(void *addr, size_t length, int flags) {
    int res;
    do {
        res = msync(addr, length, flags);
    } while (res < 0 && errno == EINTR);
    return res;
}

static void display_interactive_menu(device_cfg_t *cfg) {
    char choice[MENU_BUF_SIZE];
    
    while (1) {
        printf("Current: baud_rate=%d sampling_rate=%d log_level=%d\n",
               cfg->baud_rate, cfg->sampling_rate_hz, cfg->log_level);
        printf("\nSelect field to update [baud/rate/log/quit]: ");
        
        if (acquire_console_line_protected(choice, sizeof(choice), stdin) == NULL) break;
        choice[strcspn(choice, "\r\n")] = '\0';

        if (strcmp(choice, "quit") == 0) {
            break;
        } else if (strcmp(choice, "baud") == 0) {
            int new_baud = 0;
            printf("Select baud rate [%d/%d/%d]: ", BAUD_LOW, BAUD_MID, BAUD_HIGH);
            if (acquire_console_int_protected(&new_baud) == 1) {
                if (new_baud == BAUD_LOW || new_baud == BAUD_MID || new_baud == BAUD_HIGH) {
                    cfg->baud_rate = new_baud;
                    dispatch_memory_flush(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] baud_rate = %d\n", new_baud);
                } else {
                    printf("ERROR: Invalid baud rate option.\n");
                }
            }
            while (getchar() != '\n');
        } else if (strcmp(choice, "rate") == 0) {
            int new_rate = 0;
            printf("Enter sampling rate hz [%d-%d]: ", SAMPLING_MIN, SAMPLING_MAX);
            if (acquire_console_int_protected(&new_rate) == 1) {
                if (new_rate >= SAMPLING_MIN && new_rate <= SAMPLING_MAX) {
                    cfg->sampling_rate_hz = new_rate;
                    dispatch_memory_flush(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] sampling_rate_hz = %d\n", new_rate);
                } else {
                    printf("ERROR: Bounds violation.\n");
                }
            }
            while (getchar() != '\n');
        } else if (strcmp(choice, "log") == 0) {
            int new_log = 0;
            printf("Select log level [0=OFF, 1=ERROR, 2=INFO, 3=DEBUG]: ");
            if (acquire_console_int_protected(&new_log) == 1) {
                if (new_log >= 0 && new_log <= 3) {
                    cfg->log_level = new_log;
                    dispatch_memory_flush(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] log_level = %d\n", new_log);
                } else {
                    printf("ERROR: Invalid log scale range.\n");
                }
            }
            while (getchar() != '\n');
        } else {
            printf("Unknown operational choice.\n");
        }
        printf("---------------------------------------------------\n");
    }
}

int main(void) {
    setbuf(stdout, NULL);
    int needs_initialization = (access(CONFIG_FILE_PATH, F_OK) != 0);

    int fd = open(CONFIG_FILE_PATH, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("CRITICAL: Cannot open system storage file");
        return EXIT_FAILURE;
    }

    if (ftruncate(fd, sizeof(device_cfg_t)) < 0) {
        perror("CRITICAL: storage ftruncate resizing failed");
        close(fd);
        return EXIT_FAILURE;
    }

    device_cfg_t *cfg = (device_cfg_t *)mmap(NULL, sizeof(device_cfg_t),
                                             PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (cfg == MAP_FAILED) {
        perror("CRITICAL: Core mmap execution failed");
        close(fd);
        return EXIT_FAILURE;
    }
    close(fd);

    /* Clean initialization step if file allocation occurs for the first time */
    if (needs_initialization) {
        memset(cfg, 0, sizeof(device_cfg_t));
        cfg->baud_rate = BAUD_LOW;
        cfg->sampling_rate_hz = 100; /* Standard 100Hz base telemetry */
        cfg->log_level = 2;          /* Default operational index to INFO */
        dispatch_memory_flush(cfg, sizeof(device_cfg_t), MS_SYNC);
    }

    printf("[Config Writer] Synchronized target: %s\n", CONFIG_FILE_PATH);
    display_interactive_menu(cfg);

    if (munmap(cfg, sizeof(device_cfg_t)) < 0) perror("ERROR: munmap teardown failed");
    return EXIT_SUCCESS;
}