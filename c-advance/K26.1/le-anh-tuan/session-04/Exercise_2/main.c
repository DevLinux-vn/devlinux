/**
 * @file main.c
 * @brief Fixed-size object pool allocator for network packets.
 *
 * This program demonstrates a statically allocated object pool.
 * No dynamic memory allocation is used.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define POOL_SIZE (5U)
#define PAYLOAD_SIZE (64U)

/**
 * @brief Represents a network packet stored in the object pool.
 */
typedef struct
{
    uint32_t id;
    uint8_t payload[PAYLOAD_SIZE];
} network_packet_t;

/**
 * @brief Static storage for network packets.
 *
 * Because this object has static storage duration and is not explicitly
 * initialized, it is zero-initialized before main() executes.
 */
static network_packet_t s_packet_pool[POOL_SIZE];

/**
 * @brief Tracks allocation state of each packet in the pool.
 *
 * false = available
 * true  = allocated
 */
static bool s_packet_in_use[POOL_SIZE];

/**
 * @brief Initialize the network packet object pool.
 *
 * Marks every packet slot as available.
 */
static void pool_init(void)
{
    uint32_t index;

    for (index = 0U; index < POOL_SIZE; ++index)
    {
        s_packet_in_use[index] = false;
    }
}

/**
 * @brief Allocate one network packet from the object pool.
 *
 * The function searches from the beginning of the pool and returns
 * the first available packet.
 *
 * @return Pointer to an allocated packet if one is available.
 * @return NULL if all packets are currently allocated.
 */
static network_packet_t *packet_alloc(void)
{
    uint32_t index;
    network_packet_t *p_packet = NULL;

    for (index = 0U; index < POOL_SIZE; ++index)
    {
        if (s_packet_in_use[index] == false)
        {
            s_packet_in_use[index] = true;
            p_packet = &s_packet_pool[index];
            break;
        }
    }

    return p_packet;
}

/**
 * @brief Return a packet to the object pool.
 *
 * The function accepts NULL safely. It also verifies that the supplied
 * pointer exactly matches one of the objects belonging to the pool.
 *
 * @param p_packet Pointer to the packet that should be released.
 */
static void packet_free(network_packet_t *p_packet)
{
    uint32_t index;

    if (p_packet == NULL)
    {
        return;
    }

    for (index = 0U; index < POOL_SIZE; ++index)
    {
        if (p_packet == &s_packet_pool[index])
        {
            s_packet_in_use[index] = false;
            break;
        }
    }
}

/**
 * @brief Program entry point and object-pool demonstration.
 *
 * Allocates all five packets, verifies that a sixth allocation fails,
 * releases packet 2, and verifies that another allocation succeeds.
 *
 * @return 0 when all tests pass.
 * @return 1 if an unexpected allocation result occurs.
 */
int main(void)
{
    network_packet_t *p_packets[POOL_SIZE];
    network_packet_t *p_extra_packet;
    uint32_t index;

    pool_init();

    for (index = 0U; index < POOL_SIZE; ++index)
    {
        p_packets[index] = packet_alloc();

        if (p_packets[index] != NULL)
        {
            printf(
                "Allocating packet %lu: Success\n",
                (unsigned long)(index + 1U));
        }
        else
        {
            printf(
                "Allocating packet %lu: Failed\n",
                (unsigned long)(index + 1U));

            return 1;
        }
    }

    p_extra_packet = packet_alloc();

    if (p_extra_packet == NULL)
    {
        printf("Allocating packet 6: Failed (Pool Full)\n");
    }
    else
    {
        printf("Allocating packet 6: Unexpected Success\n");
        return 1;
    }

    printf("Freeing packet 2...\n");
    packet_free(p_packets[1]);

    p_packets[1] = NULL;

    p_extra_packet = packet_alloc();

    if (p_extra_packet != NULL)
    {
        printf("Allocating packet 6 again: Success\n");
    }
    else
    {
        printf("Allocating packet 6 again: Failed\n");
        return 1;
    }

    return 0;
}