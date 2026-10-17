#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>

#define SHM_NAME "/device_shm"

typedef struct {
    pthread_mutex_t mutex;   /* mutex lives inside shared memory */
    int             status;  /* 0 = OFF, 1 = ON */
} device_state_t;

static device_state_t *global_state = NULL;
static volatile sig_atomic_t should_exit = 0;

/* Async-signal-safe signal handler */
void signal_handler(int sig) {
    (void)sig;
    should_exit = 1;
}

int main() {
    // Register signal handler FIRST
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    
    // Open existing shared memory (no O_CREAT, assume controller created it)
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd == -1) {
        fprintf(stderr, "[Device] shm_open failed: %s\n", strerror(errno));
        fprintf(stderr, "[Device] Ensure controller is running first.\n");
        exit(1);
    }
    
    // Map shared memory
    device_state_t *state = mmap(NULL, sizeof(device_state_t),
                                 PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (state == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(1);
    }
    
    close(fd);
    global_state = state;
    
    printf("[Device] Attached to %s\n", SHM_NAME);
    fflush(stdout);
    
    while (!should_exit) {
        int ret = pthread_mutex_lock(&state->mutex);
        if (ret != 0) {
            fprintf(stderr, "[Device] pthread_mutex_lock failed: %s\n", strerror(ret));
            break;
        }
        
        int current_status = state->status;
        
        ret = pthread_mutex_unlock(&state->mutex);
        if (ret != 0) {
            fprintf(stderr, "[Device] pthread_mutex_unlock failed: %s\n", strerror(ret));
            break;
        }
        
        if (current_status == 1) {
            printf("[Device] Status: ON  — Running...\n");
        } else {
            printf("[Device] Status: OFF — Idle.\n");
        }
        
        fflush(stdout);
        
        /* Sleep might be interrupted by SIGINT, but that's OK - loop will check should_exit */
        sleep(1);
    }
    
    /* Cleanup - only called from main, not from signal handler */
    printf("\n[Device] Received signal. Detaching from shared memory...\n");
    fflush(stdout);
    
    if (global_state != NULL) {
        if (munmap(global_state, sizeof(device_state_t)) == -1) {
            perror("munmap");
        }
    }
    
    printf("[Device] Detached from shared memory. Goodbye.\n");
    
    return 0;
}
