#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <errno.h>
#include "sensor_shm.h"

int main(void) {
    setbuf(stdout, NULL);

    /* Thử kết nối vào vùng nhớ của daemon, tuyệt đối không dùng cờ IPC_CREAT */
    int shmid = shmget(SHM_KEY, sizeof(sensor_data_t), 0666);
    if (shmid < 0) {
        if (errno == ENOENT) {
            fprintf(stderr, "Daemon is not running.\n");
        } else {
            perror("ERROR: shmget failed");
        }
        return EXIT_FAILURE; /* Thoát với mã lỗi 1 theo đặc tả yêu cầu */
    }

    sensor_data_t *shm_ptr = (sensor_data_t *)shmat(shmid, NULL, SHM_RDONLY);
    if (shm_ptr == (void *)-1) {
        perror("CRITICAL: cli shmat failed");
        return EXIT_FAILURE;
    }

    /* Đọc dữ liệu ra stdout một lần duy nhất */
    printf("[Sensor Report]\n");
    printf("Timestamp : %ld\n", (long)shm_ptr->timestamp);
    printf("CPU Temp  : %.2f C\n", shm_ptr->cpu_temp);
    printf("RAM Used  : %.2f %%\n", shm_ptr->ram_used_pct);

    if (shmdt(shm_ptr) < 0) {
        perror("WARNING: cli shmdt failed");
    }

    return EXIT_SUCCESS;
}