#ifndef PACKET_UTIL_H
#define PACKET_UTIL_H

/**
 * @brief Network packet handle.
 */
typedef struct st_network_packet    *packet_handle_t;

/**
 * @brief Network packet interface (HAL).
 * Contains function pointers for polymorphic packet operations.
 */
typedef struct st_packet_util
{
    /**
     * @brief Allocate a packet from the pool.
     * @return Pointer to packet, or NULL if pool is full.
     */
    packet_handle_t (*Allocate)(void);

    /**
     * @brief Free a packet back to the pool.
     * @param p_packet Pointer to the packet to free.
     */
    void (*Free)(const packet_handle_t p_packet);
} st_packet_util_t;

/**
 * @brief Initialize the packet utility system.
 *
 * Initializes the packet pool and configures the HAL object with the packet
 * allocation and release functions. The HAL object address is returned
 * through the output parameter.
 *
 * @param[out] pp_hal_obj Pointer to the variable that receives the HAL object
 *                       address.
 */
// NOLINTNEXTLINE(readability-identifier-naming)
void init_sys(void **const pp_hal_obj);

#endif