/**
 * @file main.c
 * @brief Static Object Pool Allocator implementation for network packets.
 * @author Tran Doan Trung
 * @date 2026-11-21
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

/**
 * @brief Maximum number of packets supported in the static object pool.
 */
#define POOL_SIZE 5U

/**
 * @brief Network packet structure.
 */
typedef struct {
    uint32_t id;          /**< Packet identifier */
    uint8_t payload[64];  /**< Data payload buffer */
} network_packet_t;

/**
 * @brief Statically allocated object pool for network packets (.bss section).
 */
static network_packet_t packet_pool[POOL_SIZE];

/**
 * @brief Allocation tracking flags for each slot in the pool.
 */
static bool packet_in_use[POOL_SIZE] = { false };

/**
 * @brief Initialize the object pool.
 * @details Marks all packet slots as unused and clears packet header and payload.
 */
static void pool_init(void) {
    for (uint32_t i = 0U; i < POOL_SIZE; i++) {
        packet_in_use[i] = false;
        packet_pool[i].id = 0U;
        packet_pool[i].payload[0] = 0U;
    }
}

/**
 * @brief Allocate an available packet from the static object pool.
 * @return Pointer to the allocated packet structure, or NULL if the pool is full.
 */
static network_packet_t* packet_alloc(void) {
    for (uint32_t i = 0U; i < POOL_SIZE; i++) {
        if (!packet_in_use[i]) {
            packet_in_use[i] = true;
            return &packet_pool[i];
        }
    }
    return NULL;
}

/**
 * @brief Free an allocated packet back into the object pool.
 * @param[in] p_packet Pointer to the packet structure to be released.
 */
static void packet_free(const network_packet_t *p_packet) {
    if (p_packet == NULL) {
        return;
    }

    for (uint32_t i = 0U; i < POOL_SIZE; i++) {
        if (&packet_pool[i] == p_packet) {
            packet_in_use[i] = false;
            break;
        }
    }
}

/**
 * @brief Main entry point verifying the static object pool allocator.
 * @return 0 on success.
 */
int main(void) {
    network_packet_t *p_packets[POOL_SIZE];

    pool_init();

    /* 1. Allocate 5 packets successfully */
    for (uint32_t i = 0U; i < POOL_SIZE; i++) {
        p_packets[i] = packet_alloc();
        if (p_packets[i] != NULL) {
            p_packets[i]->id = i + 1U;
            (void)printf("Allocating packet %u: Success\n", (unsigned int)(i + 1U));
        }
    }

    /* 2. Attempt to allocate 6th packet (should fail) */
    const network_packet_t *p_extra = packet_alloc();
    if (p_extra == NULL) {
        (void)printf("Allocating packet 6: Failed (Pool Full)\n");
    }

    /* 3. Free packet 2 (index 1) */
    (void)printf("Freeing packet 2...\n");
    packet_free(p_packets[1]);

    /* 4. Re-allocate 6th packet (should now succeed) */
    p_extra = packet_alloc();
    if (p_extra != NULL) {
        (void)printf("Allocating packet 6 again: Success\n");
    }

    return 0;
}
