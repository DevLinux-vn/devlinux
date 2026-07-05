#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#define SHM_NAME "/device_shm"

typedef struct {
    pthread_mutex_t mutex;
    int status; /* 0 = OFF, 1 = ON */
} device_state_t;

static device_state_t *state = NULL;

static void die(const char *msg) {
    perror(msg);
    exit(1);
}

void cleanup(int sig) {
    (void)sig;

    printf("\n[Device] Detaching shared memory. Goodbye.\n");

    if (state != NULL) {
        if (munmap(state, sizeof(device_state_t)) == -1) {
            perror("munmap");
        }
    }

    exit(0);
}

int main(void) {
    signal(SIGINT, cleanup);

    int fd = shm_open(SHM_NAME, O_RDWR, 0666);

    if (fd == -1) {
        printf("[Device] Shared memory not found. Run controller first.\n");
        exit(1);
    }

    state = mmap(NULL, sizeof(device_state_t),
                 PROT_READ | PROT_WRITE,
                 MAP_SHARED, fd, 0);

    if (state == MAP_FAILED) {
        close(fd);
        die("mmap");
    }

    close(fd);

    printf("[Device] Attached to %s\n", SHM_NAME);

    while (1) {
        int current_status;

        if (pthread_mutex_lock(&state->mutex) != 0) {
            die("pthread_mutex_lock");
        }

        current_status = state->status;

        if (pthread_mutex_unlock(&state->mutex) != 0) {
            die("pthread_mutex_unlock");
        }

        if (current_status == 1) {
            printf("[Device] Status: ON  — Running...\n");
        } else {
            printf("[Device] Status: OFF — Idle.\n");
        }

        sleep(1);
    }

    return 0;
}