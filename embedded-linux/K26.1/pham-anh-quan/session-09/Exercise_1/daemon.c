#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "sensor_shm.h"

int shmid = -1;
sensor_data_t *shm_ptr = (sensor_data_t *)-1;

void handle_sigint(int sig) {
    (void)sig;
    printf("\n[Daemon] cleaning up shared memory. Goodbye.\n");
    if (shm_ptr != (sensor_data_t *)-1) {
        shmdt(shm_ptr);
    }
    if (shmid != -1) {
        shmctl(shmid, IPC_RMID, NULL);
    }
    exit(0);
}

void get_system_metrics(double *temp, double *mem_used_pct) {
    FILE *f_load = fopen("/proc/loadavg", "r");
    double load1 = 0.0;
    if (f_load) {
        if (fscanf(f_load, "%lf", &load1) != 1) {
            load1 = 0.0;
        }
        fclose(f_load);
    }
    *temp = 40.0 + load1 * 10.0;

    FILE *f_mem = fopen("/proc/meminfo", "r");
    long long mem_total = 0, mem_free = 0;
    if (f_mem) {
        char line[128];
        while (fgets(line, sizeof(line), f_mem)) {
            if (sscanf(line, "MemTotal: %lld kB", &mem_total) == 1) continue;
            if (sscanf(line, "MemFree: %lld kB", &mem_free) == 1) continue;
        }
        fclose(f_mem);
    }
    if (mem_total > 0) {
        *mem_used_pct = (double)(mem_total - mem_free) / mem_total * 100.0;
    } else {
        *mem_used_pct = 0.0;
    }
}

int main() {
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    shmid = shmget(SHM_KEY, sizeof(sensor_data_t), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
    printf("[Daemon] Shared memory created. Key=0x%x\n", SHM_KEY);

    shm_ptr = (sensor_data_t *)shmat(shmid, NULL, 0);
    if (shm_ptr == (sensor_data_t *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        double temp = 0.0, ram_pct = 0.0;
        get_system_metrics(&temp, &ram_pct);

        shm_ptr->timestamp = time(NULL);
        shm_ptr->cpu_temp = temp;
        shm_ptr->ram_used_pct = ram_pct;

        printf("[Daemon] Written: temp=%.2f ram=%.2f%%\n", temp, ram_pct);
        sleep(2);
    }

    return 0;
}