#include <stdio.h>

/*
 * MISRA-C 2012 Rule 20.9: identifiers used in the controlling
 * expression of #if / #elif shall be checked with defined(),
 * never referenced directly, so an undefined macro cannot be
 * silently treated as 0.
 */
#if defined(CONFIG_WIFI_ENABLED) && defined(CONFIG_ETHERNET_ENABLED)
    #error "Cannot enable both WiFi and Ethernet at the same time!"
#elif !defined(CONFIG_WIFI_ENABLED) && !defined(CONFIG_ETHERNET_ENABLED)
    #error "At least one network interface must be enabled!"
#endif

#if defined(CONFIG_WIFI_ENABLED)
/**
 * @brief Simulated initialization of the Wi-Fi network driver.
 */
static void wifi_driver_init(void)
{
    printf("[NET] Initializing Wi-Fi Driver...\n");
}
#endif /* CONFIG_WIFI_ENABLED */

#if defined(CONFIG_ETHERNET_ENABLED)
/**
 * @brief Simulated initialization of the Ethernet network driver.
 */
static void ethernet_driver_init(void)
{
    printf("[NET] Initializing Ethernet Driver...\n");
}
#endif /* CONFIG_ETHERNET_ENABLED */

/**
 * @brief Program entry point. Selects and initializes the network
 *        driver that was enabled at compile time.
 *
 * @return 0 on successful completion.
 */
int main(void)
{
    printf("=== Exercise 4: Feature Flags ===\n");

#if defined(CONFIG_WIFI_ENABLED)
    wifi_driver_init();
#elif defined(CONFIG_ETHERNET_ENABLED)
    ethernet_driver_init();
#endif

    return 0;
}