/**
 * @file    crc_calc.h
 * @brief   Public interface for the simple CRC-8 calculation library.
 *
 * @details This module provides a single function to compute an 8-bit
 *          checksum (CRC-8) over a byte buffer. The current implementation
 *          uses a simplified XOR-based algorithm.
 */
#ifndef CRC_CALC_H
#define CRC_CALC_H

#include <stdint.h>

/**
 * @brief   Compute the CRC-8 checksum of a data buffer.
 *
 * @param[in] data   Pointer to the buffer to checksum. Must not be NULL
 *                    when @p length is greater than zero.
 * @param[in] length Number of bytes in @p data to process.
 *
 * @return  The computed 8-bit CRC value.
 *
 * @note    This is a simplified XOR-based checksum, not a
 *          polynomial CRC-8 implementation.
 */
uint8_t crc8_calc(const uint8_t *data, uint32_t length);

#endif /* CRC_CALC_H */