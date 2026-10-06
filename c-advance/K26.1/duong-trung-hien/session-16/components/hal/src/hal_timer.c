#include "hal_timer.h"
#include <stdint.h>

static uint32_t s_tick;

void hal_timer_init(void) { s_tick = 0U; }

uint32_t hal_timer_get_tick(void) { return s_tick; }

void hal_timer_tick(void) { s_tick++; }