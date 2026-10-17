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

int main(void)
{
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd == -1) {
        fprintf(stderr, "[Device] shm_open failed: %s\n", strerror(errno));
        fprintf(stderr, "[Device] Is the controller running?\n");
        exit(EXIT_FAILURE);
    }

    device_state_t *state = mmap(NULL, sizeof(device_state_t),
                                  PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (state == MAP_FAILED) {
        perror("[Device] mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd); 

    printf("[Device] Attached to %s\n", SHM_NAME);

    while (1) {
        int status;

        pthread_mutex_lock(&state->mutex);
        status = state->status;
        pthread_mutex_unlock(&state->mutex);

        if (status == 1) {
            printf("[Device] Status: ON  - Running...\n");
        } else {
            printf("[Device] Status: OFF - Idle.\n");
        }
        fflush(stdout);

        sleep(1);
    }
    
    munmap(state, sizeof(device_state_t));
    return 0;
}
