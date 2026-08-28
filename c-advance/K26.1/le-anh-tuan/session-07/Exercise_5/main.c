#include <stdint.h>
#include <stdio.h>

/*
 * Device structure.
 */
struct device
{
    uint32_t dev_id;
};

/*
 * Token pasting:
 * DEFINE_DEVICE(spi, 1) generates:
 *
 * struct device device_spi_1 = { .dev_id = (1) };
 */
#define DEFINE_DEVICE(name, id) \
    struct device device_ ## name ## _ ## id = { .dev_id = (id) }

/*
 * Two-level stringification.
 *
 * STRINGIFY() converts its argument directly to a string.
 * TO_STRING() allows macro expansion before stringification.
 */
#define STRINGIFY(x)    #x
#define TO_STRING(x)    STRINGIFY(x)

/*
 * Firmware version.
 */
#define FW_VERSION_MAJOR    3
#define FW_VERSION_MINOR    0
#define FW_VERSION_PATCH    4

#define FW_VERSION_STRING                                      \
    TO_STRING(FW_VERSION_MAJOR) "."                            \
    TO_STRING(FW_VERSION_MINOR) "."                            \
    TO_STRING(FW_VERSION_PATCH)

/*
 * Device Tree mock address.
 */
#define DT_N_NODELABEL_my_i2c_REG_ADDR    0x40003000U

/*
 * Token pasting:
 *
 * DT_REG_ADDR(DT_N_NODELABEL_my_i2c)
 *
 * becomes:
 *
 * DT_N_NODELABEL_my_i2c_REG_ADDR
 *
 * which is then expanded to:
 *
 * 0x40003000U
 */
#define DT_REG_ADDR(node_id)    node_id ## _REG_ADDR

/*
 * Generate device objects at compile time.
 */
DEFINE_DEVICE(spi, 1);
DEFINE_DEVICE(i2c, 2);

int main(void)
{
    uint32_t p_i2c_base;

    p_i2c_base = DT_REG_ADDR(DT_N_NODELABEL_my_i2c);

    printf("=== Exercise 5: Token Pasting & Stringification ===\n");
    printf("Firmware version: %s\n", FW_VERSION_STRING);
    printf("Initialized SPI device with ID: %u\n",
           (unsigned int)device_spi_1.dev_id);
    printf("Initialized I2C device with ID: %u\n",
           (unsigned int)device_i2c_2.dev_id);
    printf("I2C reg addr: 0x%08X\n",
           (unsigned int)p_i2c_base);

    return 0;
}
