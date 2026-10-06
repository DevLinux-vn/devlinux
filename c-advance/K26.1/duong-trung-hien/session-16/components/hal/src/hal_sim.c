#include "hal_sim.h"
#include <stdint.h>
#include <stdlib.h>

static volatile uint16_t s_registers[HAL_REG_COUNT];

void hal_sim_init(uint32_t seed) { srand(seed); }

uint16_t hal_sim_read_register(uint8_t reg_addr) {
  if (reg_addr >= HAL_REG_COUNT) {
    return UINT16_MAX;
  }

  return s_registers[reg_addr];
}

void hal_sim_write_register(uint8_t reg_addr, uint16_t value) {
  s_registers[reg_addr] = value;
}

void hal_sim_update(void) {
  s_registers[HAL_REG_TEMP_RAW] = (uint16_t)rand(); // NOLINT

  s_registers[HAL_REG_HUMI_RAW] = (uint16_t)rand(); // NOLINT
}