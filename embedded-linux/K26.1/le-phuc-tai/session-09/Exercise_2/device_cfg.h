#ifndef DEVICE_CFG_H
#define DEVICE_CFG_H

#define CONFIG_FILE_PATH "/tmp/device.cfg"
#define MENU_BUF_SIZE    32  
#define BAUD_LOW         9600
#define BAUD_MID         115200
#define BAUD_HIGH        460800
#define SAMPLING_MIN     1
#define SAMPLING_MAX     1000

typedef struct {
    int baud_rate;
    int sampling_rate_hz;
    int log_level;
} device_cfg_t;

#endif /* DEVICE_CFG_H */