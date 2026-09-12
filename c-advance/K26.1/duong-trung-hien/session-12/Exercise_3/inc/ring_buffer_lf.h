#ifndef RING_BUFFER_LF_H
#define RING_BUFFER_LF_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define RB_CAPACITY     (8U)
#define RB_MASK         (RB_CAPACITY - 1U)
#define NULL_POINTER    (0U)

/**
 * @brief Ring buffer structure for single-producer/single-consumer operation.
 *
 * @note The ring buffer uses one wasted slot to distinguish between
 *       full and empty states. Therefore, the maximum number of
 *       stored elements is RB_CAPACITY - 1.
 */
typedef struct {

    int16_t buffer[RB_CAPACITY];
    volatile uint32_t head;
    volatile uint32_t tail;

} ring_buf_lf_t;

/**
 * @brief Initializes a ring buffer.
 *
 * @param[in,out] p_rb Pointer to the ring buffer.
 *
 * @note The buffer contents are not modified.
 */
void rb_init(ring_buf_lf_t *p_rb);

/**
 * @brief Checks whether the ring buffer is empty.
 *
 * @param[in] p_rb Pointer to the ring buffer.
 *
 * @return true if the buffer is empty.
 * @return false otherwise.
 */
bool rb_is_empty(const ring_buf_lf_t *p_rb);

/**
 * @brief Checks whether the ring buffer is full.
 *
 * @param[in] p_rb Pointer to the ring buffer.
 *
 * @return true if the buffer is full.
 * @return false otherwise.
 */
bool rb_is_full(const ring_buf_lf_t *p_rb);

/**
 * @brief Pushes an element into the ring buffer.
 *
 * @param[in,out] p_rb Pointer to the ring buffer.
 * @param[in] data Data to be stored.
 *
 * @return true if the data was successfully pushed.
 * @return false if the buffer is full or p_rb is NULL.
 */
bool rb_push(ring_buf_lf_t *p_rb, int16_t data);

/**
 * @brief Pops the oldest element from the ring buffer.
 *
 * @param[in,out] p_rb Pointer to the ring buffer.
 * @param[out] p_data Pointer to store the popped data.
 *
 * @return true if the data was successfully popped.
 * @return false if the buffer is empty or an argument is NULL.
 */
bool rb_pop(ring_buf_lf_t *p_rb, int16_t *p_data);

/**
 * @brief Reads the oldest element without removing it.
 *
 * @param[in] p_rb Pointer to the ring buffer.
 * @param[out] p_data Pointer to store the peeked data.
 *
 * @return true if the data was successfully read.
 * @return false if the buffer is empty or an argument is NULL.
 */
bool rb_peek(const ring_buf_lf_t *p_rb, int16_t *p_data);

/**
 * @brief Returns the number of available elements in the ring buffer.
 *
 * @param[in] p_rb Pointer to the ring buffer.
 *
 * @return Number of elements currently stored.
 */
uint32_t rb_available(const ring_buf_lf_t *p_rb);

#endif /* RING_BUFFER_LF_H */