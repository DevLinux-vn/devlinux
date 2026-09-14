/**
 * @file moving_average.c
 * @brief Implementation of the O(1) moving average filter.
 */

#include <stddef.h>

#include "moving_average.h"

void ma_init(ma_filter_t *p_filt)
{
    uint32_t index;

    if (p_filt == NULL)
    {
        return;
    }

    p_filt->head = 0U;
    p_filt->running_sum = 0;
    p_filt->count = 0U;

    for (index = 0U; index < MA_WINDOW_SIZE; ++index)
    {
        p_filt->buffer[index] = 0;
    }
}

int16_t ma_process(ma_filter_t *p_filt, int16_t new_sample)
{
    int16_t average;

    if (p_filt == NULL)
    {
        return 0;
    }

    if (p_filt->count < MA_WINDOW_SIZE)
    {
        /* Startup phase: do not subtract an old sample yet. */
        p_filt->running_sum += (int32_t)new_sample;
        p_filt->buffer[p_filt->head] = new_sample;
        p_filt->head = (p_filt->head + 1U) & MA_WINDOW_MASK;
        p_filt->count++;

        average = (int16_t)(p_filt->running_sum / (int32_t)p_filt->count);
    }
    else
    {
        /* Full window: replace the oldest sample in O(1). */
        p_filt->running_sum -= (int32_t)p_filt->buffer[p_filt->head];
        p_filt->running_sum += (int32_t)new_sample;
        p_filt->buffer[p_filt->head] = new_sample;
        p_filt->head = (p_filt->head + 1U) & MA_WINDOW_MASK;

        average = (int16_t)(p_filt->running_sum >> MA_SHIFT);
    }

    return average;
}

void ma_reset(ma_filter_t *p_filt)
{
    ma_init(p_filt);
}
