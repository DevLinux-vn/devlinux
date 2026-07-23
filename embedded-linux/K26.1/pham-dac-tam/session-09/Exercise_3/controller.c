#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
        shm_unlink(SHM_NAME);
        exit(1);
    }
    
    // Map shared memory
    device_state_t *state = mmap(NULL, sizeof(device_state_t),
                                 PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (state == MAP_FAILED) {
        perror("mmap");
        close(fd);
        shm_unlink(SHM_NAME);
        exit(1);
    }
    
    close(fd);
    global_state = state;  // Store for signal handler
    
    // Initialize mutex with PTHREAD_PROCESS_SHARED attribute
    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0) {
        fprintf(stderr, "[Controller] pthread_mutexattr_init failed\n");
        munmap(state, sizeof(device_state_t));
        shm_unlink(SHM_NAME);
        exit(1);
    }
    
    if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
        fprintf(stderr, "[Controller] pthread_mutexattr_setpshared failed\n");
        pthread_mutexattr_destroy(&attr);
        munmap(state, sizeof(device_state_t));
        shm_unlink(SHM_NAME);
        exit(1);
    }
    
    if (pthread_mutex_init(&state->mutex, &attr) != 0) {
        fprintf(stderr, "[Controller] pthread_mutex_init failed\n");
        pthread_mutexattr_destroy(&attr);
        munmap(state, sizeof(device_state_t));
        shm_unlink(SHM_NAME);
        exit(1);
    }
    
    if (pthread_mutexattr_destroy(&attr) != 0) {
        fprintf(stderr, "[Controller] pthread_mutexattr_destroy failed\n");
    }
    
    // Initialize status
    state->status = 0;
    
    printf("[Controller] Shared memory ready. Commands: on / off / quit\n");
    fflush(stdout);
    
    char input[256];
    
    while (!should_exit) {
        printf("> ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        // Remove trailing newline
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        
        if (strcmp(input, "quit") == 0) {
            break;
        }
        
        if (strcmp(input, "on") == 0) {
            if (pthread_mutex_lock(&state->mutex) != 0) {
                fprintf(stderr, "[Controller] pthread_mutex_lock failed\n");
                continue;
            }
            state->status = 1;
            if (pthread_mutex_unlock(&state->mutex) != 0) {
                fprintf(stderr, "[Controller] pthread_mutex_unlock failed\n");
            }
            printf("[Controller] Command sent: ON\n");
        } else if (strcmp(input, "off") == 0) {
            if (pthread_mutex_lock(&state->mutex) != 0) {
                fprintf(stderr, "[Controller] pthread_mutex_lock failed\n");
                continue;
            }
            state->status = 0;
            if (pthread_mutex_unlock(&state->mutex) != 0) {
                fprintf(stderr, "[Controller] pthread_mutex_unlock failed\n");
            }
            printf("[Controller] Command sent: OFF\n");
        } else if (strlen(input) > 0) {
            printf("[Controller] Unknown command (use: on / off / quit)\n");
        }
    }
    
    /* Cleanup - called from main, not signal handler */
    printf("\n[Controller] Cleaning up...\n");
    fflush(stdout);
    
    if (pthread_mutex_destroy(&state->mutex) != 0) {
        fprintf(stderr, "[Controller] pthread_mutex_destroy failed\n");
    }
    
    if (munmap(state, sizeof(device_state_t)) == -1) {
        perror("munmap");
    }
    
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink");
    }
    
    printf("[Controller] Cleanup done. Goodbye.\n");
    
    return 0;
}
