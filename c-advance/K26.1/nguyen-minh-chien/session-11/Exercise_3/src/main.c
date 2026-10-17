/**
 * @file    main.c
 * @brief   Demonstration program for the endian-safe telemetry codec.
 *
 * @details Exercises both serialization techniques implemented in
 *          telemetry_codec.c:
 *            - Part A: fixed-layout serialization (memcpy + htonl).
 *            - Part B: TLV framing, including graceful handling of an
 *              unknown TLV type for forward compatibility.
 */

#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include "telemetry_codec.h"

/**
 * @brief   Print a byte buffer as space-separated upper-case hex pairs.
 *
 * @param[in] p_buf  Pointer to the buffer to print. Must not be NULL.
 * @param[in] len    Number of bytes in p_buf to print.
 */
static void print_hex_dump(const uint8_t *p_buf, size_t len)
{
    size_t i;

    for (i = 0U; i < len; i++)
    {
        (void)printf("%02X", p_buf[i]);
        if ((i + 1U) < len)
        {
            (void)printf(" ");
        }
    }
    (void)printf("\n");
}

/**
 * @brief   Compare two telemetry_t values for equality.
 *
 * @details Integer fields are compared exactly; floating point fields are
 *          compared bit-for-bit via memcmp() on their serialized network
 *          representation is unnecessary here since a true round-trip must
 *          reproduce the exact original bit pattern, so a direct equality
 *          comparison is sufficient and intentional for this test.
 *
 * @param[in] p_a  First telemetry sample.
 * @param[in] p_b  Second telemetry sample.
 *
 * @return 1 if all fields match, 0 otherwise.
 */
static int32_t telemetry_equal(const telemetry_t *p_a,
                                const telemetry_t *p_b)
{
    int32_t is_equal = 1;

    if (p_a->device_id != p_b->device_id)
    {
        is_equal = 0;
    }
    if (p_a->temperature != p_b->temperature)
    {
        is_equal = 0;
    }
    if (p_a->humidity != p_b->humidity)
    {
        is_equal = 0;
    }

    return is_equal;
}

/**
 * @brief   Run the Part A (fixed-layout) round-trip demonstration.
 *
 * @return 0 on success, non-zero if the round-trip check fails.
 */
static int32_t run_part_a(void)
{
    telemetry_t original    = { 4096U, 24.5f, 60.2f };
    telemetry_t recovered   = { 0U, 0.0f, 0.0f };
    uint8_t     buffer[TELEMETRY_FIXED_SIZE_BYTES];
    size_t      bytes_written;
    int32_t     status = 0;

    (void)printf("\n--- Part A: Fixed-Layout (memcpy + htonl) ---\n");
    (void)printf("Original: id=%u, temp=%.2f, hum=%.2f\n",
                  (unsigned int)original.device_id,
                  (double)original.temperature,
                  (double)original.humidity);

    bytes_written = serialize_telemetry(&original, buffer);
    (void)printf("Serialized (%u bytes): ", (unsigned int)bytes_written);
    print_hex_dump(buffer, bytes_written);

    deserialize_telemetry(buffer, &recovered);
    (void)printf("Deserialized: id=%u, temp=%.2f, hum=%.2f\n",
                  (unsigned int)recovered.device_id,
                  (double)recovered.temperature,
                  (double)recovered.humidity);

    if ((bytes_written == (size_t)TELEMETRY_FIXED_SIZE_BYTES) &&
        (telemetry_equal(&original, &recovered) != 0))
    {
        (void)printf("Round-trip: PASS\n");
    }
    else
    {
        (void)printf("Round-trip: FAIL\n");
        status = 1;
    }

    return status;
}

/**
 * @brief   Run the Part B (TLV) round-trip demonstration.
 *
 * @return 0 on success, non-zero if the round-trip check fails.
 */
static int32_t run_part_b(void)
{
    telemetry_t original  = { 4096U, 24.5f, 60.2f };
    telemetry_t recovered = { 0U, 0.0f, 0.0f };
    uint8_t     buffer[TELEMETRY_TLV_SIZE_BYTES];
    size_t      bytes_written;
    int32_t     parse_status;
    int32_t     status = 0;

    (void)printf("\n--- Part B: TLV Framing ---\n");

    bytes_written = serialize_telemetry_tlv(&original, buffer);
    (void)printf("TLV Serialized (%u bytes): ", (unsigned int)bytes_written);
    print_hex_dump(buffer, bytes_written);

    parse_status = deserialize_telemetry_tlv(buffer, bytes_written,
                                              &recovered);
    (void)printf("TLV Deserialized: id=%u, temp=%.2f, hum=%.2f\n",
                  (unsigned int)recovered.device_id,
                  (double)recovered.temperature,
                  (double)recovered.humidity);

    if ((parse_status == TLV_DESERIALIZE_OK) &&
        (bytes_written == (size_t)TELEMETRY_TLV_SIZE_BYTES) &&
        (telemetry_equal(&original, &recovered) != 0))
    {
        (void)printf("TLV Round-trip: PASS\n");
    }
    else
    {
        (void)printf("TLV Round-trip: FAIL\n");
        status = 1;
    }

    return status;
}

/**
 * @brief   Demonstrate forward-compatible skipping of an unknown TLV type.
 *
 * @details Builds a buffer by hand containing: device_id, an unknown type
 *          0xFF entry, and temperature -- then verifies that the unknown
 *          entry is skipped and the known fields are still decoded
 *          correctly.
 *
 * @return 0 on success, non-zero if the demonstration fails.
 */
static int32_t run_forward_compat_demo(void)
{
    uint8_t     buffer[64];
    size_t      offset = 0U;
    uint32_t    net_device;
    uint32_t    net_temp;
    uint8_t     unknown_payload[3] = { 0xAAU, 0xBBU, 0xCCU };
    telemetry_t decoded = { 0U, 0.0f, 0.0f };
    int32_t     parse_status;
    int32_t     status = 0;
    telemetry_t source = { 4096U, 24.5f, 0.0f };

    (void)printf("\n--- TLV Forward Compatibility ---\n");

    /* Field 1: known TLV_TYPE_DEVICE_ID */
    net_device = htonl(source.device_id);
    buffer[offset] = (uint8_t)TLV_TYPE_DEVICE_ID;
    offset += TLV_TYPE_FIELD_SIZE_BYTES;
    buffer[offset] = (uint8_t)TLV_VALUE_SIZE_BYTES;
    offset += TLV_LENGTH_FIELD_SIZE_BYTES;
    (void)memcpy(&buffer[offset], &net_device, sizeof(net_device));
    offset += sizeof(net_device);

    /* Field 2: unknown type 0xFF, length 3, arbitrary payload */
    buffer[offset] = 0xFFU;
    offset += TLV_TYPE_FIELD_SIZE_BYTES;
    buffer[offset] = (uint8_t)sizeof(unknown_payload);
    offset += TLV_LENGTH_FIELD_SIZE_BYTES;
    (void)memcpy(&buffer[offset], unknown_payload, sizeof(unknown_payload));
    offset += sizeof(unknown_payload);

    /* Field 3: known TLV_TYPE_TEMPERATURE */
    {
        uint32_t host_bits = 0U;
        (void)memcpy(&host_bits, &source.temperature, sizeof(host_bits));
        net_temp = htonl(host_bits);
    }
    buffer[offset] = (uint8_t)TLV_TYPE_TEMPERATURE;
    offset += TLV_TYPE_FIELD_SIZE_BYTES;
    buffer[offset] = (uint8_t)TLV_VALUE_SIZE_BYTES;
    offset += TLV_LENGTH_FIELD_SIZE_BYTES;
    (void)memcpy(&buffer[offset], &net_temp, sizeof(net_temp));
    offset += sizeof(net_temp);

    (void)printf("Buffer with unknown type 0xFF: ");
    parse_status = deserialize_telemetry_tlv(buffer, offset, &decoded);

    if ((parse_status == TLV_DESERIALIZE_OK) &&
        (decoded.device_id == source.device_id) &&
        (decoded.temperature == source.temperature))
    {
        (void)printf("Skipped gracefully.\n");
        (void)printf("Known fields decoded correctly: id=%u, temp=%.2f\n",
                      (unsigned int)decoded.device_id,
                      (double)decoded.temperature);
    }
    else
    {
        (void)printf("FAILED to skip unknown type correctly.\n");
        status = 1;
    }

    return status;
}

/**
 * @brief   Program entry point.
 *
 * @return EXIT_SUCCESS (0) if all demonstrations pass, non-zero otherwise.
 */
int main(void)
{
    int32_t exit_code = 0;

    (void)printf("========== L11 Exercise 3: Telemetry Serializer "
                  "==========\n");

    if (run_part_a() != 0)
    {
        exit_code = 1;
    }
    if (run_part_b() != 0)
    {
        exit_code = 1;
    }
    if (run_forward_compat_demo() != 0)
    {
        exit_code = 1;
    }

    return exit_code;
}