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

static void print_current(const device_cfg_t *cfg)
{
    printf("Current: baud_rate=%d sampling_rate=%d log_level=%d\n",
           cfg->baud_rate, cfg->sampling_rate_hz, cfg->log_level);
}

static int read_line(char *buf, size_t bufsize)
{
    if (fgets(buf, (int)bufsize, stdin) == NULL) {
        return -1;
    }
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }
    return 0;
}

static void update_baud(device_cfg_t *cfg)
{
    char line[64];
    printf("Select baud rate [9600/115200/460800]: ");
    fflush(stdout);
    if (read_line(line, sizeof(line)) == -1) {
        return;
    }

    int value = atoi(line);
    if (value != 9600 && value != 115200 && value != 460800) {
        fprintf(stderr, "[Writer] Invalid baud rate: %s\n", line);
        return;
    }

    cfg->baud_rate = value;

    if (msync(cfg, sizeof(*cfg), MS_SYNC) == -1) {
        perror("[Writer] msync");
        return;
    }

    printf("[Updated] baud_rate = %d\n", value);
}

static void update_sampling(device_cfg_t *cfg)
{
    char line[64];
    printf("Enter sampling rate (1-1000 Hz): ");
    fflush(stdout);
    if (read_line(line, sizeof(line)) == -1) {
        return;
    }

    int value = atoi(line);
    if (value < 1 || value > 1000) {
        fprintf(stderr, "[Writer] Invalid sampling rate: %s\n", line);
        return;
    }

    cfg->sampling_rate_hz = value;

    if (msync(cfg, sizeof(*cfg), MS_SYNC) == -1) {
        perror("[Writer] msync");
        return;
    }

    printf("[Updated] sampling_rate_hz = %d\n", value);
}

static void update_log(device_cfg_t *cfg)
{
    char line[64];
    printf("Select log level [0=OFF/1=ERROR/2=INFO/3=DEBUG]: ");
    fflush(stdout);
    if (read_line(line, sizeof(line)) == -1) {
        return;
    }

    int value = atoi(line);
    if (value < 0 || value > 3) {
        fprintf(stderr, "[Writer] Invalid log level: %s\n", line);
        return;
    }

    cfg->log_level = value;

    if (msync(cfg, sizeof(*cfg), MS_SYNC) == -1) {
        perror("[Writer] msync");
        return;
    }

    printf("[Updated] log_level = %d (%s)\n", value, log_level_name(value));
}

int main(void)
{
    int fd = open(CFG_FILE_PATH, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("[Writer] open");
        exit(EXIT_FAILURE);
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("[Writer] fstat");
        close(fd);
        exit(EXIT_FAILURE);
    }
    int is_new = (st.st_size < (off_t)sizeof(device_cfg_t));

    if (ftruncate(fd, sizeof(device_cfg_t)) == -1) {
        perror("[Writer] ftruncate");
        close(fd);
        exit(EXIT_FAILURE);
    }

    device_cfg_t *cfg = mmap(NULL, sizeof(device_cfg_t),
                              PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (cfg == MAP_FAILED) {
        perror("[Writer] mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd); 

    if (is_new) {
        cfg->baud_rate = 9600;
        cfg->sampling_rate_hz = 100;
        cfg->log_level = 2; 
        if (msync(cfg, sizeof(*cfg), MS_SYNC) == -1) {
            perror("[Writer] msync (init)");
        }
    }

    printf("[Config Writer] Loaded %s\n", CFG_FILE_PATH);
    print_current(cfg);

    char line[64];
    while (1) {
        printf("\nSelect field to update [baud/rate/log/quit]: ");
        fflush(stdout);

        if (read_line(line, sizeof(line)) == -1) {
            break; 
        }

        if (strcmp(line, "quit") == 0) {
            break;
        } else if (strcmp(line, "baud") == 0) {
            update_baud(cfg);
        } else if (strcmp(line, "rate") == 0) {
            update_sampling(cfg);
        } else if (strcmp(line, "log") == 0) {
            update_log(cfg);
        } else {
            fprintf(stderr, "[Writer] Unknown field: %s\n", line);
        }
    }

    if (munmap(cfg, sizeof(device_cfg_t)) == -1) {
        perror("[Writer] munmap");
        exit(EXIT_FAILURE);
    }

    printf("[Writer] Cleaning up. Goodbye.\n");
    return 0;
}
