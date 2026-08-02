#include <stdio.h>
#include <stdint.h>

#define APP_SUCCESS     (0)

#define FW_VERSION_MAJOR 3
#define FW_VERSION_MINOR 0
#define FW_VERSION_PATCH 4

struct device {
    int32_t dev_id;
};

#define DEFINE_DEVICE(name, id)  \
    struct device device##_##name##_##id = { .dev_id = (id) }

#define STRINGFY(x)     #x
#define TO_STRING(x)    STRINGFY(x)
#define FW_VERSION_STRING               \
    TO_STRING(FW_VERSION_MAJOR) "."     \
    TO_STRING(FW_VERSION_MINOR) "."     \
    TO_STRING(FW_VERSION_PATCH)

#define DT_N_NODELABEL_my_i2c_REG_ADDR (0x40003000U)
#define DT_REG_ADDR(node_id)           node_id##_REG_ADDR
    
int32_t main()
{
    DEFINE_DEVICE(spi, 1);
    DEFINE_DEVICE(i2c, 2);
    uint32_t p_i2c_base = DT_REG_ADDR(DT_N_NODELABEL_my_i2c);

    printf("=== Exercise 5: Token Pasting & Stringification ===\n");
    printf("Firmware version: %s\n", FW_VERSION_STRING);
    printf("Initialized SPI device with ID: %d\n", device_spi_1.dev_id);
    printf("Initialized I2C device with ID: %d\n", device_i2c_2.dev_id);
    printf("I2C base address: 0x%08X\n", p_i2c_base);

    return APP_SUCCESS;
}