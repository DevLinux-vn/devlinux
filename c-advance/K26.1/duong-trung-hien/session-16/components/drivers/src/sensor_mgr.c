#include "sensor_mgr.h"

#include "common_types.h"
#include "contract.h"
#include "hal_timer.h"
#include "humi_drv.h"
#include "sensor_intf.h"
#include "temp_drv.h"
#include <stddef.h>
#include <stdint.h>

#define SENSOR_MGR_COUNT (2U)

static const sensor_intf_t *s_sensor_interfaces[SENSOR_MGR_COUNT] = {NULL,
                                                                     NULL};

static uint8_t s_sensor_count = 0U;

status_t sensor_mgr_init(void) {
  const sensor_intf_t *p_temp_intf;
  const sensor_intf_t *p_humi_intf;
  status_t status;

  p_temp_intf = temp_drv_get_interface();
  p_humi_intf = humi_drv_get_interface();

  if ((p_temp_intf == NULL) || (p_humi_intf == NULL)) {
    return STATUS_ERR_INVALID_PARAM;
  }

  s_sensor_interfaces[0U] = p_temp_intf;
  s_sensor_interfaces[1U] = p_humi_intf;
  s_sensor_count = SENSOR_MGR_COUNT;

  status = s_sensor_interfaces[0U]->init();
  if (status != STATUS_OK) {
    return status;
  }

  status = s_sensor_interfaces[1U]->init();
  if (status != STATUS_OK) {
    return status;
  }

  return STATUS_OK;
}

status_t sensor_mgr_read_all(sensor_data_t *p_data) {
  uint8_t sensor_index;
  int16_t sensor_value;

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_data != NULL);

  p_data->temperature = 0;
  p_data->humidity = 0;
  p_data->timestamp = hal_timer_get_tick();

  for (sensor_index = 0U; sensor_index < s_sensor_count; sensor_index++) {

    if (s_sensor_interfaces[sensor_index] == NULL) {
      return STATUS_ERR_INVALID_PARAM;
    }

    if (s_sensor_interfaces[sensor_index]->read == NULL) {
      return STATUS_ERR_INVALID_PARAM;
    }

    sensor_value = 0;

    status_t status = s_sensor_interfaces[sensor_index]->read(&sensor_value);
    if (status != STATUS_OK) {
      return status;
    }

    if (sensor_index == 0U) {
      p_data->temperature = sensor_value;
    } else {
      p_data->humidity = sensor_value;
    }
  }

  return STATUS_OK;
}