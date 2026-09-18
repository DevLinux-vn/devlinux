/**
 * @file ring_buffer_lf.h
 * @brief Lock-free SPSC ring buffer using the waste-one-slot strategy.
 */

#ifndef RING_BUFFER_LF_H
#define RING_BUFFER_LF_H

#include <stdbool.h>
#include <stdint.h>

#define RB_CAPACITY 8U
#define RB_MASK     (RB_CAPACITY - 1U)

_Static_assert((RB_CAPACITY > 1U), "RB_CAPACITY must be greater than 1");
_Static_assert((RB_CAPACITY & (RB_CAPACITY - 1U)) == 0U,
               "RB_CAPACITY must be a power of 2");

/**
 * @brief Lock-free single-producer single-consumer ring buffer.
 *
 * The producer writes only head.
 * The consumer writes only tail.
 * One slot is intentionally left unused so that:
 * - empty: head == tail
 * - full : next(head) == tail
 */
typedef struct
{
    int16_t buffer[RB_CAPACITY];
    volatile uint32_t head;
    volatile uint32_t tail;
} ring_buf_lf_t;

/** @brief Initialize a ring buffer. @param p_rb Pointer to ring buffer. */
void rb_init(ring_buf_lf_t *p_rb);

/** @brief Check whether the ring buffer is empty. */
bool rb_is_empty(const ring_buf_lf_t *p_rb);

/** @brief Check whether the ring buffer is full. */
bool rb_is_full(const ring_buf_lf_t *p_rb);

/** @brief Push one item. Returns false if full or p_rb is NULL. */
bool rb_push(ring_buf_lf_t *p_rb, int16_t data);

/** @brief Pop one item. Returns false if empty or arguments are invalid. */
bool rb_pop(ring_buf_lf_t *p_rb, int16_t *p_data);

/** @brief Read the oldest item without advancing tail. */
bool rb_peek(const ring_buf_lf_t *p_rb, int16_t *p_data);

/** @brief Return the number of unread items. */
uint32_t rb_available(const ring_buf_lf_t *p_rb);

#endif /* RING_BUFFER_LF_H */
