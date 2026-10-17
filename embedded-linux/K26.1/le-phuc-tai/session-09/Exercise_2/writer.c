#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include "device_cfg.h"

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

static void handle_baud_update(device_cfg_t *cfg) {
    int new_baud = 0;
    printf("Select baud rate [%d/%d/%d]: ", BAUD_LOW, BAUD_MID, BAUD_HIGH);
    if (acquire_console_int_protected(&new_baud) == 1) {
        if (new_baud == BAUD_LOW || new_baud == BAUD_MID || new_baud == BAUD_HIGH) {
            cfg->baud_rate = new_baud;
            dispatch_memory_flush(cfg, sizeof(device_cfg_t), MS_SYNC);
            printf("[Updated] baud_rate = %d\n", new_baud);
        } else {
            printf("ERROR: Invalid baud rate.\n");
        }
    }
    while (getchar() != '\n');
}

static void handle_rate_update(device_cfg_t *cfg) {
    int new_rate = 0;
    printf("Enter sampling rate hz [%d-%d]: ", SAMPLING_MIN, SAMPLING_MAX);
    if (acquire_console_int_protected(&new_rate) == 1) {
        if (new_rate >= SAMPLING_MIN && new_rate <= SAMPLING_MAX) {
            cfg->sampling_rate_hz = new_rate;
            dispatch_memory_flush(cfg, sizeof(device_cfg_t), MS_SYNC);
            printf("[Updated] sampling_rate_hz = %d\n", new_rate);
        } else {
            printf("ERROR: Out of bounds.\n");
        }
    }
    while (getchar() != '\n');
}

static void handle_log_update(device_cfg_t *cfg) {
    int new_log = 0;
    printf("Select log level [0=OFF, 1=ERROR, 2=INFO, 3=DEBUG]: ");
    if (acquire_console_int_protected(&new_log) == 1) {
        if (new_log >= 0 && new_log <= 3) {
            cfg->log_level = new_log;
            dispatch_memory_flush(cfg, sizeof(device_cfg_t), MS_SYNC);
            printf("[Updated] log_level = %d\n", new_log);
        } else {
            printf("ERROR: Invalid log level.\n");
        }
    }
    while (getchar() != '\n');
}

static void display_interactive_menu(device_cfg_t *cfg) {
    char choice[MENU_BUF_SIZE];
    while (1) {
        printf("Current: baud_rate=%d sampling_rate=%d log_level=%d\n",
               cfg->baud_rate, cfg->sampling_rate_hz, cfg->log_level);
        printf("\nSelect field to update [baud/rate/log/quit]: ");
        
        if (acquire_console_line_protected(choice, sizeof(choice), stdin) == NULL) break;
        choice[strcspn(choice, "\r\n")] = '\0';

        if (strcmp(choice, "quit") == 0) break;
        else if (strcmp(choice, "baud") == 0) handle_baud_update(cfg);
        else if (strcmp(choice, "rate") == 0) handle_rate_update(cfg);
        else if (strcmp(choice, "log") == 0) handle_log_update(cfg);
        else printf("Unknown option.\n");
        printf("---------------------------------------------------\n");
    }
}

int main(void) {
    setbuf(stdout, NULL);
    int needs_initialization = (access(CONFIG_FILE_PATH, F_OK) != 0);

    int fd = open(CONFIG_FILE_PATH, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("CRITICAL: Cannot open file");
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

    if (needs_initialization) {
        memset(cfg, 0, sizeof(device_cfg_t));
        cfg->baud_rate = BAUD_LOW;
        cfg->sampling_rate_hz = 100;
        cfg->log_level = 2;
        dispatch_memory_flush(cfg, sizeof(device_cfg_t), MS_SYNC);
    }

    printf("[Config Writer] Loaded %s\n", CONFIG_FILE_PATH);
    display_interactive_menu(cfg);

    if (munmap(cfg, sizeof(device_cfg_t)) < 0) perror("ERROR: munmap failed");
    return EXIT_SUCCESS;
}