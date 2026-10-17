#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_PAYLOAD_SIZE (64U)
#define START_BYTE       (0xAAU)

typedef struct
{
    uint8_t length;
    uint8_t payload[MAX_PAYLOAD_SIZE];
} packet_t;

/**
 * @brief Demonstrates an intentionally vulnerable packet parser.
 *
 * @param[in] raw_data Pointer to raw UART packet data.
 *
 * @return None.
 */
static void parse_packet(const uint8_t *raw_data)
{
    packet_t pkt;

    if (raw_data[0] != START_BYTE)
    {
        return;
    }

    pkt.length = raw_data[1];

    memcpy(pkt.payload, &raw_data[2], pkt.length);

    printf("Successfully parsed packet of length: %u\n",
           (unsigned int)pkt.length);
}

/**
 * @brief Program entry point.
 *
 * @return 0 on program termination.
 */
int main(void)
{
    const uint8_t malicious_uart_data[] =
    {
        0xAAU,
        200U,
        0x01U,
        0x02U,
        0x03U
    };

    parse_packet(malicious_uart_data);

    return 0;
}