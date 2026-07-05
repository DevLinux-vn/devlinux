/**
 * @file cli.c
 * @brief CLI application to read Sensor Data from SystemV Shared Memory
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "sensor_shm.h"

int main(void) {
    /* 1. Locate existing shared memory (No IPC_CREAT) */
    int shm_id = shmget(SHM_KEY, sizeof(sensor_data_t), 0666);
    if (shm_id < 0) {
        fprintf(stderr, "Daemon is not running.\n");
        exit(EXIT_FAILURE);
    }

    /* 2. Attach to virtual address space */
    sensor_data_t *shared_data = (sensor_data_t *)shmat(shm_id, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    /* 3. Output the report exactly as requested */
    printf("[Sensor Report]\n");
    printf("Timestamp : %ld\n", (long)shared_data->timestamp);
    printf("CPU Temp  : %.2f C\n", shared_data->cpu_temp);
    printf("RAM Used  : %.2f %%\n", shared_data->ram_used_pct);

    /* 4. Detach from memory space (Does NOT destroy the segment) */
    if (shmdt(shared_data) == -1) {
        perror("shmdt failed");
    }

    return EXIT_SUCCESS;
}