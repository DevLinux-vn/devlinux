#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_PAYLOAD_SIZE 64
#define START_BYTE 0xAA

/**
 * @brief Represents a parsed UART packet.
 *
 * Stores the payload length and payload data extracted from a UART
 * packet after validation.
 */
typedef struct {
    uint8_t length;                          /**< Number of bytes in the payload. */
    uint8_t payload[MAX_PAYLOAD_SIZE];       /**< Payload data buffer. */
} packet_t;

/**
 * @brief Parses and validates a UART packet.
 *
 * This function verifies that the packet begins with the expected start
 * byte and that the payload length does not exceed the maximum supported
 * payload size. If the packet is valid, the payload is copied into a
 * local packet structure. Invalid packets are safely rejected without
 * performing any memory copy.
 *
 * @param raw_data Pointer to the raw UART packet data. The expected
 *                 packet format is:
 *                 [START_BYTE][LENGTH_BYTE][PAYLOAD...][CHECKSUM]
 */
void parse_packet(const uint8_t *raw_data)
{
    packet_t pkt;

    if (raw_data[0] != START_BYTE) {
        return; /* Invalid start */
    }

    /* Always clamp/check lengths before memory operations */
    if (raw_data[1] > MAX_PAYLOAD_SIZE) {
        printf("ERROR: Packet length (%d) exceeds buffer size!\n", raw_data[1]);
        return;
    }

    pkt.length = raw_data[1];

    memcpy(pkt.payload, &raw_data[2], pkt.length);

    printf("Successfully parsed packet of length: %d\n", pkt.length);
}

int main(void) {
    printf("=== Exercise 3: Secure Serial Parser ===\n");
    uint8_t malicious_uart_data[] = {0xAA, 200, 0x01, 0x02, 0x03};
    parse_packet(malicious_uart_data);
    return 0;
}