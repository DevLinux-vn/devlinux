#include "telemetry_codec.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define APP_SUCCESS     (0)
#define BUFFER_SIZE     (64U)

int32_t main() {
    printf("========== L11 Exercise 3: Telemetry Serializer ==========\n\n");

    telemetry_t original = {
        .device_id = 4096U,
        .temperature = 24.50f,
        .humidity = 60.20f
    };
    telemetry_t restored = {0};
    uint8_t buf[BUFFER_SIZE] = {0};
    size_t count = 0U;
    
    /*------------------Fixed-Layout----------------------*/
    printf("--- Part A: Fixed-Layout (memcpy + htonl) ---\n");
    count = serialize_telemetry(&original, buf);

    printf("Original: id=%u, temp=%.2f, hum=%.2f\n", original.device_id,
                                                     original.temperature,
                                                     original.humidity
    );
    printf("Serialized (%zu bytes): ", count);
    for (uint32_t i = 0U; i < count; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\n");

    deserialize_telemetry(buf, &restored);
    printf("Deserialized: id=%u, temp=%.2f, hum=%.2f\n", restored.device_id, 
                                                         restored.temperature, 
                                                         restored.humidity
    );
    printf("Round-trip: %s\n\n", check_round_trip(&original, &restored, EPSILON));
    memset(&restored, 0, sizeof(telemetry_t));

    /*------------------TLV Framing----------------------*/
    printf("--- Part B: TLV Framing ---\n");
    count = serialize_telemetry_tlv(&original, buf);

    printf("TLV Serialized (%zu bytes): ", count);
    for(uint32_t i = 0U; i < count; i++) {
        printf("%02X ", buf[i]);
    }

    deserialize_telemetry_tlv(&buf[0], count, &restored);
    printf("\nTLV Deserialized: id=%u, temp=%.2f, hum=%.2f\n", restored.device_id,
                                                               restored.temperature,
                                                               restored.humidity
    );
    printf("Round-trip: %s\n", check_round_trip(&original, &restored, EPSILON));

    /*------------------TLV Forward Compatibility----------------------*/
    printf("\n--- TLV Forward Compatibility ---\n");
    memset(&restored, 0, sizeof(telemetry_t));

    uint8_t buf_2[] = {
    0x01, 0x04, 0x00, 0x00, 0x10, 0x00, /* Device ID */
    0x02, 0x04, 0x41, 0xC4, 0x00, 0x00, /* Temperature */
    0xFF, 0x04, 0x00, 0x11, 0x22, 0x33, /* Unknown type */
    0x03, 0x04, 0x42, 0x70, 0xCC, 0xCD  /* Humidity */
    };
    count = sizeof(buf_2) / sizeof(buf_2[0]);
    deserialize_telemetry_tlv(&buf_2[0], count, &restored);

    printf("Known fields decoded correctly: id=%u, temp=%.2f, hum=%.2f\n", restored.device_id,
                                                                           restored.temperature,
                                                                           restored.humidity
    );

    return APP_SUCCESS;
}