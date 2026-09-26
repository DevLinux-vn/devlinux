#include "register_driver.h"
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

/**
 * @brief Initializes the timer registers.
 *
 * Resets the control register, counter, status register, and
 * auto-reload register to zero.
 *
 * @param[in,out] p_timer Pointer to the timer register structure.
 *
 * @return None.
 */
void timer_init(timer_reg_t *p_timer) {
    if (NULL == p_timer) {
        return;
    }

    p_timer->CR = 0U;
    p_timer->CNT = 0U;
    p_timer->SR = 0U;
    p_timer->ARR = 0U;
}

/**
 * @brief Configures the timer prescaler and auto-reload value.
 *
 * Updates the prescaler field in the control register and sets
 * the auto-reload register to the specified reload value.
 *
 * @param[in,out] p_timer Pointer to the timer register structure.
 * @param[in] prescaler Timer prescaler value. Valid range is 0 to 15.
 * @param[in] reload Auto-reload value.
 *
 * @return 0 on success.
 * @return -1 if p_timer is NULL or prescaler is out of range.
 */
int32_t timer_configure(timer_reg_t *p_timer, uint32_t prescaler,
                        uint32_t reload) {
    if (NULL == p_timer) {
        return -1;
    }

    if (prescaler > 15U) {
        return -1;
    }

    p_timer->CR &= ~TIMER_CR_PSC_Msk;
    p_timer->CR |= _VAL2FLD(TIMER_CR_PSC, prescaler);

    p_timer->ARR = reload;

    return 0;
}

/**
 * @brief Starts the timer.
 *
 * Sets the timer enable bit in the control register.
 *
 * @param[in,out] p_timer Pointer to the timer register structure.
 *
 * @return None.
 */
void timer_start(timer_reg_t *p_timer) {
    if (NULL == p_timer) {
        return;
    }

    SET_BIT(p_timer->CR, TIMER_CR_EN_Pos);
}

/**
 * @brief Stops the timer.
 *
 * Clears the timer enable bit in the control register.
 *
 * @param[in,out] p_timer Pointer to the timer register structure.
 *
 * @return None.
 */
void timer_stop(timer_reg_t *p_timer) {
    if (NULL == p_timer) {
        return;
    }

    CLR_BIT(p_timer->CR, TIMER_CR_EN_Pos);
}

/**
 * @brief Reads the current timer counter value.
 *
 * @param[in] p_timer Pointer to the timer register structure.
 *
 * @return Current timer counter value.
 * @return UINT32_MAX if p_timer is NULL.
 */
uint32_t timer_read_count(const timer_reg_t *p_timer) {
    if (NULL == p_timer) {
        return UINT32_MAX;
    }

    return p_timer->CNT;
}

/**
 * @brief Checks whether a timer update event has occurred.
 *
 * Reads the update interrupt flag (UIF) from the status register.
 *
 * @param[in] p_timer Pointer to the timer register structure.
 *
 * @return true if the update event flag is set.
 * @return false if the flag is cleared or p_timer is NULL.
 */
bool timer_is_event(const timer_reg_t *p_timer) {
    if (NULL == p_timer) {
        return false;
    }

    return (_FLD2VAL(TIMER_SR_UIF, p_timer->SR) != 0U);
}

/**
 * @brief Clears the timer update event flag.
 *
 * Clears the update interrupt flag (UIF) in the status register.
 *
 * @param[in,out] p_timer Pointer to the timer register structure.
 *
 * @return None.
 */
void timer_clear_event(timer_reg_t *p_timer) {
    if (NULL == p_timer) {
        return;
    }

    CLR_BIT(p_timer->SR, TIMER_SR_UIF_Pos);
}