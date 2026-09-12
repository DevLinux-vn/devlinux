/**
 * @file    ring_buffer.h
 * @brief   Public interface for a minimal ring buffer library.
 *
 * @details This is a simplified demo interface exposing push/pop
 *          operations for a single integer at a time.
 */
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>

/**
 * @brief   Push a value into the ring buffer.
 *
 * @param[in] val Value to push.
 *
 * @return  true if the push succeeded, false otherwise.
 */
bool ring_buf_push(int val);

/**
 * @brief   Pop a value from the ring buffer.
 *
 * @param[out] val Pointer to storage for the popped value.
 *                 Must not be NULL.
 *
 * @return  true if the pop succeeded, false otherwise.
 */
bool ring_buf_pop(int *val);

#endif /* RING_BUFFER_H */