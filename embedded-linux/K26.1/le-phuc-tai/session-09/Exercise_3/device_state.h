/**
 * @file device_state.h
 * @brief Operational system core schema for cross-process atomic structures.
 * @date 2026-07-04
 */

#ifndef DEVICE_STATE_H
#define DEVICE_STATE_H

#include <pthread.h>

#define SHM_NAME        "/device_shm"
#define STATUS_IDLE     0
#define STATUS_RUNNING  1

typedef struct {
    pthread_mutex_t mutex;   /* Synchronization construct physically hosted inside shared RAM */
    int status;              /* Core state system flag */
} device_state_t;

#endif /* DEVICE_STATE_H */