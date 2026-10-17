/**
 * @file ring_buffer_lf.h
 * @brief Lock-Free SPSC Ring Buffer definitions.
 */
#ifndef RING_BUFFER_LF_H
#define RING_BUFFER_LF_H

#include <stdint.h>
#include <stdbool.h>

#define RB_CAPACITY 8U
#define RB_MASK     (RB_CAPACITY - 1U)

typedef struct {
    int16_t           buffer[RB_CAPACITY];
    volatile uint32_t head;
    volatile uint32_t tail;
} ring_buf_lf_t;

void rb_init(ring_buf_lf_t *p_rb);
bool rb_is_empty(const ring_buf_lf_t *p_rb);
bool rb_is_full(const ring_buf_lf_t *p_rb);
bool rb_push(ring_buf_lf_t *p_rb, int16_t data);
bool rb_pop(ring_buf_lf_t *p_rb, int16_t *p_data);
bool rb_peek(const ring_buf_lf_t *p_rb, int16_t *p_data);
uint32_t rb_available(const ring_buf_lf_t *p_rb);

#endif /* RING_BUFFER_LF_H */