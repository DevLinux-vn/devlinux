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
 * @brief Parses a UART packet and validates its payload length.
 *
 * The expected packet format is:
 * [START_BYTE] [LENGTH_BYTE] [PAYLOAD...] [CHECKSUM]
 *
 * Before copying the payload, this function verifies that the length
 * provided by the sender does not exceed MAX_PAYLOAD_SIZE. Invalid
 * packets are discarded without performing the memory copy.
 *
 * @param[in] raw_data Pointer to the received UART packet data.
 *
 * @return None.
 */
static void parse_packet(const uint8_t *raw_data)
{
    packet_t pkt;
    uint8_t received_length;

    if (raw_data == NULL)
    {
        printf("ERROR: Invalid data pointer!\n");
        return;
    }

    if (raw_data[0] != START_BYTE)
    {
        printf("ERROR: Invalid start byte!\n");
        return;
    }

    received_length = raw_data[1];

    if (received_length > MAX_PAYLOAD_SIZE)
    {
        printf("ERROR: Packet length (%u) exceeds buffer size!\n",
               (unsigned int)received_length);
        return;
    }

    pkt.length = received_length;

    (void)memcpy(pkt.payload, &raw_data[2], pkt.length);

    printf("Successfully parsed packet of length: %u\n",
           (unsigned int)pkt.length);
}

/**
 * @brief Program entry point for the Secure Serial Parser exercise.
 *
 * Creates a simulated malicious UART packet whose declared payload
 * length exceeds the local packet buffer. The parser must reject the
 * packet before memcpy() is executed.
 *
 * @return 0 on successful program termination.
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

    printf("=== Exercise 3: Secure Serial Parser ===\n");

    parse_packet(malicious_uart_data);

    return 0;
}