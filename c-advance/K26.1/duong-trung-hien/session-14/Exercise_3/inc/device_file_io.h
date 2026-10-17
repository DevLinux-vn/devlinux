#ifndef DEVICE_FILE_IO_H
#define DEVICE_FILE_IO_H

#include <stdint.h>

/* ================================================================
 * Register Field Definitions (bit positions and masks)
 * ================================================================ */

/* ENABLED: Bit [0] */
#define REG_ENABLED_Pos (0U)
#define REG_ENABLED_Msk (0x1UL << REG_ENABLED_Pos)

/* MODE: Bits [3:1] */
#define REG_MODE_Pos (1U)
#define REG_MODE_Msk (0x7UL << REG_MODE_Pos)

/* THRESHOLD: Bits [15:8] */
#define REG_THRESHOLD_Pos (8U)
#define REG_THRESHOLD_Msk (0xFFUL << REG_THRESHOLD_Pos)

/* SENSOR_RAW: Bits [31:16] */
#define REG_SENSOR_RAW_Pos (16U)
#define REG_SENSOR_RAW_Msk (0xFFFFUL << REG_SENSOR_RAW_Pos)

/* ================================================================
 * Generic Field Extraction / Insertion Macros
 * ================================================================ */

/** @brief Extract a field value from a register (Linux kernel FIELD_GET style).
 */
#define FIELD_GET(field, reg) (((uint32_t)(reg) & field##_Msk) >> field##_Pos)

/** @brief Prepare a value for insertion into a field (Linux kernel FIELD_PREP
 * style). */
#define FIELD_PREP(field, val) (((uint32_t)(val) << field##_Pos) & field##_Msk)

/* ================================================================
 * Parsed Register Output Struct
 * ================================================================ */

/**
 * @brief Parsed fields from a single register snapshot.
 */
typedef struct reg_fields_s {
  uint32_t enabled;    /**< Bit [0]: 0=off, 1=on.           */
  uint32_t mode;       /**< Bits [3:1]: Operating mode 0–7. */
  uint32_t threshold;  /**< Bits [15:8]: Alert threshold.   */
  uint32_t sensor_raw; /**< Bits [31:16]: Raw ADC reading.  */
} reg_fields_t;

/* ================================================================
 * API Prototypes
 * ================================================================ */

#define MAX_SNAPSHOTS 16U

/**
 * @brief Write an array of register snapshots to a binary file.
 * @return 0 on success, -1 on error.
 */
int32_t reg_snapshot_write(const char *p_path, const uint32_t *p_regs,
                           uint32_t count);

/**
 * @brief Read register snapshots from a binary file.
 * @return Number of registers read (>= 0), -1 on error.
 */
int32_t reg_snapshot_read(const char *p_path, uint32_t *p_regs,
                          uint32_t max_count);

/**
 * @brief Parse a raw register value into individual fields.
 */
void reg_snapshot_parse(uint32_t raw_reg, reg_fields_t *p_out);

#endif /* DEVICE_FILE_IO_H */