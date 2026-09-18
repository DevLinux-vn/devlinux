/**
 * @file ring_buffer_lf.c
 * @brief Implementation of lock-free SPSC ring buffer operations.
 */

#include <stddef.h>

#include "ring_buffer_lf.h"

void rb_init(ring_buf_lf_t *p_rb)
{
    uint32_t index;

    if (p_rb == NULL)
    {
        return;
    }

    p_rb->head = 0U;
    p_rb->tail = 0U;

    for (index = 0U; index < RB_CAPACITY; ++index)
    {
        p_rb->buffer[index] = 0;
    }
}

bool rb_is_empty(const ring_buf_lf_t *p_rb)
{
    if (p_rb == NULL)
    {
        return true;
    }

    return (p_rb->head == p_rb->tail);
}

bool rb_is_full(const ring_buf_lf_t *p_rb)
{
    uint32_t next_head;

    if (p_rb == NULL)
    {
        return false;
    }

    next_head = (p_rb->head + 1U) & RB_MASK;
    return (next_head == p_rb->tail);
}

bool rb_push(ring_buf_lf_t *p_rb, int16_t data)
{
    uint32_t current_head;
    uint32_t next_head;

    if (p_rb == NULL)
    {
        return false;
    }

    current_head = p_rb->head;
    next_head = (current_head + 1U) & RB_MASK;

    if (next_head == p_rb->tail)
    {
        return false;
    }

    p_rb->buffer[current_head] = data;
    p_rb->head = next_head;

    return true;
}

bool rb_pop(ring_buf_lf_t *p_rb, int16_t *p_data)
{
    uint32_t current_tail;

    if ((p_rb == NULL) || (p_data == NULL))
    {
        return false;
    }

    if (p_rb->head == p_rb->tail)
    {
        return false;
    }

    current_tail = p_rb->tail;
    *p_data = p_rb->buffer[current_tail];
    p_rb->tail = (current_tail + 1U) & RB_MASK;

    return true;
}

bool rb_peek(const ring_buf_lf_t *p_rb, int16_t *p_data)
{
    if ((p_rb == NULL) || (p_data == NULL))
    {
        return false;
    }

    if (p_rb->head == p_rb->tail)
    {
        return false;
    }

    *p_data = p_rb->buffer[p_rb->tail];
    return true;
}

uint32_t rb_available(const ring_buf_lf_t *p_rb)
{
    if (p_rb == NULL)
    {
        return 0U;
    }

    return (p_rb->head - p_rb->tail) & RB_MASK;
}
