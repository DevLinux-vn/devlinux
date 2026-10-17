#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define RING_CAPACITY (8U)
#define RB_EMPTY (0U)

/**
 * @brief Count-Tracking Circular Ring Buffer.
 */
typedef struct {
  uint8_t data[RING_CAPACITY]; /**< Internal storage array. */
  uint32_t head;               /**< Next write index.       */
  uint32_t tail;               /**< Next read index.        */
  uint32_t count;              /**< Current element count.  */
  uint32_t capacity;           /**< Maximum capacity.       */
} ring_buffer_t;

/**
 * @brief Initializes a ring buffer.
 *
 * Resets the ring buffer to an empty state.
 *
 * @param[in,out] p_rb Pointer to the ring buffer.
 */
void ring_init(ring_buffer_t *p_rb);

/**
 * @brief Checks whether the ring buffer is full.
 *
 * @param[in] p_rb Pointer to the ring buffer.
 *
 * @return true if the ring buffer is full, false otherwise.
 */
bool ring_is_full(const ring_buffer_t *p_rb);

/**
 * @brief Checks whether the ring buffer is empty.
 *
 * @param[in] p_rb Pointer to the ring buffer.
 *
 * @return true if the ring buffer is empty, false otherwise.
 */
bool ring_is_empty(const ring_buffer_t *p_rb);

/**
 * @brief Gets the number of bytes currently stored in the ring buffer.
 *
 * @param[in] p_rb Pointer to the ring buffer.
 *
 * @return Number of bytes currently stored in the ring buffer.
 */
uint32_t ring_count(const ring_buffer_t *p_rb);

/**
 * @brief Adds a byte to the ring buffer.
 *
 * @param[in,out] p_rb Pointer to the ring buffer.
 * @param[in] byte Byte to be added to the ring buffer.
 */
void ring_push(ring_buffer_t *p_rb, uint8_t byte);

/**
 * @brief Removes and returns a byte from the ring buffer.
 *
 * @param[in,out] p_rb Pointer to the ring buffer.
 *
 * @return The byte removed from the ring buffer.
 */
uint8_t ring_pop(ring_buffer_t *p_rb);

#endif /* RING_BUFFER_H */