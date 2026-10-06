#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

#include "common_types.h"

#define RING_BUFFER_CAPACITY (16U)

/**
 * @brief Count-tracking ring buffer for int16_t sensor samples.
 */
typedef struct {
  int16_t data[RING_BUFFER_CAPACITY];
  uint32_t head;
  uint32_t tail;
  uint32_t count;
  uint32_t capacity;
} ring_buffer_t;

/**
 * @brief Initialize a ring buffer.
 *
 * @param p_rb Pointer to ring buffer.
 */
void ring_init(ring_buffer_t *p_rb);

/**
 * @brief Add a value to the ring buffer.
 *
 * @param p_rb Pointer to ring buffer.
 * @param value Value to add.
 */
void ring_push(ring_buffer_t *p_rb, int16_t value);

/**
 * @brief Remove the oldest value from the ring buffer.
 *
 * @param p_rb Pointer to ring buffer.
 * @param p_value Pointer receiving the removed value.
 *
 * @return STATUS_OK on success, error status otherwise.
 */
status_t ring_pop(ring_buffer_t *p_rb, int16_t *p_value);

/**
 * @brief Read the oldest value without removing it.
 *
 * @param p_rb Pointer to ring buffer.
 * @param p_value Pointer receiving the oldest value.
 *
 * @return STATUS_OK on success, error status otherwise.
 */
status_t ring_peek(const ring_buffer_t *p_rb, int16_t *p_value);

/**
 * @brief Check whether the ring buffer is full.
 *
 * @param p_rb Pointer to ring buffer.
 *
 * @return true if full, otherwise false.
 */
bool ring_is_full(const ring_buffer_t *p_rb);

/**
 * @brief Check whether the ring buffer is empty.
 *
 * @param p_rb Pointer to ring buffer.
 *
 * @return true if empty, otherwise false.
 */
bool ring_is_empty(const ring_buffer_t *p_rb);

/**
 * @brief Get the number of stored elements.
 *
 * @param p_rb Pointer to ring buffer.
 *
 * @return Number of stored elements.
 */
uint32_t ring_count(const ring_buffer_t *p_rb);

#endif /* RING_BUFFER_H */