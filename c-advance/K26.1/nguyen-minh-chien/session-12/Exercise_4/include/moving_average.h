/**
 * @file moving_average.h
 * @brief Moving average filter using an O(1) running sum.
 */

#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#include <stdint.h>

#define MA_WINDOW_SIZE  4U
#define MA_WINDOW_MASK  (MA_WINDOW_SIZE - 1U)
#define MA_SHIFT        2U

/**
 * @brief Moving average filter state.
 *
 * The filter stores the latest MA_WINDOW_SIZE samples in a circular buffer.
 * running_sum stores the current sum so each update takes O(1) time.
 */
typedef struct
{
    int16_t buffer[MA_WINDOW_SIZE];
    uint32_t head;
    int32_t running_sum;
    uint32_t count;
} ma_filter_t;

/**
 * @brief Initialize the moving average filter.
 *
 * @param p_filt Pointer to the filter object.
 */
void ma_init(ma_filter_t *p_filt);

/**
 * @brief Process a new sample and return the current filtered average.
 *
 * @param p_filt Pointer to the filter object.
 * @param new_sample New input sample.
 * @return Current filtered average. Returns 0 if p_filt is NULL.
 */
int16_t ma_process(ma_filter_t *p_filt, int16_t new_sample);

/**
 * @brief Reset the filter and clear all previous samples.
 *
 * @param p_filt Pointer to the filter object.
 */
void ma_reset(ma_filter_t *p_filt);

#endif /* MOVING_AVERAGE_H */
