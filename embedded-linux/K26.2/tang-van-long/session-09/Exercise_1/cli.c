#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "sensor_shm.h"

int main(void)
{
    int shm_id = shmget(SHM_KEY, sizeof(sensor_data_t), 0666);
    if (shm_id == -1) {
        fprintf(stderr, "Daemon is not running.\n");
        exit(EXIT_FAILURE);
    }

    sensor_data_t *shm_ptr = (sensor_data_t *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("[CLI] shmat");
        exit(EXIT_FAILURE);
    }

    sensor_data_t data = *shm_ptr;

    if (shmdt(shm_ptr) == -1) {
        perror("[CLI] shmdt");
    }

    printf("[Sensor Report]\n");
    printf("Timestamp : %ld\n", (long)data.timestamp);
    printf("CPU Temp  : %.2f C\n", data.cpu_temp);
    printf("RAM Used  : %.2f %%\n", data.ram_used_pct);

    return 0;
}
