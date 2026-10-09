#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

#define SHM_NAME "/device_shm"

typedef struct {
    pthread_mutex_t mutex;
    int status;
} device_state_t;

int main() {
    // 1. shm_open(O_RDWR) + mmap (No O_CREAT, wait for controller)
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        fprintf(stderr, "Shared memory does not exist. Please run controller first.\n");
        exit(EXIT_FAILURE);
    }

    device_state_t *state = (device_state_t *)mmap(NULL, sizeof(device_state_t),
                                                   PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (state == MAP_FAILED) {
        perror("mmap failed");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }
    close(shm_fd);

    printf("[Device] Attached to %s\n", SHM_NAME);

    // 2. Loop every 1 second
    while (1) {
        pthread_mutex_lock(&(state->mutex));
        int current_status = state->status;
        pthread_mutex_unlock(&(state->mutex));

        if (current_status == 1) {
            printf("[Device] Status: ON  – Running...\n");
        } else {
            printf("[Device] Status: OFF – Idle.\n");
        }

        sleep(1);
    }

    // Unmap on clean exit (unreachable here due to infinite loop, handles via Ctrl+C default)
    munmap(state, sizeof(device_state_t));
    return 0;
}