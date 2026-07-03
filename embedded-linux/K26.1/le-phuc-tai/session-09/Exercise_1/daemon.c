#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <errno.h>
#include "sensor_shm.h"

#define INTERVAL_SEC 2

volatile sig_atomic_t g_keep_running = 1;

static void handle_sigint(int signum) {
    (void)signum;
    g_keep_running = 0;
}

static void read_system_metrics(double *temp_out, double *ram_out) {
    double load1 = 0.0;
    FILE *f_cpu = fopen("/proc/loadavg", "r");
    if (f_cpu != NULL) {
        if (fscanf(f_cpu, "%lf", &load1) != 1) load1 = 0.0;
        fclose(f_cpu);
    }
    *temp_out = 40.0 + (load1 * 10.0);

    long mem_total = 1, mem_free = 0;
    char line[128];
    FILE *f_mem = fopen("/proc/meminfo", "r");
    if (f_mem != NULL) {
        while (fgets(line, sizeof(line), f_mem)) {
            if (strncmp(line, "MemTotal:", 9) == 0) {
                sscanf(line, "MemTotal: %ld", &mem_total);
            } else if (strncmp(line, "MemFree:", 8) == 0) {
                sscanf(line, "MemFree: %ld", &mem_free);
            }
        }
        fclose(f_mem);
    }
    *ram_out = (double)(mem_total - mem_free) / (double)mem_total * 100.0;
}

int main(void) {
    setbuf(stdout, NULL);

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("CRITICAL: Failed to register SIGINT");
        return EXIT_FAILURE;
    }

    int shmid = shmget(SHM_KEY, sizeof(sensor_data_t), IPC_CREAT | 0666);
    if (shmid < 0) {
        /* Enterprise-grade detailed errno diagnostics */
        if (errno == EACCES) {
            fprintf(stderr, "CRITICAL: Permission denied (EACCES) for SHM Key 0x%x.\n", SHM_KEY);
        } else if (errno == EEXIST) {
            fprintf(stderr, "CRITICAL: Key conflict (EEXIST). Segment already exists.\n");
        } else if (errno == ENOMEM) {
            fprintf(stderr, "CRITICAL: Insufficient core memory (ENOMEM) to allocate SHM.\n");
        } else {
            perror("CRITICAL: shmget initialization failed");
        }
        return EXIT_FAILURE;
    }
    printf("[Daemon] Shared memory created. Key=0x%x\n", SHM_KEY);

    sensor_data_t *shm_ptr = (sensor_data_t *)shmat(shmid, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("CRITICAL: shmat link failed");
        return EXIT_FAILURE;
    }

    while (g_keep_running) {
        double temp = 0.0, ram_pct = 0.0;
        read_system_metrics(&temp, &ram_pct);

        shm_ptr->timestamp = time(NULL);
        shm_ptr->cpu_temp = temp;
        shm_ptr->ram_used_pct = ram_pct;

        printf("[Daemon] Written: temp=%.2f C ram=%.2f%%\n", temp, ram_pct);
        sleep(INTERVAL_SEC);
    }

    printf("\n[Daemon] Cleaning up shared memory. Goodbye.\n");
    if (shmdt(shm_ptr) < 0) perror("ERROR: shmdt failed");
    if (shmctl(shmid, IPC_RMID, NULL) < 0) perror("ERROR: shmctl IPC_RMID failed");

    return EXIT_SUCCESS;
}