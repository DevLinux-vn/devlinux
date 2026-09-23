#ifndef OBJECT_POOL_MANAGER_H
#define OBJECT_POOL_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define POOL_SIZE 5U

/**
 * @brief Network packet object managed by the object pool.
 *
 * Each packet contains an identifier and a fixed-size payload buffer.
 * The object pool manages the lifetime of these packet objects.
 */
typedef struct
{
    uint32_t id;          /**< Packet identifier. */
    uint8_t payload[64];  /**< Packet data buffer. */
} network_packet_t;


/**
 * @brief Initialize the object pool.
 *
 * Marks all packet objects in the pool as available.
 * This function must be called before allocating packets.
 *
 * @note The pool memory is statically allocated and does not use heap memory.
 */
void pool_init(void);


/**
 * @brief Allocate a network packet from the object pool.
 *
 * Searches for the first available packet object, marks it as in-use,
 * and returns its address.
 *
 * @return Pointer to an allocated network packet.
 * @return NULL if all packet objects in the pool are already allocated.
 */
network_packet_t *packet_alloc(void);


/**
 * @brief Release a network packet back to the object pool.
 *
 * @param[in] p_packet Pointer to the network packet object to release.
 */
void packet_free(network_packet_t *p_packet);


#endif /* OBJECT_POOL_MANAGER_H */