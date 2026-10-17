/**
 * @file device.c
 * @brief Device endpoint syncing state via Process-Shared Mutex
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <signal.h>

#define SHM_NAME "/device_shm"

typedef struct {
    pthread_mutex_t mutex;
    int status;
} device_state_t;

static volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
}

int main(void) {
    setbuf(stdout, NULL);

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    /* 1. Open existing POSIX Shared Memory object */
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd < 0) {
        perror("shm_open failed. Is the controller running?");
        exit(EXIT_FAILURE);
    }

    device_state_t *state = mmap(NULL, sizeof(device_state_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (state == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);

    printf("[Device] Attached to %s\n", SHM_NAME);

    /* 2. Polling loop with process-shared synchronization */
    while (keep_running) {
        pthread_mutex_lock(&state->mutex);
        int current_status = state->status;
        pthread_mutex_unlock(&state->mutex);

        if (current_status == 1) {
            printf("[Device] Status: ON  — Running...\n");
        } else {
            printf("[Device] Status: OFF — Idle.\n");
        }

        unsigned int rem = 1;
        while(rem > 0 && keep_running) rem = sleep(rem);
    }

    munmap(state, sizeof(device_state_t));
    return EXIT_SUCCESS;
}