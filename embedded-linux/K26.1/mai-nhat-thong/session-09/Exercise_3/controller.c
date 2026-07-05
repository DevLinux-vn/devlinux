/**
 * @file controller.c
 * @brief Controller using POSIX Shared Memory with Process-Shared Mutex
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

#define SHM_NAME "/device_shm"

typedef struct {
    pthread_mutex_t mutex;
    int status; /* 0 = OFF, 1 = ON */
} device_state_t;

int main(void) {
    /* 1. Create POSIX Shared Memory object */
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, sizeof(device_state_t)) == -1) {
        perror("ftruncate failed");
        close(fd);
        exit(EXIT_FAILURE);
    }

    device_state_t *state = mmap(NULL, sizeof(device_state_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (state == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);

    /* 2. Initialize Process-Shared Mutex */
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    
    if (pthread_mutex_init(&state->mutex, &attr) != 0) {
        perror("pthread_mutex_init failed");
        exit(EXIT_FAILURE);
    }
    pthread_mutexattr_destroy(&attr);

    /* Initial state */
    state->status = 0;

    printf("[Controller] Shared memory ready. Commands: on / off / quit\n");

    char input[32];
    while (1) {
        printf("> ");
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "quit") == 0) break;

        /* 3. Thread-safe write operation across processes */
        if (strcmp(input, "on") == 0) {
            pthread_mutex_lock(&state->mutex);
            state->status = 1;
            pthread_mutex_unlock(&state->mutex);
            printf("[Controller] Command sent: ON\n");
        } 
        else if (strcmp(input, "off") == 0) {
            pthread_mutex_lock(&state->mutex);
            state->status = 0;
            pthread_mutex_unlock(&state->mutex);
            printf("[Controller] Command sent: OFF\n");
        }
    }

    /* 4. Graceful Cleanup */
    printf("[Controller] Cleaning up. Goodbye.\n");
    pthread_mutex_destroy(&state->mutex);
    munmap(state, sizeof(device_state_t));
    shm_unlink(SHM_NAME); /* Removes the object from /dev/shm */

    return EXIT_SUCCESS;
}