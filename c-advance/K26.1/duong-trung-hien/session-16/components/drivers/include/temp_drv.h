#ifndef TEMP_DRV_H
#define TEMP_DRV_H

#include "sensor_intf.h"

/**
 * @brief Get the temperature driver interface.
 * @return Pointer to the static const sensor interface.
 */
const sensor_intf_t *temp_drv_get_interface(void);

#endif /* TEMP_DRV_H */