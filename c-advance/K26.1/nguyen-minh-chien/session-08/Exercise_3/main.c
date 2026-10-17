#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_PAYLOAD_SIZE 64
#define START_BYTE 0xAA

typedef struct {
    uint8_t length;
    uint8_t payload[MAX_PAYLOAD_SIZE];
} packet_t;

/**
 * @brief Parses a raw UART packet frame and copies its payload safely.
 *
 * Validates the start byte and the length byte supplied by the sender
 * before copying payload data into a fixed-size buffer, preventing
 * a buffer overflow if the sender reports an invalid length.
 *
 * @param raw_data Pointer to the raw byte buffer received from UART.
 *                 Expected layout: [START_BYTE][LENGTH_BYTE][PAYLOAD...].
 */

void parse_packet(const uint8_t* raw_data) {
    packet_t pkt;
    
    if (raw_data[0] != START_BYTE) {
        return; /* Invalid start */
    }
    
    pkt.length = raw_data[1];
    /* Always clamp/check lengths before memory operations */
    if (raw_data[1] > MAX_PAYLOAD_SIZE) {
        printf("ERROR: Packet length (%d) exceeds buffer size!\n", raw_data[1]);
        return;
    }
    /* ⚠️ DANGER: Blindly trusting sender's length! Buffer overflow possible. */
    memcpy(pkt.payload, &raw_data[2], pkt.length); 
    
    printf("Successfully parsed packet of length: %d\n", pkt.length);
}

/**
 * @brief Entry point simulating reception of a malicious UART packet.
 *
 * Builds a fake packet claiming a payload length larger than the
 * receive buffer can hold, then passes it to parse_packet() to
 * demonstrate that the bounds check safely rejects it.
 *
 * @return 0 on successful program termination.
 */

int main(void) {
    /* Simulated malicious packet: Claims length is 200, but buffer is only 64! */
    uint8_t malicious_uart_data[] = {0xAA, 200, 0x01, 0x02, 0x03}; 
    printf("=== Exercise 3: Secure Serial Parser ===\n");
    parse_packet(malicious_uart_data);
    return 0;
}