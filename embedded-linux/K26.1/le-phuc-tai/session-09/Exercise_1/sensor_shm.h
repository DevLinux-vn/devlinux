/**
 * @file sensor_shm.h
 * @brief Header definition for SystemV shared memory segment constants.
 * @date 2026-07-04
 */

#ifndef SENSOR_SHM_H
#define SENSOR_SHM_H

#include <time.h>

#define SHM_KEY 0x1234  /* Fixed memory key architecture identification */

typedef struct {
    time_t timestamp;
    double cpu_temp;
    double ram_used_pct;
} sensor_data_t;

#endif /* SENSOR_SHM_H */