#include "object_pool_manager.h"

static network_packet_t s_pool[POOL_SIZE];
static bool s_in_use[POOL_SIZE];

void pool_init(void)
{
    for (uint8_t i = 0U; i < POOL_SIZE; i++)
    {
        s_in_use[i] = false;
    }
}

network_packet_t* packet_alloc(void)
{
    uint8_t i;
    for (i = 0U; i < POOL_SIZE; i++)
    {
        if (s_in_use[i] == false)
        {
            s_in_use[i] = true;
            s_pool[i].id = i;
            return &s_pool[i];
        }
    }

    return NULL;
}

void packet_free(network_packet_t *p_packet)
{
    if (p_packet == NULL)
    {
        return;
    }

    for (uint8_t i = 0U; i < POOL_SIZE; i++)
    {
        if (p_packet == &s_pool[i])
        {
            s_in_use[p_packet->id] = false;
            return;
        }
    }

    return;
}
