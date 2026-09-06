#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

#define RING_CAPACITY 8U

/**
 * @brief Count-Tracking Circular Ring Buffer with fixed capacity.
 * 
 * Uses count tracking for efficient full/empty checks.
 * Capacity is fixed at compile-time via RING_CAPACITY.
 */
typedef struct {
    uint8_t data[RING_CAPACITY]; /**< Internal storage array. */
    uint32_t head;               /**< Next write index. */
    uint32_t tail;               /**< Next read index. */
    uint32_t count;              /**< Current element count. */
    uint32_t capacity;           /**< Maximum capacity. */
} ring_buffer_t;

/**
 * @brief Initialize ring buffer to empty state.
 * @param p_rb Pointer to ring_buffer_t structure
 * @return void
 */
void ring_init(ring_buffer_t *p_rb);

bool ring_is_full(const ring_buffer_t *p_rb);
bool ring_is_empty(const ring_buffer_t *p_rb);
uint32_t ring_count(const ring_buffer_t *p_rb);

/**
 * @brief Push byte to buffer tail.
 * @param p_rb Pointer to ring buffer
 * @param byte Value to push (0-255)
 * @return void
 * @pre buffer must not be full (ring_is_full() == false)
 */
void ring_push(ring_buffer_t *p_rb, uint8_t byte);

uint8_t ring_pop(ring_buffer_t *p_rb);

#endif /* RING_BUFFER_H */