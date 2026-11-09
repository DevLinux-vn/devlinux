#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>
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
#define SLEEP_INTERVAL_SEC  1

volatile sig_atomic_t g_is_running = 1;

static void handle_sigint(int signum) {
    (void)signum;
    g_is_running = 0;
}

int main(void) {
    /* 1. Register SIGINT handler */
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    /* 2. Open Existing POSIX Shared Memory (No O_CREAT) */
    int shm_fd = shm_open(SHM_NAME, O_RDWR, SHM_PERMISSIONS);
    if (shm_fd < 0) {
        if (errno == ENOENT) {
            fprintf(stderr, "[Device] Controller is not running (shm not found).\n");
        } else {
            perror("[Device] shm_open failed");
        }
        exit(EXIT_FAILURE);
    }

    /* 3. Map shared memory */
    device_state_t *state = mmap(NULL, sizeof(device_state_t), 
                                 PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (state == MAP_FAILED) {
        perror("[Device] mmap failed");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }
    close(shm_fd); /* FD is no longer needed after mapping */

    printf("[Device] Attached to %s\n", SHM_NAME);

    /* 4. Monitoring Loop */
    while (g_is_running) {
        int current_status;

        /* Safely read the status using Mutex Lock */
        pthread_mutex_lock(&state->mutex);
        current_status = state->status;
        pthread_mutex_unlock(&state->mutex);

        if (current_status == STATUS_ON) {
            printf("[Device] Status: ON  — Running...\n");
        } else {
            printf("[Device] Status: OFF — Idle.\n");
        }

        /* Safe sleep: Handle interruption by SIGINT */
        unsigned int time_left = SLEEP_INTERVAL_SEC;
        while (time_left > 0 && g_is_running) {
            time_left = sleep(time_left);
        }
    }

    /* 5. Cleanup on exit */
    printf("\n[Device] Detaching and shutting down...\n");
    if (munmap(state, sizeof(device_state_t)) == -1) {
        perror("[Device] munmap failed");
    }

    return EXIT_SUCCESS;
}
