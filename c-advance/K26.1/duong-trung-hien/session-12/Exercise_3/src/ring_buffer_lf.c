#include "ring_buffer_lf.h"

void rb_init(ring_buf_lf_t *p_rb) {
    if (NULL == p_rb) {
        return;
    }

    p_rb->head = 0U;
    p_rb->tail = 0U;
}

bool rb_is_empty(const ring_buf_lf_t *p_rb) {
    if (NULL == p_rb) {
        return false;
    }

    if (p_rb->head == p_rb->tail) {
        return true;
    }

    return false;
}

bool rb_is_full(const ring_buf_lf_t *p_rb) {
    if (NULL == p_rb) {
        return false;
    }

    if (((p_rb->head + 1U) & RB_MASK ) == p_rb->tail) {
        return true;
    }

    return false;
}

bool rb_push(ring_buf_lf_t *p_rb, int16_t data) {
    if (NULL == p_rb) {
        return false;
    }

    if (rb_is_full(p_rb)) {
        return false;
    }

    p_rb->buffer[p_rb->head] = data;
    p_rb->head = (p_rb->head + 1U) & RB_MASK;

    return true;
}

bool rb_pop(ring_buf_lf_t *p_rb, int16_t *p_data) {
    if (NULL == p_rb || NULL == p_data) {
        return false;
    }

    if (rb_is_empty(p_rb)) {
        return false;
    }

    *p_data = p_rb->buffer[p_rb->tail];
    p_rb->tail = (p_rb->tail + 1U) & RB_MASK;

    return true;
}

bool rb_peek(const ring_buf_lf_t *p_rb, int16_t *p_data) {
    if (NULL == p_rb || NULL == p_data) {
        return false;
    }

    if (rb_is_empty(p_rb)) {
        return false;
    }

    *p_data = p_rb->buffer[p_rb->tail];

    return true;
}

uint32_t rb_available(const ring_buf_lf_t *p_rb) {
    if (NULL == p_rb) {
        return NULL_POINTER;
    }

    return (p_rb->head - p_rb->tail) & RB_MASK;
}