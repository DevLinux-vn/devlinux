#ifndef CRC_CALC_H
#define CRC_CALC_H

#include <stdint.h>

/**
 * @brief Calculate the CRC-8 checksum of a data buffer.
 *
 * @param[in] data   Pointer to the input data buffer.
 * @param[in] length Number of bytes in the input data buffer.
 *
 * @return Calculated CRC-8 checksum.
 */
uint8_t crc8_calc(const uint8_t *data, uint32_t length);

#endif /* CRC_CALC_H */