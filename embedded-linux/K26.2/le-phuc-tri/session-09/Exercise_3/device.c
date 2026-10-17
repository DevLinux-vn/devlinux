#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <pthread.h>

#define SHM_NAME "/device_shm"

typedef struct {
    pthread_mutex_t mutex;
    int status;
} device_state_t;

static volatile sig_atomic_t running = 1;


static void handle_sigint(int sig)
{
    (void)sig;
    running = 0;
}


int main(void)
{
    int fd;
    device_state_t *state;

    /* 1. Open existing shared memory */
    fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        return EXIT_FAILURE;
    }

    /* 2. Map shared memory */
    state = mmap(NULL,
                 sizeof(device_state_t),
                 PROT_READ | PROT_WRITE,
                 MAP_SHARED,
                 fd,
                 0);

    if (state == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    /* fd no longer needed after mmap() */
    if (close(fd) == -1) {
        perror("close");
        munmap(state, sizeof(device_state_t));
        return EXIT_FAILURE;
    }

    /* 3. Install SIGINT handler */
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("signal");
        munmap(state, sizeof(device_state_t));
        return EXIT_FAILURE;
    }

    printf("[Device] Attached to /device_shm\n");

    /* 4. Device main loop */
    while (running) {

        int status;

        /*
         * Lock before reading shared state.
         */
        if (pthread_mutex_lock(&state->mutex) != 0) {
            fprintf(stderr, "pthread_mutex_lock failed\n");
            break;
        }

        status = state->status;

        /*
         * Unlock immediately after copying
         * the shared data into a local variable.
         */
        if (pthread_mutex_unlock(&state->mutex) != 0) {
            fprintf(stderr, "pthread_mutex_unlock failed\n");
            break;
        }

        if (status == 1) {
            printf("[Device] Status: ON  — Running...\n");
        } else {
            printf("[Device] Status: OFF — Idle.\n");
        }

        sleep(1);
    }

    /* 5. Detach shared memory */
    if (munmap(state, sizeof(device_state_t)) == -1) {
        perror("munmap");
        return EXIT_FAILURE;
    }

    printf("[Device] Stopped.\n");

    return EXIT_SUCCESS;
}
