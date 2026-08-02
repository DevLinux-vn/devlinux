#include <stdint.h>
#include <stdio.h>

#define APP_SUCCESS         (0)

#if defined(CONFIG_WIFI_ENABLED)
static void wifi_driver_init();

static void wifi_driver_init()
{
    printf("[NET] Initializing Wi-Fi Driver...\n");
}
#endif

#if defined(CONFIG_ETHERNET_ENABLED)
static void ethernet_driver_init();

static void ethernet_driver_init()
{
    printf("[NET] Initializing Ethernet Driver...\n");
}
#endif

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