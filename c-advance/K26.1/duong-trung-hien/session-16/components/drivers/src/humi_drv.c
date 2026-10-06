#include "humi_drv.h"
#include "common_types.h"
#include "hal_sim.h"
#include "sensor_intf.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
  bool is_initialized;
  int16_t offset_cal;
} humi_drv_config_t;

static humi_drv_config_t s_config = {.is_initialized = false, .offset_cal = 0};

static status_t humi_init(void) {
  s_config.is_initialized = true;
  s_config.offset_cal = 0;

  return STATUS_OK;
}

static status_t humi_read(int16_t *p_value) {
  if (NULL == p_value) {
    return STATUS_ERR_NULL_PTR;
  }

  if (!s_config.is_initialized) {
    return STATUS_ERR_INVALID_PARAM;
  }

  uint16_t raw_value;
  int32_t calibrated_value;

  raw_value = hal_sim_read_register(HAL_REG_HUMI_RAW);
  calibrated_value = ((int32_t)raw_value / 10) + s_config.offset_cal;

  if (calibrated_value > INT16_MAX) {
    return STATUS_ERR_INVALID_PARAM;
  }

  *p_value = (int16_t)calibrated_value;

  return STATUS_OK;
}

static const char *humi_get_name(void) { return "Humidity"; }

static const sensor_intf_t s_humi_intf = {
    .init = humi_init,
    .read = humi_read,
    .get_name = humi_get_name,
};

const sensor_intf_t *humi_drv_get_interface(void) { return &s_humi_intf; }