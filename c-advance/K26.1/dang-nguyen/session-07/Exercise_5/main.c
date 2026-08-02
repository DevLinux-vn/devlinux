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

#define DEFINE_DEVICE(dev, id)  struct device device_##dev##_##id = { .dev_id = (id) }

#define STRINGIFY(x)        #x
#define TO_STRING(x)        STRINGIFY(x)

#define FW_VERSION                      \
    TO_STRING(FW_VERSION_MAJOR) "."     \
    TO_STRING(FW_VERSION_MINOR) "."     \
    TO_STRING(FW_VERSION_PATCH)

#define DT_N_NODELABEL_my_i2c_REG_ADDR  0x40003000U

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