/**
 * @file controller.c
 * @brief Industrial controller command line managing state switches via atomic locks.
 * @date 2026-07-04
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "device_state.h"

int main(void) {
    setbuf(stdout, NULL);

    int shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
    if (shm_fd < 0) {
        perror("CRITICAL: POSIX memory layer allocation failed");
        return EXIT_FAILURE;
    }

    if (ftruncate(shm_fd, sizeof(device_state_t)) < 0) {
        perror("CRITICAL: POSIX memory segment sizing failed");
        close(shm_fd);
        return EXIT_FAILURE;
    }

    device_state_t *state = (device_state_t *)mmap(NULL, sizeof(device_state_t),
                                                   PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (state == MAP_FAILED) {
        perror("CRITICAL: Core virtual mmap failed");
        close(shm_fd);
        return EXIT_FAILURE;
    }
    close(shm_fd);

    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0) {
        fprintf(stderr, "CRITICAL: Mutex attribute initialization failed\n");
        return EXIT_FAILURE;
    }
    if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
        fprintf(stderr, "CRITICAL: Inter-process attribute config failed\n");
        pthread_mutexattr_destroy(&attr);
        return EXIT_FAILURE;
    }
    if (pthread_mutex_init(&state->mutex, &attr) != 0) {
        fprintf(stderr, "CRITICAL: Cross-process memory mutex initialization failed\n");
        pthread_mutexattr_destroy(&attr);
        return EXIT_FAILURE;
    }
    pthread_mutexattr_destroy(&attr); 

    state->status = STATUS_IDLE; 
    printf("[Controller] Shared memory ready. Commands: on / off / quit\n");

    char cmd[32];
    while (1) {
        printf("> ");
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) break;
        cmd[strcspn(cmd, "\r\n")] = '\0';

        if (strcmp(cmd, "quit") == 0) {
            break;
        } else if (strcmp(cmd, "on") == 0) {
            /* SỬA LỖI: Luôn kiểm tra mã lỗi trả về của hàm khóa mutex dùng chung */
            if (pthread_mutex_lock(&state->mutex) != 0) {
                fprintf(stderr, "ERROR: Failed to acquire atomic process lock\n");
                continue;
            }
            state->status = STATUS_RUNNING;
            if (pthread_mutex_unlock(&state->mutex) != 0) {
                fprintf(stderr, "ERROR: Failed to release atomic process lock\n");
            }
            printf("[Controller] Command sent: ON\n");
        } else if (strcmp(cmd, "off") == 0) {
            if (pthread_mutex_lock(&state->mutex) != 0) {
                fprintf(stderr, "ERROR: Failed to acquire atomic process lock\n");
                continue;
            }
            state->status = STATUS_IDLE;
            if (pthread_mutex_unlock(&state->mutex) != 0) {
                fprintf(stderr, "ERROR: Failed to release atomic process lock\n");
            }
            printf("[Controller] Command sent: OFF\n");
        } else {
            printf("Invalid command.\n");
        }
    }

    printf("[Controller] Tearing down resources. Unlinking virtual map...\n");
    pthread_mutex_destroy(&state->mutex);
    munmap(state, sizeof(device_state_t));
    shm_unlink(SHM_NAME);

    return EXIT_SUCCESS;
}