#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "sensor_shm.h"

int main() {
    // Try to access existing shared memory (no IPC_CREAT)
    int shm_id = shmget(SHM_KEY, sizeof(sensor_data_t), 0666);
    if (shm_id == -1) {
        printf("[Sensor-CLI] Daemon is not running.\n");
        exit(1);
    }
    
    // Attach to shared memory
    sensor_data_t *sensor_data = (sensor_data_t *)shmat(shm_id, NULL, 0);
    if (sensor_data == (void *)-1) {
        perror("shmat");
        exit(1);
    }
    
    // Read and print data
    printf("[Sensor Report]\n");
    printf("Timestamp : %ld\n", sensor_data->timestamp);
    printf("CPU Temp  : %.2f C\n", sensor_data->cpu_temp);
    printf("RAM Used  : %.2f %%\n", sensor_data->ram_used_pct);
    
    // Detach
    if (shmdt(sensor_data) == -1) {
        perror("shmdt");
    }
    
    return 0;
}
