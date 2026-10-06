#ifndef HAL_SIM_H
#define HAL_SIM_H

#include <stdint.h>

/** @brief Simulated register addresses. */
#define HAL_REG_TEMP_RAW (0x00U)
#define HAL_REG_HUMI_RAW (0x01U)
#define HAL_REG_STATUS (0x02U)
#define HAL_REG_COUNT (3U)

/**
 * @brief Initializes the HAL simulation with a specified random seed.
 *
 * @param[in] seed Random seed used to initialize the simulation.
 */
void hal_sim_init(uint32_t seed);

/**
 * @brief Reads a 16-bit value from a simulated register.
 *
 * @param[in] reg_addr Address of the simulated register.
 *
 * @return Register value if the address is valid; UINT16_MAX otherwise.
 */
uint16_t hal_sim_read_register(uint8_t reg_addr);

/**
 * @brief Writes a 16-bit value to a simulated register.
 *
 * @param[in] reg_addr Address of the simulated register.
 * @param[in] value Value to write to the simulated register.
 */
void hal_sim_write_register(uint8_t reg_addr, uint16_t value);

/**
 * @brief Updates the simulated hardware state.
 *
 * This function modifies simulated register values to emulate
 * changes in the hardware over time.
 */
void hal_sim_update(void);

#endif /* HAL_SIM_H */