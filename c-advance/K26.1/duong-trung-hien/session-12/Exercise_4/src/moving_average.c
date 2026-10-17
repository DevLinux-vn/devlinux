#include "moving_average.h"

void ma_init(ma_filter_t *p_filt) {
    if (NULL == p_filt) {
        return;
    }

    p_filt->head = 0U;
    p_filt->running_sum = 0;
    p_filt->count = 0U;
}

int16_t ma_process(ma_filter_t *p_filt, int16_t new_sample) {
    if (NULL == p_filt) {
        return NULL_POINTER;
    }

    if (p_filt->count < MA_WINDOW_SIZE) {
        p_filt->buffer[p_filt->head] = new_sample;
        p_filt->running_sum += p_filt->buffer[p_filt->head];

        p_filt->head = (p_filt->head + 1U) & MA_WINDOW_MASK;
        p_filt->count++;

        return (int16_t)(p_filt->running_sum / p_filt->count);
    }

    p_filt->running_sum -= p_filt->buffer[p_filt->head];
    p_filt->running_sum += new_sample;

    p_filt->buffer[p_filt->head] = new_sample;
    p_filt->head = (p_filt->head + 1U) & MA_WINDOW_MASK;

    return (int16_t)(p_filt->running_sum >> MA_SHIFT);
}

void ma_reset(ma_filter_t *p_filt) {
    if (NULL == p_filt) {
        return;
    }

    memset(p_filt->buffer, 0U, sizeof(p_filt->buffer));
    p_filt->head = 0U;
    p_filt->running_sum = 0;
    p_filt->count = 0U;
}