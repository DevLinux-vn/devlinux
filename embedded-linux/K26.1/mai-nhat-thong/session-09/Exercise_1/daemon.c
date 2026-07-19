/**
 * @file daemon.c
 * @brief Sensor background daemon using SystemV Shared Memory
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "sensor_shm.h"

static volatile sig_atomic_t keep_running = 1;
static int shm_id = -1;
static sensor_data_t *shared_data = NULL;

/**
 * @brief Asynchronous signal safe handler for SIGINT
 */
void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
}

/**
 * @brief Reads /proc/loadavg to compute mock CPU temperature
 */
double read_cpu_temp() {
    FILE *f = fopen("/proc/loadavg", "r");
    if (!f) return 40.0; /* Fallback safe value */
    
    float load1;
    if (fscanf(f, "%f", &load1) != 1) load1 = 0.0;
    fclose(f);
    
    return 40.0 + (load1 * 10.0);
}

/**
 * @brief Reads /proc/meminfo to compute RAM usage percentage
 */
double read_ram_usage() {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) return 0.0;

    char line[128];
    long mem_total = 0, mem_free = 0;
    
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "MemTotal:", 9) == 0) sscanf(line, "MemTotal: %ld kB", &mem_total);
        else if (strncmp(line, "MemFree:", 8) == 0) sscanf(line, "MemFree: %ld kB", &mem_free);
    }
    fclose(f);

    if (mem_total == 0) return 0.0;
    return ((double)(mem_total - mem_free) / mem_total) * 100.0;
}

int main(void) {
    /* Prevent log buffering */
    setbuf(stdout, NULL);

    /* Setup Graceful Shutdown via SIGINT */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    /* 1. Allocate SystemV Shared Memory segment */
    shm_id = shmget(SHM_KEY, sizeof(sensor_data_t), IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("[Daemon] shmget failed");
        exit(EXIT_FAILURE);
    }

    /* 2. Attach shared memory to the daemon's virtual address space */
    shared_data = (sensor_data_t *)shmat(shm_id, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("[Daemon] shmat failed");
        exit(EXIT_FAILURE);
    }

    printf("[Daemon] Shared memory created. Key=0x%X\n", SHM_KEY);

    /* 3. Main Data Collection Loop */
    while (keep_running) {
        shared_data->cpu_temp = read_cpu_temp();
        shared_data->ram_used_pct = read_ram_usage();
        shared_data->timestamp = time(NULL);

        printf("[Daemon] Written: temp=%.2f ram=%.2f%%\n", 
               shared_data->cpu_temp, shared_data->ram_used_pct);

        /* Sleep safely, allowing interruption by signals */
        unsigned int rem = 2;
        while (rem > 0 && keep_running) {
            rem = sleep(rem);
        }
    }

    /* 4. Cleanup Sequence */
    printf("\n[Daemon] Cleaning up shared memory. Goodbye.\n");
    if (shmdt(shared_data) == -1) perror("shmdt failed");
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) perror("shmctl failed");

    return EXIT_SUCCESS;
}