#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#include "sensor_shm.h"

static volatile sig_atomic_t running = 1;

static void handle_sigint(int sig)
{
    (void)sig;
    running = 0;
}

static int read_cpu_load(double *load1)
{
    FILE *fp;

    if (load1 == NULL) {
        return -1;
    }

    fp = fopen("/proc/loadavg", "r");
    if (fp == NULL) {
        perror("fopen /proc/loadavg");
        return -1;
    }

    if (fscanf(fp, "%lf", load1) != 1) {
        fprintf(stderr, "Failed to read CPU load\n");
        fclose(fp);
        return -1;
    }

    fclose(fp);

    return 0;
}

static int read_ram_usage(double *ram_used_pct)
{
    FILE *fp;
    char line[256];

    unsigned long mem_total = 0;
    unsigned long mem_free = 0;

    if (ram_used_pct == NULL) {
        return -1;
    }

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        perror("fopen /proc/meminfo");
        return -1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (sscanf(line, "MemTotal: %lu kB", &mem_total) == 1) {
            continue;
        }

        if (sscanf(line, "MemFree: %lu kB", &mem_free) == 1) {
            continue;
        }

        if (mem_total != 0 && mem_free != 0) {
            break;
        }
    }

    fclose(fp);

    if (mem_total == 0) {
        fprintf(stderr, "Failed to read MemTotal\n");
        return -1;
    }

    *ram_used_pct =
        ((double)(mem_total - mem_free) / (double)mem_total) * 100.0;

    return 0;
}

int main(void)
{
    int shmid;
    sensor_data_t *data;

    double load1;
    double ram_used_pct;

    signal(SIGINT, handle_sigint);

    shmid = shmget(
        SHM_KEY,
        sizeof(sensor_data_t),
        IPC_CREAT | 0666
    );

    if (shmid == -1) {
        perror("shmget");
        return EXIT_FAILURE;
    }

    data = shmat(shmid, NULL, 0);

    if (data == (void *)-1) {
        perror("shmat");
        return EXIT_FAILURE;
    }

    printf("[Daemon] Shared memory created. Key=0x%x\n", SHM_KEY);

    while (running) {
        if (read_cpu_load(&load1) == -1) {
            break;
        }

        if (read_ram_usage(&ram_used_pct) == -1) {
            break;
        }

        data->timestamp = time(NULL);
        data->cpu_temp = 40.0 + load1 * 10.0;
        data->ram_used_pct = ram_used_pct;

        printf(
            "[Daemon] Written: temp=%.2f ram=%.2f%%\n",
            data->cpu_temp,
            data->ram_used_pct
        );

        sleep(2);
    }

    printf("[Daemon] Cleaning up shared memory. Goodbye.\n");

    if (shmdt(data) == -1) {
        perror("shmdt");
        return EXIT_FAILURE;
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
