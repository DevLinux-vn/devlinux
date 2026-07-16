
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Number of network packet objects managed by the pool.
 */
#define POOL_SIZE (5U)

/**
 * @brief Successful program return status.
 */
#define APP_SUCCESS (0)

/**
 * @brief Failed program return status.
 */
#define APP_FAILURE (1)

/**
 * @brief Size of the payload contained in each network packet.
 */
#define PACKET_PAYLOAD_SIZE (64U)

/**
 * @brief Network packet managed by the object pool.
 *
 * Each packet contains an identifier and a fixed-size payload buffer.
 */
typedef struct network_packet_s
{
    uint32_t id;                            /**< Packet identifier. */
    uint8_t payload[PACKET_PAYLOAD_SIZE];   /**< Packet payload data. */
} network_packet_t;

/**
 * @brief Statically allocated packet storage.
 *
 * Since this object has static storage duration and no explicit initializer,
 * it is zero-initialized before main() starts and is normally placed in .bss.
 */
static network_packet_t s_pool[POOL_SIZE];

/**
 * @brief Allocation state of each packet in the pool.
 *
 * A value of true means that the corresponding packet is allocated. A value
 * of false means that the corresponding packet is available.
 */
static bool s_in_use[POOL_SIZE];

/**
 * @brief Initialize the network packet object pool.
 *
 * All packet slots are marked as available. Packet identifiers and payload
 * bytes are also cleared to provide a deterministic initial state.
 */
static void pool_init(void)
{
    uint32_t packet_index;
    uint32_t payload_index;

    for (packet_index = 0U; packet_index < POOL_SIZE; ++packet_index)
    {
        s_in_use[packet_index] = false;
        s_pool[packet_index].id = 0U;

        for (payload_index = 0U;
             payload_index < PACKET_PAYLOAD_SIZE;
             ++payload_index)
        {
            s_pool[packet_index].payload[payload_index] = 0U;
        }
    }
}

/**
 * @brief Allocate the first available network packet.
 *
 * The function searches the pool from index zero to index POOL_SIZE - 1.
 * The first available packet is marked as allocated and its address is
 * returned.
 *
 * @return Pointer to an allocated packet when a slot is available.
 * @return NULL when every packet in the pool is already allocated.
 */
static network_packet_t *packet_alloc(void)
{
    network_packet_t *p_packet = NULL;
    uint32_t packet_index;

    for (packet_index = 0U; packet_index < POOL_SIZE; ++packet_index)
    {
        if (s_in_use[packet_index] == false)
        {
            s_in_use[packet_index] = true;
            p_packet = &s_pool[packet_index];
            break;
        }
    }

    return p_packet;
}

/**
 * @brief Return a network packet to the object pool.
 *
 * The function safely ignores NULL pointers and pointers that do not identify
 * one of the objects in s_pool.
 *
 * Pointer membership is checked by comparing the supplied pointer with the
 * address of each valid array element. This avoids relational comparisons
 * and pointer subtraction involving pointers that may not belong to the same
 * array.
 *
 * @param[in] p_packet Pointer to the packet to release.
 */
static void packet_free(const network_packet_t *p_packet)
{

    if (p_packet != NULL)
    {
        uint32_t packet_index;
        for (packet_index = 0U; packet_index < POOL_SIZE; ++packet_index)
        {
            if (p_packet == &s_pool[packet_index])
            {
                s_in_use[packet_index] = false;
                break;
            }
        }
    }
}

/**
 * @brief Print the result of a packet allocation attempt.
 *
 * @param[in] packet_number Human-readable packet allocation number.
 * @param[in] p_packet      Pointer returned by packet_alloc().
 */
static void print_allocation_result(uint32_t packet_number,
                                    const network_packet_t *p_packet)
{
    if (p_packet != NULL)
    {
        (void)printf("Allocating packet %lu: Success\n",
                     (unsigned long)packet_number);
    }
    else
    {
        (void)printf("Allocating packet %lu: Failed (Pool Full)\n",
                     (unsigned long)packet_number);
    }
}

/**
 * @brief Program entry point.
 *
 * The test performs the following operations:
 * - Allocates all five available packets.
 * - Verifies that a sixth allocation fails.
 * - Releases packet number two.
 * - Verifies that another allocation succeeds.
 *
 * @return APP_SUCCESS when every test passes.
 * @return APP_FAILURE when an allocation behaves unexpectedly.
 */



 /**
  * @brief comment output gcc -std=c99 -Wall -Wextra -pedantic -Werror main.c -o exercise_2
*./exercise_2
  * Allocating packet 1: Success
  * Allocating packet 2: Success
  * Allocating packet 3: Success
  * Allocating packet 4: Success
  * Allocating packet 5: Success
  * Allocating packet 6: Failed (Pool Full)
  * Freeing packet 2...
  * Allocating packet 6 again: Success
  * 
  * 
  */
int main(void)
{
    network_packet_t *p_packets[POOL_SIZE] =
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    };

    network_packet_t *p_sixth_packet;
    network_packet_t *p_reallocated_packet;
    uint32_t packet_index;
    int32_t return_status = APP_SUCCESS;

    pool_init();

    for (packet_index = 0U; packet_index < POOL_SIZE; ++packet_index)
    {
        p_packets[packet_index] = packet_alloc();

        print_allocation_result(packet_index + 1U,
                                p_packets[packet_index]);

        if (p_packets[packet_index] == NULL)
        {
            return_status = APP_FAILURE;
        }
    }

    p_sixth_packet = packet_alloc();
    print_allocation_result(6U, p_sixth_packet);

    if (p_sixth_packet != NULL)
    {
        return_status = APP_FAILURE;
    }

    (void)printf("Freeing packet 2...\n");
    packet_free(p_packets[1U]);
    p_packets[1U] = NULL;

    p_reallocated_packet = packet_alloc();

    if (p_reallocated_packet != NULL)
    {
        (void)printf("Allocating packet 6 again: Success\n");
    }
    else
    {
        (void)printf("Allocating packet 6 again: Failed\n");
        return_status = APP_FAILURE;
    }

    return return_status;
}
