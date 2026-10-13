#ifndef DEVICE_CFG_H
#define DEVICE_CFG_H

/* --- File Configurations --- */
#define CONFIG_FILE_PATH    "/tmp/device.cfg"
#define CONFIG_FILE_PERMS   0666

/* --- Default Values --- */
#define DEFAULT_BAUD_RATE   9600
#define DEFAULT_SAMPLING_HZ 100
#define DEFAULT_LOG_LEVEL   2

/* --- Log Levels --- */
#define LOG_LEVEL_OFF       0
#define LOG_LEVEL_ERROR     1
#define LOG_LEVEL_INFO      2
#define LOG_LEVEL_DEBUG     3

/* --- Shared Structure --- */
typedef struct {
    int baud_rate;
    int sampling_rate_hz;
    int log_level;
} device_cfg_t;

#endif /* DEVICE_CFG_H */
