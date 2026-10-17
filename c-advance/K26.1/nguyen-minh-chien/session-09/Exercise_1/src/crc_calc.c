/**
 * @file    crc_calc.c
 * @brief   Implementation of the simple CRC-8 calculation library.
 */
#include "crc_calc.h"

/**
 * @brief   Compute the CRC-8 checksum of a data buffer.
 *
 * @param[in] data   Pointer to the buffer to checksum.
 * @param[in] length Number of bytes in @p data to process.
 *
 * @return  The computed 8-bit CRC value (XOR of all bytes).
 */
uint8_t crc8_calc(const uint8_t *data, uint32_t length)
{
    uint8_t crc = 0U;
    uint32_t i;

    for (i = 0U; i < length; i++)
    {
        crc ^= data[i]; /* Simplified CRC: XOR accumulation */
    }

    return crc;
}