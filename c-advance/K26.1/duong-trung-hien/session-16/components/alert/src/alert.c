#include "alert.h"
#include "common_types.h"
#include "logger.h"
#include <stddef.h>
#include <stdint.h>

static alert_config_t s_alert_config;
static bool s_alert_active = false;

void alert_init(const alert_config_t *p_config) {
  if (NULL == p_config) {
    LOG_ERROR("%s", "NULL alert configuration");
    return;
  }

  s_alert_config = *p_config;
  s_alert_active = false;
}

bool alert_check(const sensor_data_t *p_data) {
  if (NULL == p_data) {
    LOG_ERROR("%s", "NULL sensor data");
    return false;
  }

  const bool temperature_high = (p_data->temperature > s_alert_config.temp_max);

  const bool temperature_low = (p_data->temperature < s_alert_config.temp_min);

  const bool humidity_high = (p_data->humidity > s_alert_config.humi_max);

  const bool humidity_low = (p_data->humidity < s_alert_config.humi_min);

  if (temperature_high) {
    LOG_WARN("ALERT: Temperature %d exceeds max threshold %d",
             (int)p_data->temperature, (int)s_alert_config.temp_max);
    s_alert_active = true;
  } else if (temperature_low) {
    LOG_WARN("ALERT: Temperature %d is below min threshold %d",
             (int)p_data->temperature, (int)s_alert_config.temp_min);
    s_alert_active = true;
  } else if (humidity_high) {
    LOG_WARN("ALERT: Humidity %d exceeds max threshold %d",
             (int)p_data->humidity, (int)s_alert_config.humi_max);
    s_alert_active = true;
  } else if (humidity_low) {
    LOG_WARN("ALERT: Humidity %d is below min threshold %d",
             (int)p_data->humidity, (int)s_alert_config.humi_min);
    s_alert_active = true;
  }

  return s_alert_active;
}

void alert_acknowledge(void) {
  s_alert_active = false;
  LOG_INFO("%s", "Alert acknowledged");
}

bool alert_is_active(void) { return s_alert_active; }