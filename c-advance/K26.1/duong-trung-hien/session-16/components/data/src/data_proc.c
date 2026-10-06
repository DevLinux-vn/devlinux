#include "data_proc.h"
#include "common_types.h"
#include "contract.h"
#include "ring_buffer.h"
#include <stddef.h>
#include <stdint.h>

status_t data_proc_init(data_proc_t *p_proc, uint8_t window_size) {
  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_proc != NULL);

  if (window_size == 0U) {
    window_size = DATA_PROC_DEFAULT_WINDOW_SIZE;
  }

  if (window_size > RING_BUFFER_CAPACITY) {
    return STATUS_ERR_INVALID_PARAM;
  }

  ring_init(&p_proc->buffer);
  p_proc->running_sum = 0;
  p_proc->window_size = window_size;

  return STATUS_OK;
}

status_t data_proc_add_sample(data_proc_t *p_proc, int16_t sample) {
  int16_t oldest_sample;

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_proc != NULL);

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_proc->window_size > 0U);

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_proc->window_size <= RING_BUFFER_CAPACITY);

  /*
   * The ring buffer capacity is 16, but the moving-average
   * window can be smaller. Therefore, remove the oldest sample
   * when the configured window is already full.
   */
  if (ring_count(&p_proc->buffer) >= p_proc->window_size) {
    if (ring_pop(&p_proc->buffer, &oldest_sample) != STATUS_OK) {
      return STATUS_ERR_INVALID_PARAM;
    }

    p_proc->running_sum -= (int32_t)oldest_sample;
  }

  ring_push(&p_proc->buffer, sample);
  p_proc->running_sum += (int32_t)sample;

  return STATUS_OK;
}

int16_t data_proc_get_average(const data_proc_t *p_proc) {
  uint32_t count;

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_proc != NULL);

  count = ring_count(&p_proc->buffer);

  if (count == 0U) {
    return 0;
  }

  return (int16_t)(p_proc->running_sum / (int32_t)count);
}