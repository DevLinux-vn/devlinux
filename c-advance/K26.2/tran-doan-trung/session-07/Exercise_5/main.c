/**
 * @file main.c
 * @brief Token Pasting & Stringification (## and #) Demonstration.
 */

#include <stdint.h>
#include <stdio.h>

/**
 * @brief Simulated hardware device descriptor structure.
 */
struct device {
    uint32_t dev_id; /**< Unique hardware device identifier. */
};

/**
 * @brief Generate a uniquely named device struct variable at compile time.
 * @param[in] name Peripheral bus name (e.g., spi, i2c).
 * @param[in] id   Numeric device identifier.
 */
#define DEFINE_DEVICE(name, id) \
    struct device device_ ## name ## _ ## id = { .dev_id = (uint32_t)(id) }

/**
 * @brief Convert a token directly into a string literal (Level 1).
 * @param[in] x Token to stringify.
 */
#define STRINGIFY(x)  #x

/**
 * @brief Expand a macro before converting it into a string literal (Level 2).
 * @param[in] x Macro or token to expand and stringify.
 */
#define TO_STRING(x)  STRINGIFY(x)

/** @brief Firmware major version number. */
#define FW_VERSION_MAJOR  3
/** @brief Firmware minor version number. */
#define FW_VERSION_MINOR  0
/** @brief Firmware patch version number. */
#define FW_VERSION_PATCH  4

/**
 * @brief Full firmware version string concatenated at compile time ("3.0.4").
 */
#define FW_VERSION_STRING \
    TO_STRING(FW_VERSION_MAJOR) "." TO_STRING(FW_VERSION_MINOR) "." TO_STRING(FW_VERSION_PATCH)

/** @brief Mock Zephyr Device Tree register base address for I2C node. */
#define DT_N_NODELABEL_my_i2c_REG_ADDR  0x40003000U

/**
 * @brief Look up a Device Tree node's register address via token pasting.
 * @param[in] node_id Device Tree node label identifier.
 */
#define DT_REG_ADDR(node_id)  node_id ## _REG_ADDR

/**
 * @brief Main entry point verifying token pasting and stringification macros.
 * @return 0 on success.
 */
int main(void)
{
    DEFINE_DEVICE(spi, 1);
    DEFINE_DEVICE(i2c, 2);

    const uint32_t i2c_reg_addr = DT_REG_ADDR(DT_N_NODELABEL_my_i2c);

    printf("=== Exercise 5: Token Pasting & Stringification ===\n");
    printf("Firmware version: %s\n", FW_VERSION_STRING);
    printf("Initialized SPI device with ID: %u\n", device_spi_1.dev_id);
    printf("Initialized I2C device with ID: %u\n", device_i2c_2.dev_id);
    printf("I2C reg addr: 0x%08X\n", i2c_reg_addr);

    return 0;
}
