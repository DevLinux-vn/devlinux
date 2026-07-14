#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "packet_util.h"

#define POOL_SIZE       (5U)
#define PAYLOAD_SIZE    (64U)

typedef struct st_network_packet {
    uint32_t id;
    uint8_t payload[PAYLOAD_SIZE];
} st_network_packet_t;

static st_network_packet_t packet_pool[POOL_SIZE];
static bool packet_in_use[POOL_SIZE];

/**
 * @brief Initialize the packet pool.
 *
 * Assigns an identifier to each packet and marks every packet slot as
 * available.
 */
static void pool_init(void);

/**
 * @brief Allocate an available packet from the packet pool.
 *
 * Searches the packet pool for the first available packet, marks it as
 * in use, and returns its handle.
 *
 * @return Handle to the allocated packet.
 * @retval NULL No packet is currently available in the pool.
 */
static packet_handle_t packet_alloc(void);

/**
 * @brief Release a packet back to the packet pool.
 *
 * Searches the packet pool for the specified packet and marks the
 * corresponding pool entry as available.
 *
 * If the packet handle is NULL or does not refer to an element in the packet
 * pool, an error message is printed and no pool entry is modified.
 *
 * @param[in] p_packet Handle to the packet to release.
 */
static void packet_free(const packet_handle_t p_packet);

static void pool_init(void)
{
    for (uint32_t idx = 0U; idx < POOL_SIZE; idx++)
    {
        packet_pool[idx].id = idx;
        packet_in_use[idx]  = false;
    }
}

static packet_handle_t packet_alloc(void)
{
    st_network_packet_t *p_avail_pkt = NULL;

    for (uint32_t idx = 0U; idx < POOL_SIZE; idx++)
    {
        if (false == packet_in_use[idx])
        {
            packet_in_use[idx] = true;
            p_avail_pkt = &packet_pool[idx];
            break;
        }
    }

    return p_avail_pkt;
}

void packet_free(const packet_handle_t p_packet)
{
    if (NULL != p_packet)
    {
        uint32_t idx;

        for (idx = 0U; idx < POOL_SIZE; idx++)
        {
            if (p_packet == &packet_pool[idx])
            {
                packet_in_use[idx] = false;
                break;
            }
        }

        if (POOL_SIZE == idx)
        {
            printf("[ERROR] %s: p_packet does not belong to packet_pool!\n", __func__);
        }
    }
    else
    {
        printf("[ERROR] %s: Invalid packet!\n", __func__);
    }
}

// NOLINTNEXTLINE(readability-identifier-naming)
void init_sys(void **const pp_hal_obj)
{
    static st_packet_util_t hal_obj;

    /* Initialize the pool */
    pool_init();

    /* Initialize HAL object */
    hal_obj.Allocate = &packet_alloc;
    hal_obj.Free     = &packet_free;

    /* Get HAL object */
    *pp_hal_obj = (void *)&hal_obj;
}