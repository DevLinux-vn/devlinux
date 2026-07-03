/**
 * @file cli.c
 * @brief Client consumer outputting snapshot reports from SystemV memory segment.
 * @date 2026-07-04
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <errno.h>
#include "sensor_shm.h"

int main(void) {
    setbuf(stdout, NULL);

    int shmid = shmget(SHM_KEY, sizeof(sensor_data_t), 0666);
    if (shmid < 0) {
        /* Explicit validation for structural daemon status via exact errno mapping */
        if (errno == ENOENT) {
            fprintf(stderr, "Daemon is not running.\n");
        } else {
            perror("ERROR: Unexpected shmget failure");
        }
        return EXIT_FAILURE;
    }

    sensor_data_t *shm_ptr = (sensor_data_t *)shmat(shmid, NULL, SHM_RDONLY);
    if (shm_ptr == (void *)-1) {
        perror("CRITICAL: CLI memory attach failed");
        return EXIT_FAILURE;
    }

    printf("[Sensor Report]\n");
    printf("Timestamp : %ld\n", (long)shm_ptr->timestamp);
    printf("CPU Temp  : %.2f C\n", shm_ptr->cpu_temp);
    printf("RAM Used  : %.2f %%\n", shm_ptr->ram_used_pct);

    if (shmdt(shm_ptr) < 0) perror("WARNING: CLI shmdt detachment failed");

    return EXIT_SUCCESS;
}