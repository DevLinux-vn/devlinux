#ifndef CONFIG_H
#define CONFIG_H

#include "alert.h"
#include "common_types.h"

/**
 * @brief Default maximum temperature threshold.
 *
 * Value is represented in tenths of a degree Celsius.
 */
#define CONFIG_DEFAULT_TEMP_MAX (350)

/**
 * @brief Default minimum temperature threshold.
 *
 * Value is represented in tenths of a degree Celsius.
 */
#define CONFIG_DEFAULT_TEMP_MIN (50)

/**
 * @brief Default maximum humidity threshold.
 *
 * Value is represented in tenths of a percent.
 */
#define CONFIG_DEFAULT_HUMI_MAX (800)

/**
 * @brief Default minimum humidity threshold.
 *
 * Value is represented in tenths of a percent.
 */
#define CONFIG_DEFAULT_HUMI_MIN (200)

/**
 * @brief Minimum supported temperature threshold.
 *
 * Value is represented in tenths of a degree Celsius.
 */
#define CONFIG_TEMP_LIMIT_MIN (-400)

/**
 * @brief Maximum supported temperature threshold.
 *
 * Value is represented in tenths of a degree Celsius.
 */
#define CONFIG_TEMP_LIMIT_MAX (1250)

/**
 * @brief Minimum supported humidity threshold.
 *
 * Value is represented in tenths of a percent.
 */
#define CONFIG_HUMI_LIMIT_MIN (0)

/**
 * @brief Maximum supported humidity threshold.
 *
 * Value is represented in tenths of a percent.
 */
#define CONFIG_HUMI_LIMIT_MAX (1000)

/**
 * @brief Load alert configuration from a file.
 *
 * The configuration file uses key-value pairs in the form:
 * `key=value`.
 *
 * Missing files are handled by loading the default configuration.
 * Malformed lines are skipped with a warning.
 * Out-of-range values are clamped to the supported range.
 *
 * @param[in] p_file_path Path to the configuration file.
 * @param[out] p_config Destination configuration structure.
 *
 * @return STATUS_OK on success.
 * @return STATUS_ERR_NULL_PTR if an argument is NULL.
 * @return STATUS_ERR_INVALID_PARAM if the file cannot be read.
 */
status_t config_load(const char *p_file_path, alert_config_t *p_config);

/**
 * @brief Save alert configuration to a file.
 *
 * The configuration is written using the format:
 * `key=value`.
 *
 * @param[in] p_file_path Path to the configuration file.
 * @param[in] p_config Configuration to save.
 *
 * @return STATUS_OK on success.
 * @return STATUS_ERR_NULL_PTR if an argument is NULL.
 * @return STATUS_ERR_INVALID_PARAM if the file cannot be written.
 */
status_t config_save(const char *p_file_path, const alert_config_t *p_config);

#endif /* CONFIG_H */