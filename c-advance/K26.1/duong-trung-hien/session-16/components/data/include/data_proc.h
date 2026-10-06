#ifndef DATA_PROC_H
#define DATA_PROC_H

#include <stdint.h>

#include "common_types.h"
#include "ring_buffer.h"

#define DATA_PROC_DEFAULT_WINDOW_SIZE (10U)

/**
 * @brief Moving average filter state.
 */
typedef struct {
  ring_buffer_t buffer;
  int32_t running_sum;
  uint8_t window_size;
} data_proc_t;

/**
 * @brief Initialize the moving average filter.
 *
 * @param p_proc Pointer to filter state.
 * @param window_size Number of samples used for averaging.
 *                   Zero selects DATA_PROC_DEFAULT_WINDOW_SIZE.
 *
 * @return STATUS_OK on success, error status otherwise.
 */
status_t data_proc_init(data_proc_t *p_proc, uint8_t window_size);

/**
 * @brief Add a new sample to the moving average filter.
 *
 * @param p_proc Pointer to filter state.
 * @param sample New sensor sample.
 *
 * @return STATUS_OK on success, error status otherwise.
 */
status_t data_proc_add_sample(data_proc_t *p_proc, int16_t sample);

/**
 * @brief Get the current moving average.
 *
 * @param p_proc Pointer to filter state.
 *
 * @return Current average, or zero if no sample is available.
 */
int16_t data_proc_get_average(const data_proc_t *p_proc);

#endif /* DATA_PROC_H */