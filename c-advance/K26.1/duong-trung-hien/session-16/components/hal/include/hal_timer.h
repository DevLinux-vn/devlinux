#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include <stdint.h>

/**
 * @brief Initializes the simulated hardware timer.
 *
 * Resets the timer tick counter to its initial value.
 */
void hal_timer_init(void);

/**
 * @brief Gets the current timer tick count.
 *
 * @return Current timer tick value.
 */
uint32_t hal_timer_get_tick(void);

/**
 * @brief Advances the simulated timer by one tick.
 */
void hal_timer_tick(void);

#endif /* HAL_TIMER_H */
