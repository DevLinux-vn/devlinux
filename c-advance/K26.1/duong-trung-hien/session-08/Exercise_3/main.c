#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_PAYLOAD_SIZE 64
#define START_BYTE 0xAA

typedef struct {
    uint8_t length;
    uint8_t payload[MAX_PAYLOAD_SIZE];
} packet_t;

void parse_packet(const uint8_t* raw_data) {
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