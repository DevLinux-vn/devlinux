#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "device_state.h"

int main(void) {
    setbuf(stdout, NULL);

    /* Khởi tạo phân vùng nhớ POSIX với quyền ghi cấu hình hệ thống */
    int shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
    if (shm_fd < 0) {
        perror("CRITICAL: shm_open failed");
        return EXIT_FAILURE;
    }

    if (ftruncate(shm_fd, sizeof(device_state_t)) < 0) {
        perror("CRITICAL: ftruncate failed");
        close(shm_fd);
        return EXIT_FAILURE;
    }

    device_state_t *state = (device_state_t *)mmap(NULL, sizeof(device_state_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (state == MAP_FAILED) {
        perror("CRITICAL: mmap failed");
        close(shm_fd);
        return EXIT_FAILURE;
    }
    close(shm_fd);

    /* --- CẤU HÌNH KHÓA MUTEX DÙNG CHUNG ĐA TIẾN TRÌNH (PROCESS-SHARED) --- */
    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0) {
        fprintf(stderr, "CRITICAL: Mutex attribute init failed\n");
        return EXIT_FAILURE;
    }
    /* Ép khóa nhận diện xuyên biên giới tiến trình */
    if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
        fprintf(stderr, "CRITICAL: Failed to set process shared attribute\n");
        pthread_mutexattr_destroy(&attr);
        return EXIT_FAILURE;
    }
    if (pthread_mutex_init(&state->mutex, &attr) != 0) {
        fprintf(stderr, "CRITICAL: Mutex initialization failed\n");
        pthread_mutexattr_destroy(&attr);
        return EXIT_FAILURE;
    }
    pthread_mutexattr_destroy(&attr); /* Giải phóng thuộc tính đệm */

    state->status = 0; /* Giá trị mặc định lúc khởi động máy máy: OFF */
    printf("[Controller] Shared memory ready. Commands: on / off / quit\n");

    char cmd[32];
    while (1) {
        printf("> ");
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) break;
        cmd[strcspn(cmd, "\r\n")] = '\0';

        if (strcmp(cmd, "quit") == 0) {
            break;
        } else if (strcmp(cmd, "on") == 0) {
            pthread_mutex_lock(&state->mutex);
            state->status = 1;
            pthread_mutex_unlock(&state->mutex);
            printf("[Controller] Command sent: ON\n");
        } else if (strcmp(cmd, "off") == 0) {
            pthread_mutex_lock(&state->mutex);
            state->status = 0;
            pthread_mutex_unlock(&state->mutex);
            printf("[Controller] Command sent: OFF\n");
        } else {
            printf("Invalid command. Use 'on', 'off', or 'quit'.\n");
        }
    }

    /* Quy trình phá hủy và gỡ bỏ hoàn toàn tài nguyên */
    printf("[Controller] Cleaning up. Goodbye.\n");
    pthread_mutex_destroy(&state->mutex);
    munmap(state, sizeof(device_state_t));
    shm_unlink(SHM_NAME);

    return EXIT_SUCCESS;
}