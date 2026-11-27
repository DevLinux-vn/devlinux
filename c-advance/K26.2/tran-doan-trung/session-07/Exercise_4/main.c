/**
 * @file main.c
 * @brief Conditional Compilation with Feature Flags Demonstration.
 * @details Implements a Hardware Abstraction Layer (HAL) using preprocessor directives
 *          to conditionally compile network drivers and enforce mutual exclusion.
 *          Complies with MISRA-C 2012 Rule 20.9 and BARR-C standards.
 * @author tran-doan-trung
 * @date 2026-11-26
 */

#include <stdio.h>

/* Auto-select default interface during static analysis (clang-tidy / cppcheck) */
#if (defined(__clang_analyzer__) || defined(__CPPCHECK__)) && \
    !defined(CONFIG_WIFI_ENABLED) && !defined(CONFIG_ETHERNET_ENABLED)
    #define CONFIG_WIFI_ENABLED
#endif

/* Compile-time validation: Ensure valid feature configuration */
#if defined(CONFIG_WIFI_ENABLED) && defined(CONFIG_ETHERNET_ENABLED)
    #error "Cannot enable both WiFi and Ethernet at the same time!"
#elif !defined(CONFIG_WIFI_ENABLED) && !defined(CONFIG_ETHERNET_ENABLED)
    #error "At least one network interface must be enabled!"
#endif

#if defined(CONFIG_WIFI_ENABLED)
/**
 * @brief Initialize the simulated Wi-Fi hardware driver.
 * @details Only compiled when CONFIG_WIFI_ENABLED is defined, saving Flash memory.
 */
static void wifi_driver_init(void)
{
    printf("[NET] Initializing Wi-Fi Driver...\n");
}
#elif defined(CONFIG_ETHERNET_ENABLED)
/**
 * @brief Initialize the simulated Ethernet hardware driver.
 * @details Only compiled when CONFIG_ETHERNET_ENABLED is defined, saving Flash memory.
 */
static void ethernet_driver_init(void)
{
    printf("[NET] Initializing Ethernet Driver...\n");
}
#endif

/**
 * @brief Main entry point initializing the configured network interface.
 * @return 0 on success.
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
