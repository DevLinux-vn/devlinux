#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/stat.h>
#include <errno.h>

#include "device_state.h"

#define SHM_NAME "/device_shm"
#define INPUT_BUFFER_SIZE 256

void clear_stdin(void) {
    int c;
    while (1) {
        c = getchar();
        if (c == '\n' || c == EOF) {
            break;
        }
    }
}

int get_user_input(char *buffer, size_t size) {
    while (fgets(buffer, size, stdin) == NULL) {
        if (errno == EINTR) {
            continue;
        }
        return -1;
    }
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        clear_stdin();
    }
    return 0;
}

int main(void) {
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        return EXIT_FAILURE;
    }

    if (ftruncate(fd, sizeof(device_state_t)) == -1) {
        perror("ftruncate");
        close(fd);
        return EXIT_FAILURE;
    }

    device_state_t *state = mmap(NULL, sizeof(device_state_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (state == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }
    close(fd);

    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0) {
        perror("pthread_mutexattr_init");
        return EXIT_FAILURE;
    }

    if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
        perror("pthread_mutexattr_setpshared");
        return EXIT_FAILURE;
    }

    if (pthread_mutex_init(&state->mutex, &attr) != 0) {
        perror("pthread_mutex_init");
        return EXIT_FAILURE;
    }

    if (pthread_mutexattr_destroy(&attr) != 0) {
        perror("pthread_mutexattr_destroy");
        munmap(state, sizeof(device_state_t));
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    /* Initialize status */
    if (pthread_mutex_lock(&state->mutex) != 0) {
        perror("pthread_mutex_lock");
    }
    state->status = 0;
    if (pthread_mutex_unlock(&state->mutex) != 0) {
        perror("pthread_mutex_unlock");
    }

    printf("[Controller] Shared memory ready. Commands: on / off / quit\n");

    while (1) {
        printf("> ");
        char cmd[INPUT_BUFFER_SIZE];
        if (get_user_input(cmd, sizeof(cmd)) != 0) {
            break;
        }

        if (strcmp(cmd, "quit") == 0) {
            break;
        } else if (strcmp(cmd, "on") == 0) {
            if (pthread_mutex_lock(&state->mutex) != 0) {
                perror("pthread_mutex_lock");
            }
            state->status = 1;
            if (pthread_mutex_unlock(&state->mutex) != 0) {
                perror("pthread_mutex_unlock");
            }
            printf("[Controller] Command sent: ON\n");
        } else if (strcmp(cmd, "off") == 0) {
            if (pthread_mutex_lock(&state->mutex) != 0) {
                perror("pthread_mutex_lock");
            }
            state->status = 0;
            if (pthread_mutex_unlock(&state->mutex) != 0) {
                perror("pthread_mutex_unlock");
            }
            printf("[Controller] Command sent: OFF\n");
        } else {
            printf("Invalid command.\n");
        }
    }

    printf("[Controller] Cleaning up. Goodbye.\n");

    if (pthread_mutex_destroy(&state->mutex) != 0) {
        perror("pthread_mutex_destroy");
    }

    if (munmap(state, sizeof(device_state_t)) == -1) {
        perror("munmap");
    }

    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink");
    }

    return EXIT_SUCCESS;
}
