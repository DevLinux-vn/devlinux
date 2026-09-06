#include "ring_buffer.h"
#include "contract.h"
#include <stddef.h>

void ring_init(ring_buffer_t *p_rb)
{
    REQUIRE(p_rb != NULL);

    p_rb->head = 0U;
    p_rb->tail = 0U;
    p_rb->count = 0U;
    p_rb->capacity = RING_CAPACITY;

    INVARIANT(p_rb->count <= p_rb->capacity);
}

bool ring_is_full(const ring_buffer_t *p_rb)
{
    REQUIRE(p_rb != NULL);
    return (p_rb->count >= p_rb->capacity);
}

bool ring_is_empty(const ring_buffer_t *p_rb)
{
    REQUIRE(p_rb != NULL);
    return (p_rb->count == 0U);
}

uint32_t ring_count(const ring_buffer_t *p_rb)
{
    REQUIRE(p_rb != NULL);
    return p_rb->count;
}

void ring_push(ring_buffer_t *p_rb, uint8_t byte)
{
    REQUIRE(p_rb != NULL);
    REQUIRE(!ring_is_full(p_rb));

    p_rb->data[p_rb->head] = byte;
    p_rb->head = (p_rb->head + 1U) % p_rb->capacity;
    p_rb->count++;

    ENSURE(p_rb->count > 0U);
    INVARIANT(p_rb->count <= p_rb->capacity);
}

uint8_t ring_pop(ring_buffer_t *p_rb)
{
    REQUIRE(p_rb != NULL);
    REQUIRE(!ring_is_empty(p_rb));

    uint8_t byte = p_rb->data[p_rb->tail];
    p_rb->tail = (p_rb->tail + 1U) % p_rb->capacity;
    p_rb->count--;

    INVARIANT(p_rb->count <= p_rb->capacity);
    
    return byte;
}