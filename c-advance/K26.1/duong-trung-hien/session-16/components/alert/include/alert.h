#ifndef ALERT_H
#define ALERT_H

#include <stdbool.h>
#include <stdint.h>

#include "common_types.h"

/**
 * @brief Alert threshold configuration.
 *
 * Sensor values use the same fixed-point representation as sensor_data_t:
 * value multiplied by 10.
 */
typedef struct {
  int16_t temp_max;
  int16_t temp_min;
  int16_t humi_max;
  int16_t humi_min;
} alert_config_t;

/**
 * @brief Initialize the alert module.
 *
 * @param[in] p_config Alert threshold configuration.
 */
void alert_init(const alert_config_t *p_config);

/**
 * @brief Check sensor data against configured thresholds.
 *
 * If any sensor value is outside the configured range, the alert
 * state becomes active.
 *
 * @param[in] p_data Sensor data to check.
 *
 * @return true if an alert condition is detected; false otherwise.
 */
bool alert_check(const sensor_data_t *p_data);

/**
 * @brief Acknowledge and clear the active alert.
 */
void alert_acknowledge(void);

/**
 * @brief Check whether an alert is currently active.
 *
 * @return true if an alert is active; false otherwise.
 */
bool alert_is_active(void);

#endif /* ALERT_H */