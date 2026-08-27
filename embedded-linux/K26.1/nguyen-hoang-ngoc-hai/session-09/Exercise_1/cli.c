#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "sensor_shm.h"

int main(void) {
    int shmid = shmget(SHM_KEY, sizeof(sensor_data_t), 0666);

    if (shmid == -1) {
        printf("Daemon is not running.\n");
        exit(1);
    }

    sensor_data_t *shm_ptr = (sensor_data_t *)shmat(shmid, NULL, 0);

    if (shm_ptr == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    printf("[Sensor Report]\n");
    printf("Timestamp : %ld\n", shm_ptr->timestamp);
    printf("CPU Temp  : %.2f C\n", shm_ptr->cpu_temp);
    printf("RAM Used  : %.2f %%\n", shm_ptr->ram_used_pct);

    if (shmdt(shm_ptr) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}