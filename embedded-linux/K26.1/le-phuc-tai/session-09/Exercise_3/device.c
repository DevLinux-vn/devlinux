#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include "device_state.h"

#define LOOP_INTERVAL_SEC 1

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

    /* Kết nối vào phân vùng của controller, không tạo mới */
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("CRITICAL: shm_open failed. Run controller first!");
        return EXIT_FAILURE;
    }

    device_state_t *state = (device_state_t *)mmap(NULL, sizeof(device_state_t),
                                                   PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (state == MAP_FAILED) {
        perror("CRITICAL: mmap failed");
        close(shm_fd);
        return EXIT_FAILURE;
    }
    close(shm_fd);

    printf("[Device] Attached to %s\n", SHM_NAME);

    while (g_keep_running) {
        int current_status;

        /* Đồng bộ hóa: Khóa mutex trước khi đọc dữ liệu chia sẻ */
        pthread_mutex_lock(&state->mutex);
        current_status = state->status;
        pthread_mutex_unlock(&state->mutex);

        if (current_status == 1) {
            printf("[Device] Status: ON  — Running...\n");
        } else {
            printf("[Device] Status: OFF — Idle.\n");
        }

        sleep(LOOP_INTERVAL_SEC);
    }

    printf("\n[Device] Detaching core segment...\n");
    munmap(state, sizeof(device_state_t));

    return EXIT_SUCCESS;
}