#include "register_driver.h"
#include <stddef.h>

void timer_init(timer_reg_t *p_timer) {
    if (NULL == p_timer) {
        return;
    }

    p_timer->CR = 0U;
    p_timer->CNT = 0U;
    p_timer->SR = 0U;
    p_timer->ARR = 0U;
}

int32_t timer_configure(timer_reg_t *p_timer, uint32_t prescaler, uint32_t reload) {
    if (NULL == p_timer) {
        return -1;
    }

    if (prescaler > 15) {
        return -1;
    }

    p_timer->CR &= ~TIMER_CR_PSC_Msk;
    p_timer->CR |= _VAL2FLD(TIMER_CR_PSC, prescaler);

    p_timer->ARR = reload;
    
    return 0;
}

void timer_start(timer_reg_t *p_timer) {
    if (NULL == p_timer) {
        return;
    }

    SET_BIT(p_timer->CR, TIMER_CR_EN_Pos);
}

void timer_stop(timer_reg_t *p_timer) {
    if (NULL == p_timer) {
        return;
    }

    CLR_BIT(p_timer->CR, TIMER_CR_EN_Pos);
}

uint32_t timer_read_count(const timer_reg_t *p_timer) {
    if (NULL == p_timer) {
        return UINT32_MAX;
    }

    return p_timer->CNT;
}

bool timer_is_event(const timer_reg_t *p_timer) {
    if (NULL == p_timer) {
        return false;
    }

    return (_FLD2VAL(TIMER_SR_UIF, p_timer->SR) != 0U);
}

void timer_clear_event(timer_reg_t *p_timer) {
    if (NULL == p_timer) {
        return;
    }

    CLR_BIT(p_timer->SR, TIMER_SR_UIF_Pos);
}