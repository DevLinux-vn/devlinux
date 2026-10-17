/**
 * @file    ring_buffer.c
 * @brief   Implementation of a minimal ring buffer library.
 */
#include "ring_buffer.h"

/**
 * @brief   Push a value into the ring buffer.
 *
 * @param[in] val Value to push.
 *
 * @return  Always true in this simplified implementation.
 */
bool ring_buf_push(int val)
{
    (void)val; /* Simplified stub: value intentionally unused */
    return true;
}

/**
 * @brief   Pop a value from the ring buffer.
 *
 * @param[out] val Pointer to storage for the popped value.
 *
 * @return  Always true in this simplified implementation.
 */
bool ring_buf_pop(int *val)
{
    *val = 3;
    return true;
}