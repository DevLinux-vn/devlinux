#include "register_driver.h"
#include <stddef.h>

void timer_init(timer_reg_t *p_timer)
{
    if (p_timer != NULL)
    {
        p_timer->CR  = 0x00000000UL;
        p_timer->SR  = 0x00000000UL;
        p_timer->CNT = 0x00000000UL;
        p_timer->ARR = 0x00000000UL;
    }
}

int32_t timer_configure(timer_reg_t *p_timer, uint32_t prescaler, uint32_t reload)
{
    if ((p_timer == NULL) || (prescaler > 15U))
    {
        return -1;
    }

    uint32_t temp = p_timer->CR;
    temp &= ~TIMER_CR_PSC_Msk; 
    temp |= _VAL2FLD(TIMER_CR_PSC, prescaler);
    p_timer->CR = temp;

    p_timer->ARR = reload;

    return 0;
}

void timer_start(timer_reg_t *p_timer)
{
    if (p_timer != NULL)
    {
        uint32_t temp = p_timer->CR;
        temp |= TIMER_CR_EN_Msk;
        p_timer->CR = temp;
    }
}

void timer_stop(timer_reg_t *p_timer)
{
    if (p_timer != NULL)
    {
        uint32_t temp = p_timer->CR;
        temp &= ~TIMER_CR_EN_Msk;
        p_timer->CR = temp;
    }
}

uint32_t timer_read_count(const timer_reg_t *p_timer)
{
    if (p_timer == NULL)
    {
        return 0U;
    }
    return p_timer->CNT;
}

bool timer_is_event(const timer_reg_t *p_timer)
{
    if (p_timer == NULL)
    {
        return false;
    }
    return ((p_timer->SR & TIMER_SR_UIF_Msk) != 0U);
}

void timer_clear_event(timer_reg_t *p_timer)
{
    if (p_timer != NULL)
    {
        uint32_t temp = p_timer->SR;
        temp &= ~TIMER_SR_UIF_Msk;
        p_timer->SR = temp;
    }
}