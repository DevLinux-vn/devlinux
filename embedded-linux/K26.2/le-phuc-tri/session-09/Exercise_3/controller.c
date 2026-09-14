#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#define SHM_NAME "/device_shm"

typedef struct {
    pthread_mutex_t mutex;
    int status;
} device_state_t;


int main(void)
{
    int fd;
    device_state_t *state;

    /* 1. Create/open shared memory */
    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        return EXIT_FAILURE;
    }

    /* 2. Set shared memory size */
    if (ftruncate(fd, sizeof(device_state_t)) == -1) {
        perror("ftruncate");
        close(fd);
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    /* 3. Map shared memory into process address space */
    state = mmap(NULL,
                 sizeof(device_state_t),
                 PROT_READ | PROT_WRITE,
                 MAP_SHARED,
                 fd,
                 0);

    if (state == MAP_FAILED) {
        perror("mmap");
        close(fd);
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    /*
     * fd is no longer needed after mmap().
     * The mapping remains valid until munmap().
     */
    if (close(fd) == -1) {
        perror("close");
        munmap(state, sizeof(device_state_t));
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    /* 4. Initialize pthread mutex attributes */
    pthread_mutexattr_t attr;

    if (pthread_mutexattr_init(&attr) != 0) {
        fprintf(stderr, "pthread_mutexattr_init failed\n");
        munmap(state, sizeof(device_state_t));
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    /*
     * VERY IMPORTANT:
     * Make the mutex usable between different processes.
     */
    if (pthread_mutexattr_setpshared(&attr,
                                     PTHREAD_PROCESS_SHARED) != 0) {
        fprintf(stderr, "pthread_mutexattr_setpshared failed\n");
        pthread_mutexattr_destroy(&attr);
        munmap(state, sizeof(device_state_t));
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    /* 5. Initialize mutex inside shared memory */
    if (pthread_mutex_init(&state->mutex, &attr) != 0) {
        fprintf(stderr, "pthread_mutex_init failed\n");
        pthread_mutexattr_destroy(&attr);
        munmap(state, sizeof(device_state_t));
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    /* Attribute object is no longer needed */
    if (pthread_mutexattr_destroy(&attr) != 0) {
        fprintf(stderr, "pthread_mutexattr_destroy failed\n");
        pthread_mutex_destroy(&state->mutex);
        munmap(state, sizeof(device_state_t));
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    /* 6. Initial device state = OFF */
    state->status = 0;

    printf("[Controller] Shared memory ready. Commands: on / off / quit\n");

    char command[32];

    while (1) {

        printf("> ");
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            printf("\n");
            break;
        }

        /* Remove newline */
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "on") == 0) {

            if (pthread_mutex_lock(&state->mutex) != 0) {
                fprintf(stderr, "pthread_mutex_lock failed\n");
                break;
            }

            state->status = 1;

            if (pthread_mutex_unlock(&state->mutex) != 0) {
                fprintf(stderr, "pthread_mutex_unlock failed\n");
                break;
            }

            printf("[Controller] Command sent: ON\n");

        } else if (strcmp(command, "off") == 0) {

            if (pthread_mutex_lock(&state->mutex) != 0) {
                fprintf(stderr, "pthread_mutex_lock failed\n");
                break;
            }

            state->status = 0;

            if (pthread_mutex_unlock(&state->mutex) != 0) {
                fprintf(stderr, "pthread_mutex_unlock failed\n");
                break;
            }

            printf("[Controller] Command sent: OFF\n");

        } else if (strcmp(command, "quit") == 0) {

            printf("[Controller] Cleaning up. Goodbye.\n");
            break;

        } else if (command[0] != '\0') {

            printf("[Controller] Unknown command. Use: on / off / quit\n");
        }
    }

    /* 7. Destroy mutex */
    if (pthread_mutex_destroy(&state->mutex) != 0) {
        fprintf(stderr, "pthread_mutex_destroy failed\n");
    }

    /* 8. Unmap shared memory */
    if (munmap(state, sizeof(device_state_t)) == -1) {
        perror("munmap");
    }

    /* 9. Remove shared memory object */
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink");
    }

    return EXIT_SUCCESS;
}
