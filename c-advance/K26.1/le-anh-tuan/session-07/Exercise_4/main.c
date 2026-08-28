#include <stdio.h>

/*
 * Compile-time configuration validation.
 */
#if defined(CONFIG_WIFI_ENABLED) && defined(CONFIG_ETHERNET_ENABLED)
#error "Cannot enable both WiFi and Ethernet at the same time!"
#elif !defined(CONFIG_WIFI_ENABLED) && !defined(CONFIG_ETHERNET_ENABLED)
#error "At least one network interface must be enabled!"
#endif

/*
 * Only compile the driver that is actually enabled.
 */
#if defined(CONFIG_WIFI_ENABLED)

static void wifi_driver_init(void)
{
    (void)printf("[NET] Initializing Wi-Fi Driver...\n");
}

#elif defined(CONFIG_ETHERNET_ENABLED)

static void ethernet_driver_init(void)
{
    (void)printf("[NET] Initializing Ethernet Driver...\n");
}

#endif

int main(void)
{
    (void)printf("=== Exercise 4: Feature Flags ===\n");

#if defined(CONFIG_WIFI_ENABLED)
    wifi_driver_init();
#elif defined(CONFIG_ETHERNET_ENABLED)
    ethernet_driver_init();
#endif

    return 0;
}