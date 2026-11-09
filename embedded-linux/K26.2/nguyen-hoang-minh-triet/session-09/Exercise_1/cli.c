#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "sensor_shm.h"

int main(void) {
    /* 1. Get Shared Memory*/
    int shm_id = shmget(SHM_KEY, sizeof(sensor_data_t), SHM_PERMISSIONS);
    if (shm_id < 0) {
        if (errno == ENOENT) {
            fprintf(stderr, "Daemon is not running (Shared memory not found).\n");
        } else {
            perror("[CLI] shmget failed");
        }
        exit(EXIT_FAILURE);
    }

    /* 2. Attach to Shared Memory */
    sensor_data_t *shm_ptr = (sensor_data_t *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("[CLI] shmat failed");
        exit(EXIT_FAILURE);
    }

    /* 3. Read Data */
    printf("[Sensor Report]\n");
    printf("Timestamp : %ld\n", (long)shm_ptr->timestamp);
    printf("CPU Temp  : %.2f C\n", shm_ptr->cpu_temp);
    printf("RAM Used  : %.2f %%\n", shm_ptr->ram_used_pct);

    /* 4. Detach from Shared Memory */
    if (shmdt(shm_ptr) < 0) {
        perror("[CLI] shmdt failed");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
