/**
 * @file sensor_shm.h
 * @brief Shared memory structure definition for sensor telemetry project.
 * @date 2026-07-03
 */

#ifndef SENSOR_SHM_H
#define SENSOR_SHM_H

#include <time.h>

#define SHM_KEY 0x1234  /* Định danh khóa phân vùng SystemV */

typedef struct {
    time_t timestamp;
    double cpu_temp;
    double ram_used_pct;
} sensor_data_t;

#endif