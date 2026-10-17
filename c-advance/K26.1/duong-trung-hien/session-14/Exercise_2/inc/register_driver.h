#ifndef REGISTER_DRIVER_H
#define REGISTER_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/* ================================================================
 * Basic Bitwise Macros (common in embedded C)
 * ================================================================ */

#define BIT(n)         (1UL << (n))
#define SET_BIT(r, n)  ((r) |=  BIT(n))
#define CLR_BIT(r, n)  ((r) &= ~BIT(n))
#define GENMASK(h, l)  (((~0UL) << (l)) & (~0UL >> (31U - (h))))

/* ================================================================
 * CMSIS-Style Generic Helpers
 * ================================================================ */

/** @brief Insert a value into a register field's position. */
#define _VAL2FLD(field, value) \
    (((uint32_t)(value) << field##_Pos) & field##_Msk)

/** @brief Extract a field's value from a register. */
#define _FLD2VAL(field, reg) \
    (((uint32_t)(reg) & field##_Msk) >> field##_Pos)

/* ================================================================
 * Timer Peripheral Register Field Definitions (CMSIS-style)
 * ================================================================ */

/* CR — Control Register */
#define TIMER_CR_EN_Pos     (0U)
#define TIMER_CR_EN_Msk     (0x1UL << TIMER_CR_EN_Pos)     /* Bit 0: Enable      */

#define TIMER_CR_OPM_Pos    (1U)
#define TIMER_CR_OPM_Msk    (0x1UL << TIMER_CR_OPM_Pos)    /* Bit 1: One-pulse   */

#define TIMER_CR_PSC_Pos    (4U)
#define TIMER_CR_PSC_Msk    (0xFUL << TIMER_CR_PSC_Pos)    /* Bits [7:4]: Prescaler (0–15) */

/* SR — Status Register */
#define TIMER_SR_UIF_Pos    (0U)
#define TIMER_SR_UIF_Msk    (0x1UL << TIMER_SR_UIF_Pos)    /* Bit 0: Update event flag */

/* ================================================================
 * Timer Register Struct (mirrors hardware layout)
 * ================================================================ */

/**
 * @brief Simulated Timer peripheral register block.
 *
 * On real hardware, this struct would be mapped to a fixed base address
 * using: #define TIM2 ((timer_reg_t *)0x40000000UL)
 *
 * For unit testing on a host PC, we allocate this struct in memory.
 */
typedef struct timer_reg_s {
    volatile uint32_t CR;    /**< Offset 0x00: Control Register (R/W).         */
    volatile uint32_t SR;    /**< Offset 0x04: Status Register (R/O on real HW). */
    volatile uint32_t CNT;   /**< Offset 0x08: Counter Register (R/W).         */
    volatile uint32_t ARR;   /**< Offset 0x0C: Auto-Reload Register (R/W).     */
} timer_reg_t;

/* ================================================================
 * Driver API Prototypes
 * ================================================================ */

void     timer_init(timer_reg_t *p_timer);
int32_t  timer_configure(timer_reg_t *p_timer, uint32_t prescaler,
                         uint32_t reload);
void     timer_start(timer_reg_t *p_timer);
void     timer_stop(timer_reg_t *p_timer);
uint32_t timer_read_count(const timer_reg_t *p_timer);
bool     timer_is_event(const timer_reg_t *p_timer);
void     timer_clear_event(timer_reg_t *p_timer);

#endif /* REGISTER_DRIVER_H */