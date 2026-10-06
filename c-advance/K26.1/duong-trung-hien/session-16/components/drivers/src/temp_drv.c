#include "temp_drv.h"
#include "common_types.h"
#include "hal_sim.h"
#include "sensor_intf.h"
#include <stddef.h>
#include <stdint.h>

/** @brief Internal driver config — NOT visible outside this file. */
typedef struct {
  bool is_initialized;
  int16_t offset_cal; /**< Calibration offset in tenths of degree. */
} temp_drv_config_t;

static temp_drv_config_t s_config = {.is_initialized = false, .offset_cal = 0};

static status_t temp_init(void) {
  s_config.is_initialized = true;
  s_config.offset_cal = 0;

  return STATUS_OK;
}

static status_t temp_read(int16_t *p_value) {
  if (NULL == p_value) {
    return STATUS_ERR_NULL_PTR;
  }

  if (!s_config.is_initialized) {
    return STATUS_ERR_INVALID_PARAM;
  }

  uint16_t raw_value;
  int32_t calibrated_value;

  raw_value = hal_sim_read_register(HAL_REG_TEMP_RAW);
  calibrated_value = ((int32_t)raw_value / 10) + s_config.offset_cal;

  if (calibrated_value > INT16_MAX) {
    return STATUS_ERR_INVALID_PARAM;
  }

  *p_value = (int16_t)calibrated_value;

  return STATUS_OK;
}

static const char *temp_get_name(void) { return "Temperature"; }

static const sensor_intf_t s_temp_intf = {
    .init = temp_init,
    .read = temp_read,
    .get_name = temp_get_name,
};

const sensor_intf_t *temp_drv_get_interface(void) { return &s_temp_intf; }