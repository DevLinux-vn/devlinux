#include <stdint.h>
#include <stdio.h>

#define APP_SUCCESS         (0)

struct device
{
    int32_t dev_id;
};

#define FW_VERSION_MAJOR    3
#define FW_VERSION_MINOR    0
#define FW_VERSION_PATCH    4

/**
 * @brief Generate and define a device struct with unique name via token pasting.
 * @param dev Device type (e.g., spi, i2c)
 * @param id Device ID
 * Expands to: struct device device_<dev>_<id> = { .dev_id = (id) }
 */
#define DEFINE_DEVICE(dev, id)  struct device device_##dev##_##id = { .dev_id = (id) }

/**
 * @brief Stringify preprocessor token (single-level).
 */
#define STRINGIFY(x)        #x

/**
 * @brief Stringify with macro expansion (two-level for expansion before stringify).
 */
#define TO_STRING(x)        STRINGIFY(x)

/**
 * @brief Firmware version string concatenated from version parts.
 */
#define FW_VERSION                      \
    TO_STRING(FW_VERSION_MAJOR) "."     \
    TO_STRING(FW_VERSION_MINOR) "."     \
    TO_STRING(FW_VERSION_PATCH)

#define DT_N_NODELABEL_my_i2c_REG_ADDR  0x40003000U

/**
 * @brief Zephyr Device Tree helper: token paste node_id with _REG_ADDR suffix.
 * @note MISRA-C Rule 20.10 Advisory violation: ## operator used for driver boilerplate,
 *       standard practice in Linux/Zephyr kernel.
 * @param reg Node identifier (e.g., DT_N_NODELABEL_my_i2c)
 * @return Preprocessor-substituted register address constant at compile time.
 */
#define DT_REG_ADDR(reg)                reg##_REG_ADDR

int32_t main(void)
{
    
    DEFINE_DEVICE(spi, 1);
    DEFINE_DEVICE(i2c, 2);
    uint32_t p_i2c_base = DT_REG_ADDR(DT_N_NODELABEL_my_i2c);
    
    printf("=== Exercise 5: Token Pasting & Stringification ===\n");
    printf("Firmware version: %s\n", FW_VERSION);
    printf("Initialized SPI device with ID: %d\n", device_spi_1.dev_id);
    printf("Initialized I2C device with ID: %d\n", device_i2c_2.dev_id);
    printf("I2C reg addr: 0x%08X\n", p_i2c_base);

    return APP_SUCCESS;
}