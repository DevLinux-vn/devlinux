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
    int status; /* 0 = OFF, 1 = ON */
} device_state_t;

static void die(const char *msg) {
    perror(msg);
    exit(1);
}

int main(void) {
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) die("shm_open");

    if (ftruncate(fd, sizeof(device_state_t)) == -1) {
        close(fd);
        die("ftruncate");
    }

    device_state_t *state = mmap(NULL, sizeof(device_state_t),
                                 PROT_READ | PROT_WRITE,
                                 MAP_SHARED, fd, 0);

    if (state == MAP_FAILED) {
        close(fd);
        die("mmap");
    }

    close(fd);

    pthread_mutexattr_t attr;

    if (pthread_mutexattr_init(&attr) != 0) {
        die("pthread_mutexattr_init");
    }

    if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
        die("pthread_mutexattr_setpshared");
    }

    if (pthread_mutex_init(&state->mutex, &attr) != 0) {
        die("pthread_mutex_init");
    }

    if (pthread_mutexattr_destroy(&attr) != 0) {
        die("pthread_mutexattr_destroy");
    }

    state->status = 0;

    printf("[Controller] Shared memory ready. Commands: on / off / quit\n");

    char input[64];

    while (1) {
        printf("> ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "quit") == 0) {
            break;
        }

        if (strcmp(input, "on") == 0) {
            if (pthread_mutex_lock(&state->mutex) != 0) {
                die("pthread_mutex_lock");
            }

            state->status = 1;

            if (pthread_mutex_unlock(&state->mutex) != 0) {
                die("pthread_mutex_unlock");
            }

            printf("[Controller] Command sent: ON\n");
        }

        else if (strcmp(input, "off") == 0) {
            if (pthread_mutex_lock(&state->mutex) != 0) {
                die("pthread_mutex_lock");
            }

            state->status = 0;

            if (pthread_mutex_unlock(&state->mutex) != 0) {
                die("pthread_mutex_unlock");
            }

            printf("[Controller] Command sent: OFF\n");
        }

        else {
            printf("Unknown command. Use: on / off / quit\n");
        }
    }

    printf("[Controller] Cleaning up. Goodbye.\n");

    if (pthread_mutex_destroy(&state->mutex) != 0) {
        die("pthread_mutex_destroy");
    }

    if (munmap(state, sizeof(device_state_t)) == -1) {
        die("munmap");
    }

    if (shm_unlink(SHM_NAME) == -1) {
        die("shm_unlink");
    }

    return 0;
}