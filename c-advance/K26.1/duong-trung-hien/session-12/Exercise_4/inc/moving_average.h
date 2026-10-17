#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define MA_WINDOW_SIZE  4U
#define MA_WINDOW_MASK  (MA_WINDOW_SIZE - 1U)
#define MA_SHIFT        2U   /* 1 << 2 = 4 */

#define NULL_POINTER    (-9999)

/**
 * @brief Moving average filter state.
 *
 * Stores the samples and internal state required to calculate
 * the moving average of a fixed-size sliding window.
 */
typedef struct {

    int16_t  buffer[MA_WINDOW_SIZE]; /**< Sample buffer. */
    uint32_t head;                    /**< Index of the next sample to overwrite. */
    int32_t  running_sum;             /**< Sum of all samples currently in the window. */
    uint32_t count;                   /**< Number of valid samples currently stored. */

} ma_filter_t;

/**
 * @brief Initializes a moving average filter.
 *
 * Clears the sample buffer and resets all internal state.
 *
 * @param[in,out] p_filt Pointer to the moving average filter instance.
 *
 * @pre p_filt must not be NULL.
 */
void ma_init(ma_filter_t *p_filt);

/**
 * @brief Processes a new sample through the moving average filter.
 *
 * Adds the new sample to the sliding window and returns
 * the current average of the stored samples.
 *
 * @param[in,out] p_filt    Pointer to the moving average filter instance.
 * @param[in]     new_sample New sample to be processed.
 *
 * @return Current moving average of the samples in the window.
 *
 * @pre p_filt must not be NULL.
 */
int16_t ma_process(ma_filter_t *p_filt, int16_t new_sample);

/**
 * @brief Resets the moving average filter.
 *
 * Clears the sample buffer and resets the filter state,
 * allowing the filter to be reused from an empty state.
 *
 * @param[in,out] p_filt Pointer to the moving average filter instance.
 *
 * @pre p_filt must not be NULL.
 */
void ma_reset(ma_filter_t *p_filt);

#endif /* MOVING_AVERAGE_H */