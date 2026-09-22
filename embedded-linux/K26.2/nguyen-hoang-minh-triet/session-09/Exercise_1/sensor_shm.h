#ifndef SENSOR_SHM_H
#define SENSOR_SHM_H

#include <time.h>

/* --- Shared Memory Configuration --- */
#define SHM_KEY         0x1234
#define SHM_PERMISSIONS 0666

/* --- Data Structure --- */
typedef struct {
    time_t  timestamp;
    double  cpu_temp;
    double  ram_used_pct;
} sensor_data_t;

#endif /* SENSOR_SHM_H */

