#include "moving_average.h"
#include <stddef.h>

void ma_reset(ma_filter_t *p_filt)
{
    if (p_filt != NULL)
    {
        p_filt->head = 0U;
        p_filt->running_sum = 0;
        p_filt->count = 0U;
        for (uint32_t i = 0U; i < MA_WINDOW_SIZE; ++i)
        {
            p_filt->buffer[i] = 0;
        }
    }
}

void ma_init(ma_filter_t *p_filt)
{
    ma_reset(p_filt);
}

int16_t ma_process(ma_filter_t *p_filt, int16_t new_sample)
{
    if (p_filt == NULL)
    {
        return 0;
    }

    if (p_filt->count >= MA_WINDOW_SIZE)
    {
        p_filt->running_sum -= p_filt->buffer[p_filt->head];
    }
    else
    {
        p_filt->count++;
    }

    p_filt->running_sum += new_sample;
    p_filt->buffer[p_filt->head] = new_sample;
    p_filt->head = (p_filt->head + 1U) & MA_WINDOW_MASK;

    if (p_filt->count >= MA_WINDOW_SIZE)
    {
        return (int16_t)(p_filt->running_sum >> MA_SHIFT);
    }
    
    return (int16_t)(p_filt->running_sum / (int32_t)p_filt->count);
}