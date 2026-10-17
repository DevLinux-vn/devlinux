#include "sensor.h"
#include <stdint.h>

void sensor_process_readings(uint8_t *p_readings, uint32_t count) {
  for (uint32_t i = 0U; i < count; i++) {
    p_readings[i] = (uint8_t)(p_readings[i] * 2U);
  }
}

int32_t sensor_accumulate_data(int32_t current_val, int32_t sample) {
  if ((sample > 0) && (current_val > (INT32_MAX - sample))) {
    return INT32_MAX;
  }

  if ((sample < 0) && (current_val < (INT32_MIN - sample))) {
    return INT32_MIN;
  }

  return (current_val + sample);
}

uint32_t sensor_get_calibration(void) {
  static uint32_t data = 0xDEADBEEFU;

  return data;
}
