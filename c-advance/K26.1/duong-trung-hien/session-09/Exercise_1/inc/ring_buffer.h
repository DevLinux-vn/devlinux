#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>

/**
 * @brief Push a value into the ring buffer.
 *
 * @param[in] val Value to be added to the ring buffer.
 *
 * @return true if the value was successfully pushed,
 *         false if the ring buffer is full.
 */
bool ring_buf_push(int val);

/**
 * @brief Pop a value from the ring buffer.
 *
 * @param[out] val Pointer to store the value removed from the ring buffer.
 *
 * @return true if a value was successfully popped,
 *         false if the ring buffer is empty.
 */
bool ring_buf_pop(int *val);

#endif /* RING_BUFFER_H */