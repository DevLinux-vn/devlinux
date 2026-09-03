/**
 * @file moving_average.h
 * @brief O(1) Moving Average Filter definitions.
 */
#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#include <stdint.h>

#define MA_WINDOW_SIZE 4U
#define MA_WINDOW_MASK (MA_WINDOW_SIZE - 1U)
#define MA_SHIFT       2U  /* 1 << 2 = 4 */

/**
 * @brief Filter state structure.
 */
typedef struct {
    int16_t  buffer[MA_WINDOW_SIZE];
    uint32_t head;
    int32_t  running_sum;
    uint32_t count;
} ma_filter_t;

void ma_init(ma_filter_t *p_filt);
void ma_reset(ma_filter_t *p_filt);
int16_t ma_process(ma_filter_t *p_filt, int16_t new_sample);

#endif /* MOVING_AVERAGE_H */