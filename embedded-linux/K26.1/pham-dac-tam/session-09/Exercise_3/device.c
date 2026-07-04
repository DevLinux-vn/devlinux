#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <signal.h>

#define SHM_NAME "/device_shm"

typedef struct {
    pthread_mutex_t mutex;   /* mutex lives inside shared memory */
    int             status;  /* 0 = OFF, 1 = ON */
} device_state_t;

device_state_t *state = NULL;

void cleanup(int sig) {
    if (state != NULL) {
        munmap(state, sizeof(device_state_t));
    }
    printf("\n[Device] Detached from shared memory. Goodbye.\n");
    exit(0);
}

int main() {
    signal(SIGINT, cleanup);
    
    // Open existing shared memory (no O_CREAT, assume controller created it)
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        printf("[Device] Controller should run first to create shared memory.\n");
        exit(1);
    }
    
    // Map shared memory
    state = mmap(NULL, sizeof(device_state_t),
                 PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (state == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(1);
    }
    
    close(fd);
    
    printf("[Device] Attached to %s\n", SHM_NAME);
    
    while (1) {
        pthread_mutex_lock(&state->mutex);
        int current_status = state->status;
        pthread_mutex_unlock(&state->mutex);
        
        if (current_status == 1) {
            printf("[Device] Status: ON  — Running...\n");
        } else {
            printf("[Device] Status: OFF — Idle.\n");
        }
        
        sleep(1);
    }
    
    return 0;
}
