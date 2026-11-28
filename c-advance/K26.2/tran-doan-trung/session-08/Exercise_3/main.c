/**
 * @file main.c
 * @brief Secure Serial Parser — Bounds Checking Patch.
 * @details Validates the UART packet length byte against MAX_PAYLOAD_SIZE before
 *          performing memcpy() to prevent stack buffer overflows.
 *          Complies with MISRA-C 2012 Rule 18.1, CERT-C ARR30-C / MEM35-C,
 *          and BARR-C coding standards.
 * @author tran-doan-trung
 * @date 2026-11-28
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

/** @brief Maximum allowed payload size in bytes. */
#define MAX_PAYLOAD_SIZE 64U

/** @brief Protocol frame start delimiter byte. */
#define START_BYTE       0xAAU

/**
 * @brief Parsed UART packet container.
 */
typedef struct {
    uint8_t length;                    /**< Payload length in bytes.  */
    uint8_t payload[MAX_PAYLOAD_SIZE]; /**< Bounded payload buffer.   */
} packet_t;

/**
 * @brief Parse a raw UART frame with strict bounds checking.
 * @details Checks NULL pointer, start byte, and verifies that length does not
 *          exceed MAX_PAYLOAD_SIZE before invoking memcpy().
 * @param[in] p_raw_data Pointer to raw UART byte stream. Must not be NULL.
 */
static void parse_packet(const uint8_t *p_raw_data)
{
    if (NULL == p_raw_data) {
        return;
    }

    if (p_raw_data[0] != START_BYTE) {
        return; /* Invalid start byte */
    }

    /* Bounds check before copying (MISRA Rule 18.1 / CERT ARR30-C / MEM35-C) */
    if (p_raw_data[1] > MAX_PAYLOAD_SIZE) {
        printf("ERROR: Packet length (%u) exceeds buffer size!\n", p_raw_data[1]);
        return;
    }

    packet_t pkt = { 0 };
    pkt.length = p_raw_data[1];
    memcpy(pkt.payload, &p_raw_data[2], pkt.length);

    printf("Successfully parsed packet of length: %u\n", pkt.length);
}

/**
 * @brief Main entry point simulating a malicious oversized UART packet.
 * @return 0 on success.
 */
int main(void)
{
    /* Simulated malicious packet: Claims length is 200, but buffer is only 64! */
    const uint8_t malicious_uart_data[] = { 0xAAU, 200U, 0x01U, 0x02U, 0x03U };

    printf("=== Exercise 3: Secure Serial Parser ===\n");
    parse_packet(malicious_uart_data);

    return 0;
}
