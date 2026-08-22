#include <stdint.h>
#include <stdio.h>

struct device {
    uint32_t dev_id;
};

/* Token pasting — generates a unique variable name at compile time */
#define DEFINE_DEVICE(name, id) \
    struct device device_ ## name ## _ ## id = { .dev_id = (id) }

/* Stringification — two levels ensure macros are expanded before stringifying */
#define STRINGIFY(x)  #x
#define TO_STRING(x)  STRINGIFY(x)

/* Version string — C merges adjacent string literals at compile time */
#define FW_VERSION_MAJOR  3
#define FW_VERSION_MINOR  0
#define FW_VERSION_PATCH  4
/* Hint: FW_VERSION_STRING should produce "3.0.4" */
#define FW_VERSION_STRING \
    TO_STRING(FW_VERSION_MAJOR) "." \
    TO_STRING(FW_VERSION_MINOR) "." \
    TO_STRING(FW_VERSION_PATCH)

/* DT mock — paste node_id with _REG_ADDR to form the full constant name */
#define DT_N_NODELABEL_my_i2c_REG_ADDR 0x40003000U
#define DT_REG_ADDR(node_id)  node_id ## _REG_ADDR

DEFINE_DEVICE(spi, 1);
DEFINE_DEVICE(i2c, 2);

/**
 * @brief Program entry point. Demonstrates token pasting and
 *        stringification via generated device variables, a
 *        compile-time version string, and a mocked register
 *        address lookup.
 *
 * @return 0 on successful completion.
 */
int main(void)
{
    printf("=== Exercise 5: Token Pasting & Stringification ===\n");
    printf("Firmware version: %s\n", FW_VERSION_STRING);
    printf("Initialized SPI device with ID: %u\n", device_spi_1.dev_id);
    printf("Initialized I2C device with ID: %u\n", device_i2c_2.dev_id);

    uint32_t p_i2c_base = DT_REG_ADDR(DT_N_NODELABEL_my_i2c);
    printf("I2C reg addr: 0x%08X\n", p_i2c_base);

    return 0;
}