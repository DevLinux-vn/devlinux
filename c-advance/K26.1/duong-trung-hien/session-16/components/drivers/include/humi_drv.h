#ifndef HUMI_DRV_H
#define HUMI_DRV_H

#include "sensor_intf.h"

/**
 * @brief Get the humidity driver interface.
 *
 * @return Pointer to the static humidity sensor interface.
 */
const sensor_intf_t *humi_drv_get_interface(void);

#endif /* HUMI_DRV_H */