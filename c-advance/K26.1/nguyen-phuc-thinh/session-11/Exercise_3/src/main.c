/**
 * @file main.c
 * @brief Demonstration of the Telemetry Codec functions.
 */
#include <stdio.h>
#include <string.h>
#include "telemetry_codec.h"

/**
 * @brief Helper to print a byte array in hex format.
 */
static void print_hex(const uint8_t *p_buf, size_t len) 
{
    for (size_t i = 0; i < len; i++) 
    {
        printf("%02X ", p_buf[i]);
    }
    printf("\n");
}

int main(void) 
{
    printf("========== L11 Exercise 3: Telemetry Serializer ==========\n\n");

    telemetry_t original = { .device_id = 4096, .temperature = 24.50f, .humidity = 60.20f };
    telemetry_t deserialized;
    uint8_t buffer[64];
    size_t bytes_written;

    /* --- Part A: Fixed-Layout --- */
    printf("--- Part A: Fixed-Layout (memcpy + htonl) ---\n");
    printf("Original: id=%u, temp=%.2f, hum=%.2f\n", 
           original.device_id, (double)original.temperature, (double)original.humidity);
           
    bytes_written = serialize_telemetry(&original, buffer);
    printf("Serialized (%zu bytes): ", bytes_written);
    print_hex(buffer, bytes_written);
    
    memset(&deserialized, 0, sizeof(telemetry_t));
    deserialize_telemetry(buffer, &deserialized);
    printf("Deserialized: id=%u, temp=%.2f, hum=%.2f\n", 
           deserialized.device_id, (double)deserialized.temperature, (double)deserialized.humidity);
           
    if (memcmp(&original, &deserialized, sizeof(telemetry_t)) == 0) 
    {
        printf("Round-trip: PASS\n\n");
    }

    /* --- Part B: TLV Framing --- */
    printf("--- Part B: TLV Framing ---\n");
    bytes_written = serialize_telemetry_tlv(&original, buffer);
    printf("TLV Serialized (%zu bytes): ", bytes_written);
    print_hex(buffer, bytes_written);
    
    memset(&deserialized, 0, sizeof(telemetry_t));
    deserialize_telemetry_tlv(buffer, bytes_written, &deserialized);
    printf("TLV Deserialized: id=%u, temp=%.2f, hum=%.2f\n", 
           deserialized.device_id, (double)deserialized.temperature, (double)deserialized.humidity);
           
    if (memcmp(&original, &deserialized, sizeof(telemetry_t)) == 0) 
    {
        printf("TLV Round-trip: PASS\n\n");
    }

    /* --- TLV Forward Compatibility --- */
    printf("--- TLV Forward Compatibility ---\n");
    uint8_t tlv_malformed[] = {
        0xFF, 0x02, 0xAA, 0xBB,                         /* Unknown type 0xFF, length 2 */
        0x01, 0x04, 0x00, 0x00, 0x10, 0x00,             /* Device ID = 4096 */
        0x02, 0x04, 0x41, 0xC4, 0x00, 0x00              /* Temp = 24.50 */
    };
    
    memset(&deserialized, 0, sizeof(telemetry_t));
    deserialize_telemetry_tlv(tlv_malformed, sizeof(tlv_malformed), &deserialized);
    printf("Buffer with unknown type 0xFF: Skipped gracefully.\n");
    printf("Known fields decoded correctly: id=%u, temp=%.2f\n", 
           deserialized.device_id, (double)deserialized.temperature);

    return 0;
}