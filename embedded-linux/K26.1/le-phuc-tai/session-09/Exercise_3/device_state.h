#ifndef DEVICE_STATE_H
#define DEVICE_STATE_H

#include <pthread.h>

#define SHM_NAME        "/device_shm"
#define STATUS_IDLE     0
#define STATUS_RUNNING  1

typedef struct {
    pthread_mutex_t mutex;   /* Mutex shared globally within virtual map memory */
    int status;              /* Core discrete execution state */
} device_state_t;

#endif /* DEVICE_STATE_H */