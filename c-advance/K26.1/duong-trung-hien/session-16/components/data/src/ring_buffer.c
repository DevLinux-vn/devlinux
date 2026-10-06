#include "ring_buffer.h"
#include "common_types.h"
#include "contract.h"
#include <stddef.h>
#include <stdint.h>

void ring_init(ring_buffer_t *p_rb) {
  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_rb != NULL);

  p_rb->head = 0U;
  p_rb->tail = 0U;
  p_rb->count = 0U;
  p_rb->capacity = RING_BUFFER_CAPACITY;

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  INVARIANT(p_rb->count <= p_rb->capacity);
}

void ring_push(ring_buffer_t *p_rb, int16_t value) {
  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_rb != NULL);

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_rb->capacity > 0U);

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(!ring_is_full(p_rb));

  p_rb->data[p_rb->head] = value;
  p_rb->head++;

  if (p_rb->head >= p_rb->capacity) {
    p_rb->head = 0U;
  }

  p_rb->count++;

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  ENSURE(p_rb->count > 0U);

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  INVARIANT(p_rb->count <= p_rb->capacity);
}

status_t ring_pop(ring_buffer_t *p_rb, int16_t *p_value) {
  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_rb != NULL);

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_value != NULL);

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_rb->capacity > 0U);

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(!ring_is_empty(p_rb));

  *p_value = p_rb->data[p_rb->tail];

  p_rb->tail++;

  if (p_rb->tail >= p_rb->capacity) {
    p_rb->tail = 0U;
  }

  p_rb->count--;

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  INVARIANT(p_rb->count <= p_rb->capacity);

  return STATUS_OK;
}

status_t ring_peek(const ring_buffer_t *p_rb, int16_t *p_value) {
  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_rb != NULL);

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_value != NULL);

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_rb->capacity > 0U);

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(!ring_is_empty(p_rb));

  *p_value = p_rb->data[p_rb->tail];

  return STATUS_OK;
}

bool ring_is_full(const ring_buffer_t *p_rb) {
  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_rb != NULL);

  return p_rb->count >= p_rb->capacity;
}

bool ring_is_empty(const ring_buffer_t *p_rb) {
  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_rb != NULL);

  return p_rb->count == 0U;
}

uint32_t ring_count(const ring_buffer_t *p_rb) {
  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(p_rb != NULL);

  return p_rb->count;
}