#ifndef SENSOR_SHM_H
#define SENSOR_SHM_H

#include <time.h>

#define SHM_KEY 0x1234  /* Khóa định danh cố định cho phân vùng nhớ SystemV */

/* Cấu trúc dữ liệu lưu trữ thông tin vi điều khiển và tài nguyên hệ thống */
typedef struct {
    time_t timestamp;
    double cpu_temp;
    double ram_used_pct;
} sensor_data_t;

#endif /* SENSOR_SHM_H */