#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

#define SHM_NAME "/device_shm"

typedef struct {
    pthread_mutex_t mutex; /* mutex lives inside shared memory */
    int status;            /* 0 = OFF, 1 = ON */
} device_state_t;

int main() {
    // 1. shm_open(O_CREAT|O_RDWR) + ftruncate() + mmap
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(device_state_t)) == -1) {
        perror("ftruncate failed");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

    device_state_t *state = (device_state_t *)mmap(NULL, sizeof(device_state_t),
                                                   PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (state == MAP_FAILED) {
        perror("mmap failed");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }
    close(shm_fd); // Close descriptor as mmap is active

    // 2. Initialize mutex with PTHREAD_PROCESS_SHARED attribute
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&(state->mutex), &attr);
    pthread_mutexattr_destroy(&attr);

    // Initial state setup
    pthread_mutex_lock(&(state->mutex));
    state->status = 0;
    pthread_mutex_unlock(&(state->mutex));

    printf("[Controller] Shared memory ready. Commands: on / off / quit\n");

    char cmd[32];
    while (1) {
        printf("> ");
        if (scanf("%31s", cmd) != 1) break;

        if (strcmp(cmd, "quit") == 0) {
            break;
        } else if (strcmp(cmd, "on") == 0) {
            pthread_mutex_lock(&(state->mutex));
            state->status = 1;
            pthread_mutex_unlock(&(state->mutex));
            printf("[Controller] Command sent: ON\n");
        } else if (strcmp(cmd, "off") == 0) {
            pthread_mutex_lock(&(state->mutex));
            state->status = 0;
            pthread_mutex_unlock(&(state->mutex));
            printf("[Controller] Command sent: OFF\n");
        } else {
            printf("Unknown command. Use 'on', 'off', or 'quit'.\n");
        }
    }

    // On exit: destroy mutex, unmap, and unlink shm
    pthread_mutex_lock(&(state->mutex));
    pthread_mutex_unlock(&(state->mutex));
    pthread_mutex_destroy(&(state->mutex));
    
    munmap(state, sizeof(device_state_t));
    shm_unlink(SHM_NAME);

    return 0;
}