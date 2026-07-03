#ifndef DEVICE_STATE_H
#define DEVICE_STATE_H

#include <pthread.h>

#define SHM_NAME "/device_shm"  /* Đường dẫn định danh tệp chia sẻ ảo POSIX */

/* Cấu trúc dữ liệu chứa khóa đồng bộ dùng chung giữa các tiến trình độc lập */
typedef struct {
    pthread_mutex_t mutex;   /* Khóa vật lý nằm trực tiếp trên phân vùng RAM chia sẻ */
    int status;
} device_state_t;

#endif /* DEVICE_STATE_H */