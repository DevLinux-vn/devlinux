#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

#define SHM_NAME "/device_shm"

typedef struct {
    pthread_mutex_t mutex;   /* mutex lives inside shared memory */
    int             status;  /* 0 = OFF, 1 = ON */
} device_state_t;

int main() {
    // Create shared memory with truncate
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }
    
    // Truncate to size
    if (ftruncate(fd, sizeof(device_state_t)) == -1) {
        perror("ftruncate");
        close(fd);
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
    
    // Initialize mutex with PTHREAD_PROCESS_SHARED attribute
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&state->mutex, &attr);
    pthread_mutexattr_destroy(&attr);
    
    // Initialize status
    state->status = 0;
    
    printf("[Controller] Shared memory ready. Commands: on / off / quit\n");
    
    char input[256];
    
    while (1) {
        printf("> ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        // Remove trailing newline
        size_t len = strlen(input);
        if (input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        
        if (strcmp(input, "quit") == 0) {
            break;
        }
        
        if (strcmp(input, "on") == 0) {
            pthread_mutex_lock(&state->mutex);
            state->status = 1;
            pthread_mutex_unlock(&state->mutex);
            printf("[Controller] Command sent: ON\n");
        } else if (strcmp(input, "off") == 0) {
            pthread_mutex_lock(&state->mutex);
            state->status = 0;
            pthread_mutex_unlock(&state->mutex);
            printf("[Controller] Command sent: OFF\n");
        } else {
            printf("[Controller] Unknown command\n");
        }
    }
    
    // Cleanup
    pthread_mutex_destroy(&state->mutex);
    munmap(state, sizeof(device_state_t));
    shm_unlink(SHM_NAME);
    
    printf("[Controller] Cleaning up. Goodbye.\n");
    
    return 0;
}
