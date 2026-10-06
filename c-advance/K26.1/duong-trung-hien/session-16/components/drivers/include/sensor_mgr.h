#ifndef SENSOR_MGR_H
#define SENSOR_MGR_H

#include "common_types.h"

/**
 * @brief Initialize the sensor manager.
 *
 * @return STATUS_OK on success.
 */
status_t sensor_mgr_init(void);

/**
 * @brief Read all registered sensors.
 *
 * @param[out] p_data Destination for sensor data.
 *
 * @return STATUS_OK on success.
 *
 * @pre p_data must not be NULL.
 */
status_t sensor_mgr_read_all(sensor_data_t *p_data);

#endif /* SENSOR_MGR_H */