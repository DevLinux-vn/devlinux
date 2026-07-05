#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include "sensor_shm.h"
#include <stdint.h>

static int shmid = -1;
static sensor_data_t *shm_ptr = NULL;

void cleanup(int sig) {
    (void)sig;

    printf("\n[Daemon] Cleaning up shared memory. Goodbye.\n");
    fflush(stdout);
    if (shm_ptr != NULL) {
        if (shmdt(shm_ptr) == -1) {
            perror("shmdt");
        }
    }

    if (shmid != -1) {
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl IPC_RMID");
        }
    }

    exit(0);
}

double read_cpu_temp(void) {
    FILE *fp = fopen("/proc/loadavg", "r");
    if (fp == NULL) {
        perror("fopen /proc/loadavg");
        return 0.0;
    }

    double load1 = 0.0;
    if (fscanf(fp, "%lf", &load1) != 1) {
        perror("fscanf /proc/loadavg");
        fclose(fp);
        return 0.0;
    }

    fclose(fp);
    return 40.0 + load1 * 10.0;
}

double read_ram_used_pct(void) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        perror("fopen /proc/meminfo");
        return 0.0;
    }

    char key[64];
    long value;
    char unit[32];

    long mem_total = 0;
    long mem_free = 0;

    while (fscanf(fp, "%63s %ld %31s", key, &value, unit) == 3) {
        if (strcmp(key, "MemTotal:") == 0) {
            mem_total = value;
        } else if (strcmp(key, "MemFree:") == 0) {
            mem_free = value;
        }

        if (mem_total > 0 && mem_free > 0) {
            break;
        }
    }

    fclose(fp);

    if (mem_total <= 0 || mem_free < 0) {
        fprintf(stderr, "Warning: incomplete /proc/meminfo\n");
        return 0.0;
    }

    return ((double)(mem_total - mem_free) / mem_total) * 100.0;
}

int main(void) {
    signal(SIGINT, cleanup);

    shmid = shmget(SHM_KEY, sizeof(sensor_data_t), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    shm_ptr = (sensor_data_t *)shmat(shmid, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    printf("[Daemon] Shared memory created. Key=0x%x\n", SHM_KEY);

    while (1) {
        shm_ptr->timestamp = time(NULL);
        shm_ptr->cpu_temp = read_cpu_temp();
        shm_ptr->ram_used_pct = read_ram_used_pct();

        printf("[Daemon] Written: temp=%.2f ram=%.2f%%\n",
               shm_ptr->cpu_temp,
               shm_ptr->ram_used_pct);

        sleep(2);
    }

    return 0;
}