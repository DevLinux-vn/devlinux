/**
 * @file device.c
 * @brief Remote listener tracking memory flag adjustments via atomic process validation.
 * @date 2026-07-04
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include "device_state.h"

#define POLLING_CYCLE_SEC 1

volatile sig_atomic_t g_keep_running = 1;

static void handle_sigint(int signum) {
    (void)signum;
    g_keep_running = 0;
}

int main(void) {
    setbuf(stdout, NULL);

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("CRITICAL: Failed to register SIGINT");
        return EXIT_FAILURE;
    }

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("CRITICAL: Host shm connection missing. Initiate controller first");
        return EXIT_FAILURE;
    }

    device_state_t *state = (device_state_t *)mmap(NULL, sizeof(device_state_t),
                                                   PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (state == MAP_FAILED) {
        perror("CRITICAL: Device mmap linking crashed");
        close(shm_fd);
        return EXIT_FAILURE;
    }
    close(shm_fd);

    printf("[Device] Attached to %s\n", SHM_NAME);

    while (g_keep_running) {
        int active_state_snapshot = STATUS_IDLE;

        /* SỬA LỖI: Luôn kiểm tra kiểm tra mã lỗi của thao tác đồng bộ phía Reader */
        if (pthread_mutex_lock(&state->mutex) == 0) {
            active_state_snapshot = state->status;
            if (pthread_mutex_unlock(&state->mutex) != 0) {
                fprintf(stderr, "ERROR: Mutex release failed inside processing thread\n");
            }
        } else {
            fprintf(stderr, "ERROR: Mutex lock acquisition failure encountered\n");
        }

        if (active_state_snapshot == STATUS_RUNNING) {
            printf("[Device] Status: ON  — Running...\n");
        } else {
            printf("[Device] Status: OFF — Idle.\n");
        }

        sleep(POLLING_CYCLE_SEC);
    }

    printf("\n[Device] Disconnecting segment memory traces...\n");
    munmap(state, sizeof(device_state_t));

    return EXIT_SUCCESS;
}