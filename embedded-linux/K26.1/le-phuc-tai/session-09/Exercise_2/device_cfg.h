#ifndef DEVICE_CFG_H
#define DEVICE_CFG_H

#define CONFIG_FILE_PATH "/tmp/device.cfg"
#define MENU_BUF_SIZE    32  

typedef struct {
    int baud_rate;
    int sampling_rate_hz;
    int log_level;
} device_cfg_t;

#endif