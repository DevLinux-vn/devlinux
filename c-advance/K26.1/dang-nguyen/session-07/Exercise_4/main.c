#include <stdint.h>
#include <stdio.h>

#define APP_SUCCESS         (0)

#if defined(CONFIG_WIFI_ENABLED) && defined(CONFIG_ETHERNET_ENABLED)
/* error case */
#elif defined(CONFIG_WIFI_ENABLED)
/**
 * @brief Initialize Wi-Fi driver.
 */
static void wifi_driver_init(void)
{
    printf("[NET] Initializing Wi-Fi Driver...\n");
}
#elif defined(CONFIG_ETHERNET_ENABLED)
/**
 * @brief Initialize Ethernet driver.
 */
static void ethernet_driver_init(void)
{
    printf("[NET] Initializing Ethernet Driver...\n");
}
#endif

/**
 * @file main.c
 * @brief Conditional compilation demo with network driver feature flags.
 *
 * This program demonstrates compile-time feature selection using #if defined().
 * Compile with -DCONFIG_WIFI_ENABLED or -DCONFIG_ETHERNET_ENABLED to select driver.
 */
int32_t main(void)
{
    printf("=== Exercise 4: Feature Flags ===\n");

#if defined(CONFIG_WIFI_ENABLED) && defined(CONFIG_ETHERNET_ENABLED)
    #error "Cannot enable both WiFi and Ethernet at the same time!"
#elif defined(CONFIG_WIFI_ENABLED)
    wifi_driver_init();
#elif defined(CONFIG_ETHERNET_ENABLED)
    ethernet_driver_init();
#else
    #error "At least one network interface must be enabled!"
#endif

    return APP_SUCCESS;
}