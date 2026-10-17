#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>

#define SHM_NAME "/device_shm"

typedef struct {
    pthread_mutex_t mutex; 
    int status;            
} device_state_t;

static int read_line(char *buf, size_t bufsize)
{
    if (fgets(buf, (int)bufsize, stdin) == NULL) {
        return -1;
    }
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }
    return 0;
}

int main(void)
{
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("[Controller] shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, sizeof(device_state_t)) == -1) {
        perror("[Controller] ftruncate");
        close(fd);
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    device_state_t *state = mmap(NULL, sizeof(device_state_t),
                                  PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (state == MAP_FAILED) {
        perror("[Controller] mmap");
        close(fd);
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    close(fd); 

    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0) {
        perror("[Controller] pthread_mutexattr_init");
        munmap(state, sizeof(device_state_t));
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
        perror("[Controller] pthread_mutexattr_setpshared");
        pthread_mutexattr_destroy(&attr);
        munmap(state, sizeof(device_state_t));
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&state->mutex, &attr) != 0) {
        perror("[Controller] pthread_mutex_init");
        pthread_mutexattr_destroy(&attr);
        munmap(state, sizeof(device_state_t));
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    pthread_mutexattr_destroy(&attr);

    pthread_mutex_lock(&state->mutex);
    state->status = 0;
    pthread_mutex_unlock(&state->mutex);

    printf("[Controller] Shared memory ready. Commands: on / off / quit\n");

    char line[32];
    while (1) {
        printf("> ");
        fflush(stdout);

        if (read_line(line, sizeof(line)) == -1) {
            break; 
        }

        if (strcmp(line, "quit") == 0) {
            break;
        } else if (strcmp(line, "on") == 0) {
            pthread_mutex_lock(&state->mutex);
            state->status = 1;
            pthread_mutex_unlock(&state->mutex);
            printf("[Controller] Command sent: ON\n");
        } else if (strcmp(line, "off") == 0) {
            pthread_mutex_lock(&state->mutex);
            state->status = 0;
            pthread_mutex_unlock(&state->mutex);
            printf("[Controller] Command sent: OFF\n");
        } else {
            fprintf(stderr, "[Controller] Unknown command: %s\n", line);
        }
    }

    if (pthread_mutex_destroy(&state->mutex) != 0) {
        perror("[Controller] pthread_mutex_destroy");
    }

    if (munmap(state, sizeof(device_state_t)) == -1) {
        perror("[Controller] munmap");
    }

    if (shm_unlink(SHM_NAME) == -1) {
        perror("[Controller] shm_unlink");
    }

    printf("[Controller] Cleaning up. Goodbye.\n");
    return 0;
}
