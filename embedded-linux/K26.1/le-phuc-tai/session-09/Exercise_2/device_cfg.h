/**
 * @file device_cfg.h
 * @brief Configuration data schema mapped directly into system storage file.
 * @date 2026-07-03
 */

#ifndef DEVICE_CFG_H
#define DEVICE_CFG_H

#define CONFIG_FILE_PATH "/tmp/device.cfg"
#define MENU_BUF_SIZE    32  /* SỬA LỖI: Định nghĩa macro xóa bỏ triệt để Magic Number */

typedef struct {
    int baud_rate;
    int sampling_rate_hz;
    int log_level;
} device_cfg_t;

#endif