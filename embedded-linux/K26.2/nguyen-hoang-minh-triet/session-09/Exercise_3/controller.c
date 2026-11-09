#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>

/* --- Shared Configurations --- */
#define SHM_NAME            "/device_shm"
#define SHM_PERMISSIONS     0666

/* --- Device States --- */
#define STATUS_OFF          0
#define STATUS_ON           1

/* --- Shared Structure --- */
typedef struct {
    pthread_mutex_t mutex;
    int             status;
} device_state_t;

/* --- Local Constants --- */
#define CMD_ON              "on"
#define CMD_OFF             "off"
#define CMD_QUIT            "quit"
#define MAX_BUFFER_SIZE     128

int main(void) {
    /* 1. Create POSIX Shared Memory */
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, SHM_PERMISSIONS);
    if (shm_fd < 0) {
        perror("[Controller] shm_open failed");
        exit(EXIT_FAILURE);
    }

    /* 2. Set the size of the shared memory object */
    if (ftruncate(shm_fd, sizeof(device_state_t)) == -1) {
        perror("[Controller] ftruncate failed");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    /* 3. Map shared memory into process address space */
    device_state_t *state = mmap(NULL, sizeof(device_state_t), 
                                 PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (state == MAP_FAILED) {
        perror("[Controller] mmap failed");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    /* We don't need the file descriptor after mmap */
    close(shm_fd);

    /* 4. Initialize Process-Shared Mutex */
    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0) {
        perror("[Controller] pthread_mutexattr_init failed");
        exit(EXIT_FAILURE);
    }
    
    if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
        perror("[Controller] pthread_mutexattr_setpshared failed");
        exit(EXIT_FAILURE);
    }
    
    if (pthread_mutex_init(&state->mutex, &attr) != 0) {
        perror("[Controller] pthread_mutex_init failed");
        exit(EXIT_FAILURE);
    }
    
    pthread_mutexattr_destroy(&attr);

    /* Set initial state */
    state->status = STATUS_OFF;

    printf("[Controller] Shared memory ready. Commands: on / off / quit\n");

    /* 5. Command Loop */
    char input[MAX_BUFFER_SIZE];
    while (1) {
        printf("> ");
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) {
            break; /* EOF */
        }

        /* Strip trailing newline */
        input[strcspn(input, "\r\n")] = '\0';

        if (strcmp(input, CMD_QUIT) == 0) {
            break;
        }

        if (strcmp(input, CMD_ON) == 0) {
            pthread_mutex_lock(&state->mutex);
            state->status = STATUS_ON;
            pthread_mutex_unlock(&state->mutex);
            printf("[Controller] Command sent: ON\n");
        } else if (strcmp(input, CMD_OFF) == 0) {
            pthread_mutex_lock(&state->mutex);
            state->status = STATUS_OFF;
            pthread_mutex_unlock(&state->mutex);
            printf("[Controller] Command sent: OFF\n");
        } else if (strlen(input) > 0) {
            printf("[Controller] Unknown command. Use: on / off / quit\n");
        }
    }

    /* 6. Cleanup */
    printf("[Controller] Cleaning up. Goodbye.\n");
    
    pthread_mutex_destroy(&state->mutex);
    
    if (munmap(state, sizeof(device_state_t)) == -1) {
        perror("[Controller] munmap failed");
    }
    
    if (shm_unlink(SHM_NAME) == -1) {
        perror("[Controller] shm_unlink failed");
    }

    return EXIT_SUCCESS;
}
