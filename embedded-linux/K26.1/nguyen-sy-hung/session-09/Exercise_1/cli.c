#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include "sensor_shm.h"

static int attach_shared_memory(int *shmid,
                                sensor_data_t **sensor_data)
{
    *shmid = shmget(SHM_KEY,
                    sizeof(sensor_data_t),
                    SHM_PERMISSION);

    if (*shmid == -1)
    {
        perror("shmget");
        fprintf(stderr, "Daemon is not running.\n");
        return -1;
    }

    *sensor_data = (sensor_data_t *)shmat(*shmid, NULL, SHM_RDONLY);

    if (*sensor_data == (void *)-1)
    {
        perror("shmat");
        *sensor_data = NULL;
        return -1;
    }

    return 0;
}

/*------------------------------------------------------------------*/

static void print_report(const sensor_data_t *sensor_data)
{
    printf("\n");
    printf("[Sensor Report]\n");
    printf("Timestamp : %ld\n", (long)sensor_data->timestamp);
    printf("CPU Temp  : %.2f C\n", sensor_data->cpu_temp);
    printf("RAM Used  : %.2f %%\n", sensor_data->ram_used_pct);
}

/*------------------------------------------------------------------*/

int main(void)
{
    int shmid;
    sensor_data_t *sensor_data;

    if (attach_shared_memory(&shmid,
                             &sensor_data) == -1)
    {
        return EXIT_FAILURE;
    }

    print_report(sensor_data);

    if (shmdt(sensor_data) == -1)
    {
        perror("shmdt");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}