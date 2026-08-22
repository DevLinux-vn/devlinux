#ifndef DEVICE_CFG_H
#define DEVICE_CFG_H

#define CONFIG_FILE_PATH "/tmp/device.cfg"
#define CONFIG_FILE_PERMISSION 0666

#define DEFAULT_BAUD_RATE 9600
#define DEFAULT_SAMPLING_RATE 100
#define DEFAULT_LOG_LEVEL 2

#define MIN_SAMPLING_RATE 1
#define MAX_SAMPLING_RATE 1000

#define LOG_LEVEL_OFF 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 3

typedef struct
{
    int baud_rate;
    int sampling_rate_hz;
    int log_level;
} device_cfg_t;

#endif /* DEVICE_CFG_H */