#ifndef TIMER_CALLBACK_H
#define TIMER_CALLBACK_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * @brief Timer expiration callback function type.
 *
 * This callback is invoked automatically when the timer reaches
 * the registered expiration tick.
 */
typedef void (*timer_callback_t)(void);

/**
 * @brief Register a software timer.
 *
 * Configures the timer with an expiration tick and a callback
 * function to be executed when the timer expires.
 *
 * @param[in] expire_at_tick Tick value at which the timer expires.
 * @param[in] callback Pointer to the callback function.
 */
void Timer_Register(uint32_t expire_at_tick, timer_callback_t callback);

/**
 * @brief Advance the timer by one tick.
 *
 * Increments the internal tick counter. If the current tick reaches
 * the registered expiration tick, the registered callback is executed.
 */
void Timer_Tick(void);

/**
 * @brief Reset the timer.
 *
 * Clears the current tick count and stops the timer.
 * Any registered callback is also cleared.
 */
void Timer_Reset(void);

/**
 * @brief Check whether the timer is running.
 *
 * @retval true  The timer is currently active.
 * @retval false The timer is stopped.
 */
bool Timer_IsRunning(void);

/**
 * @brief Get the current timer tick.
 *
 * @return Current internal tick count.
 */
uint32_t Timer_GetCurrentTick(void);

#endif /* TIMER_CALLBACK_H */