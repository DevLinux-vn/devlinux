#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "device_cfg.h"

static const char *log_level_to_string(int level)
{
    switch (level) {
    case 0:
        return "OFF";
    case 1:
        return "ERROR";
    case 2:
        return "INFO";
    case 3:
        return "DEBUG";
    default:
        return "UNKNOWN";
    }
}

static void print_config(const device_cfg_t *cfg)
{
    printf(
        "Current: baud_rate=%d sampling_rate=%d log_level=%d (%s)\n",
        cfg->baud_rate,
        cfg->sampling_rate_hz,
        cfg->log_level,
        log_level_to_string(cfg->log_level)
    );
}

static int update_config(device_cfg_t *cfg, const char *field)
{
    int value;

    if (strcmp(field, "baud") == 0) {

        printf("Select baud rate [9600/115200/460800]: ");

        if (scanf("%d", &value) != 1) {
            printf("Invalid input.\n");
            while (getchar() != '\n') {
                /* discard invalid input */
            }
            return -1;
        }

        if (value != 9600 &&
            value != 115200 &&
            value != 460800) {

            printf("Invalid baud rate.\n");
            return -1;
        }

        cfg->baud_rate = value;

        printf("[Updated] baud_rate = %d\n", cfg->baud_rate);
    }
    else if (strcmp(field, "rate") == 0) {

        printf("Enter sampling rate [1-1000 Hz]: ");

        if (scanf("%d", &value) != 1) {
            printf("Invalid input.\n");
            while (getchar() != '\n') {
                /* discard invalid input */
            }
            return -1;
        }

        if (value < 1 || value > 1000) {
            printf("Invalid sampling rate.\n");
            return -1;
        }

        cfg->sampling_rate_hz = value;

        printf(
            "[Updated] sampling_rate_hz = %d\n",
            cfg->sampling_rate_hz
        );
    }
    else if (strcmp(field, "log") == 0) {

        printf("Select log level [0=OFF 1=ERROR 2=INFO 3=DEBUG]: ");

        if (scanf("%d", &value) != 1) {
            printf("Invalid input.\n");
            while (getchar() != '\n') {
                /* discard invalid input */
            }
            return -1;
        }

        if (value < 0 || value > 3) {
            printf("Invalid log level.\n");
            return -1;
        }

        cfg->log_level = value;

        printf(
            "[Updated] log_level = %d (%s)\n",
            cfg->log_level,
            log_level_to_string(cfg->log_level)
        );
    }
    else {
        printf("Unknown field. Use: baud, rate, log, quit\n");
        return -1;
    }

    return 0;
}

int main(void)
{
    int fd;
    device_cfg_t *cfg;
    char command[32];

    /*
     * 1. Open or create configuration file
     */
    fd = open(
        CONFIG_FILE,
        O_RDWR | O_CREAT,
        0666
    );

    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    /*
     * 2. Make sure the file is large enough
     *    to contain device_cfg_t
     */
    if (ftruncate(fd, sizeof(device_cfg_t)) == -1) {
        perror("ftruncate");
        close(fd);
        return EXIT_FAILURE;
    }

    /*
     * 3. Map file into memory
     */
    cfg = mmap(
        NULL,
        sizeof(device_cfg_t),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0
    );

    if (cfg == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    /*
     * fd can be closed after successful mmap()
     */
    if (close(fd) == -1) {
        perror("close");
        munmap(cfg, sizeof(device_cfg_t));
        return EXIT_FAILURE;
    }

    /*
     * If the configuration file was newly created,
     * initialize default values.
     */
    if (cfg->baud_rate != 9600 &&
        cfg->baud_rate != 115200 &&
        cfg->baud_rate != 460800) {

        cfg->baud_rate = 9600;
        cfg->sampling_rate_hz = 100;
        cfg->log_level = 2;

        if (msync(
                cfg,
                sizeof(device_cfg_t),
                MS_SYNC
            ) == -1) {

            perror("msync");
            munmap(cfg, sizeof(device_cfg_t));
            return EXIT_FAILURE;
        }
    }

    printf("[Config Writer] Loaded %s\n", CONFIG_FILE);
    print_config(cfg);

    while (1) {

        printf(
            "\nSelect field to update [baud/rate/log/quit]: "
        );

        if (scanf("%31s", command) != 1) {
            printf("Failed to read command.\n");
            break;
        }

        if (strcmp(command, "quit") == 0) {
            break;
        }

        if (update_config(cfg, command) == 0) {

            /*
             * Ensure the mmap changes are
             * synchronized with the file.
             */
            if (msync(
                    cfg,
                    sizeof(device_cfg_t),
                    MS_SYNC
                ) == -1) {

                perror("msync");
                break;
            }
        }
    }

    /*
     * 4. Unmap memory
     */
    if (munmap(cfg, sizeof(device_cfg_t)) == -1) {
        perror("munmap");
        return EXIT_FAILURE;
    }

    printf("[Config Writer] Exiting.\n");

    return EXIT_SUCCESS;
}
