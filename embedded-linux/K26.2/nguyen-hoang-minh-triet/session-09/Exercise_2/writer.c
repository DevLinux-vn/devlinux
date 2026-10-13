#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#include "device_cfg.h"

#define MAX_BUFFER_SIZE 128

/* --- Helpers for UI --- */
static void clear_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main(void) {
    /* 1. Open or Create the configuration file */
    int fd = open(CONFIG_FILE_PATH, O_RDWR | O_CREAT, CONFIG_FILE_PERMS);
    if (fd < 0) {
        perror("[Writer] open failed");
        exit(EXIT_FAILURE);
    }

    /* Check if the file is newly created to initialize defaults */
    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("[Writer] fstat failed");
        close(fd);
        exit(EXIT_FAILURE);
    }
    int is_new_file = (st.st_size == 0);

    /* 2. Truncate file to exact size of our struct */
    if (ftruncate(fd, sizeof(device_cfg_t)) == -1) {
        perror("[Writer] ftruncate failed");
        close(fd);
        exit(EXIT_FAILURE);
    }

    /* 3. Memory Map the file */
    device_cfg_t *cfg = mmap(NULL, sizeof(device_cfg_t), 
                             PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (cfg == MAP_FAILED) {
        perror("[Writer] mmap failed");
        close(fd);
        exit(EXIT_FAILURE);
    }

    /* Close the file descriptor, we only need the mmap pointer now */
    close(fd);

    /* 4. Initialize defaults if file was just created */
    if (is_new_file || cfg->baud_rate == 0) {
        cfg->baud_rate        = DEFAULT_BAUD_RATE;
        cfg->sampling_rate_hz = DEFAULT_SAMPLING_HZ;
        cfg->log_level        = DEFAULT_LOG_LEVEL;
        if (msync(cfg, sizeof(device_cfg_t), MS_SYNC) == -1) {
            perror("[Writer] initial msync failed");
        }
    }

    printf("[Config Writer] Loaded %s\n", CONFIG_FILE_PATH);

    /* 5. Interactive Menu Loop */
    char input[MAX_BUFFER_SIZE];
    while (1) {
        printf("\nCurrent: baud_rate=%d sampling_rate=%d log_level=%d\n", 
               cfg->baud_rate, cfg->sampling_rate_hz, cfg->log_level);
        printf("Select field to update [baud/rate/log/quit]: ");
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\r\n")] = '\0';

        if (strcmp(input, "quit") == 0) {
            break;
        }

        if (strcmp(input, "baud") == 0) {
            printf("Select baud rate [9600/115200/460800]: ");
            int new_baud;
            if (scanf("%d", &new_baud) == 1) {
                if (new_baud == 9600 || new_baud == 115200 || new_baud == 460800) {
                    cfg->baud_rate = new_baud;
                    msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] baud_rate = %d\n", new_baud);
                } else {
                    printf("Invalid baud rate!\n");
                }
            }
            clear_stdin();
        } 
        else if (strcmp(input, "rate") == 0) {
            printf("Enter sampling rate (1-1000): ");
            int new_rate;
            if (scanf("%d", &new_rate) == 1 && new_rate >= 1 && new_rate <= 1000) {
                cfg->sampling_rate_hz = new_rate;
                msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                printf("[Updated] sampling_rate_hz = %d\n", new_rate);
            } else {
                printf("Invalid sampling rate!\n");
            }
            clear_stdin();
        } 
        else if (strcmp(input, "log") == 0) {
            printf("Select log level [0=OFF, 1=ERROR, 2=INFO, 3=DEBUG]: ");
            int new_log;
            if (scanf("%d", &new_log) == 1 && new_log >= 0 && new_log <= 3) {
                cfg->log_level = new_log;
                msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                printf("[Updated] log_level = %d\n", new_log);
            } else {
                printf("Invalid log level!\n");
            }
            clear_stdin();
        } 
        else if (strlen(input) > 0) {
            printf("Unknown option.\n");
        }
    }

    /* 6. Cleanup */
    if (munmap(cfg, sizeof(device_cfg_t)) == -1) {
        perror("[Writer] munmap failed");
    }

    printf("[Config Writer] Exiting.\n");
    return EXIT_SUCCESS;
}