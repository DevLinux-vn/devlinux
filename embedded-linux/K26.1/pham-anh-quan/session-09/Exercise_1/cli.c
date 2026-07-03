#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "sensor_shm.h"

int main() {
    int shmid = shmget(SHM_KEY, sizeof(sensor_data_t), 0666);
    if (shmid == -1) {
        fprintf(stderr, "Daemon is not running.\n");
        exit(1);
    }

    sensor_data_t *shm_ptr = (sensor_data_t *)shmat(shmid, NULL, 0);
    if (shm_ptr == (sensor_data_t *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    printf("[Sensor Report]\n");
    printf("Timestamp : %ld\n", (long)shm_ptr->timestamp);
    printf("CPU Temp  : %.2f C\n", shm_ptr->cpu_temp);
    printf("RAM Used  : %.2f %%\n", shm_ptr->ram_used_pct);

    shmdt(shm_ptr);
    return 0;
}