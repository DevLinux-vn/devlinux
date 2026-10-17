/**
 * @file    telemetry_codec.c
 * @brief   Implementation of endian-safe telemetry serialization
 *          (fixed-layout and TLV framing).
 */

#include "telemetry_codec.h"

#include <string.h>

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

/**
 * @brief   Convert a float to its network-byte-order uint32_t representation.
 *
 * @details Uses memcpy() to reinterpret the float's bit pattern as a
 *          uint32_t (MISRA-C Rule 11.3 / CERT-C EXP39-C compliant -- never
 *          a pointer cast), then applies htonl().
 *
 * @param[in] value  The float value to convert.
 *
 * @return The value's bit pattern, in network byte order.
 */
static uint32_t float_to_net_u32(float value)
{
    uint32_t host_bits = 0U;

    (void)memcpy(&host_bits, &value, sizeof(host_bits));

    return htonl(host_bits);
}

/**
 * @brief   Convert a network-byte-order uint32_t back into a float.
 *
 * @details Applies ntohl() to obtain the host-order bit pattern, then
 *          uses memcpy() to reinterpret those bits as a float (never a
 *          pointer cast), per MISRA-C Rule 11.3 / CERT-C EXP39-C.
 *
 * @param[in] net_bits  The network-order bit pattern of a float.
 *
 * @return The reconstructed float value.
 */
static float net_u32_to_float(uint32_t net_bits)
{
    float    value     = 0.0f;
    uint32_t host_bits = ntohl(net_bits);

    (void)memcpy(&value, &host_bits, sizeof(value));

    return value;
}

size_t serialize_telemetry(const telemetry_t *p_src, uint8_t *p_buf)
{
    size_t   offset      = 0U;
    uint32_t net_device  = 0U;
    uint32_t net_temp    = 0U;
    uint32_t net_hum     = 0U;

    if ((p_src == NULL) || (p_buf == NULL))
    {
        return 0U;
    }

    net_device = htonl(p_src->device_id);
    (void)memcpy(&p_buf[offset], &net_device, sizeof(net_device));
    offset += sizeof(net_device);

    net_temp = float_to_net_u32(p_src->temperature);
    (void)memcpy(&p_buf[offset], &net_temp, sizeof(net_temp));
    offset += sizeof(net_temp);

    net_hum = float_to_net_u32(p_src->humidity);
    (void)memcpy(&p_buf[offset], &net_hum, sizeof(net_hum));
    offset += sizeof(net_hum);

    return offset;
}

void deserialize_telemetry(const uint8_t *p_buf, telemetry_t *p_dst)
{
    size_t   offset     = 0U;
    uint32_t net_device = 0U;
    uint32_t net_temp   = 0U;
    uint32_t net_hum    = 0U;

    if ((p_buf == NULL) || (p_dst == NULL))
    {
        return;
    }

    (void)memcpy(&net_device, &p_buf[offset], sizeof(net_device));
    p_dst->device_id = ntohl(net_device);
    offset += sizeof(net_device);

    (void)memcpy(&net_temp, &p_buf[offset], sizeof(net_temp));
    p_dst->temperature = net_u32_to_float(net_temp);
    offset += sizeof(net_temp);

    (void)memcpy(&net_hum, &p_buf[offset], sizeof(net_hum));
    p_dst->humidity = net_u32_to_float(net_hum);
}

/**
 * @brief   Append one TLV entry (Type + Length + 4-byte network-order Value)
 *          to a buffer.
 *
 * @param[out] p_buf     Destination buffer.
 * @param[in]  offset    Byte offset within p_buf at which to write.
 * @param[in]  type      TLV type identifier (fits in a single byte).
 * @param[in]  net_value 4-byte value, already in network byte order.
 *
 * @return The offset immediately following the newly written entry.
 */
static size_t append_tlv_entry(uint8_t *p_buf, size_t offset, uint8_t type,
                                uint32_t net_value)
{
    p_buf[offset] = type;
    offset += TLV_TYPE_FIELD_SIZE_BYTES;

    p_buf[offset] = (uint8_t)TLV_VALUE_SIZE_BYTES;
    offset += TLV_LENGTH_FIELD_SIZE_BYTES;

    (void)memcpy(&p_buf[offset], &net_value, sizeof(net_value));
    offset += sizeof(net_value);

    return offset;
}

size_t serialize_telemetry_tlv(const telemetry_t *p_src, uint8_t *p_buf)
{
    size_t offset = 0U;

    if ((p_src == NULL) || (p_buf == NULL))
    {
        return 0U;
    }

    offset = append_tlv_entry(p_buf, offset, (uint8_t)TLV_TYPE_DEVICE_ID,
                               htonl(p_src->device_id));

    offset = append_tlv_entry(p_buf, offset, (uint8_t)TLV_TYPE_TEMPERATURE,
                               float_to_net_u32(p_src->temperature));

    offset = append_tlv_entry(p_buf, offset, (uint8_t)TLV_TYPE_HUMIDITY,
                               float_to_net_u32(p_src->humidity));

    return offset;
}

int32_t deserialize_telemetry_tlv(const uint8_t *p_buf, size_t buf_len,
                                   telemetry_t *p_dst)
{
    size_t  offset = 0U;
    int32_t result = TLV_DESERIALIZE_OK;

    if ((p_buf == NULL) || (p_dst == NULL))
    {
        return TLV_DESERIALIZE_ERROR;
    }

    (void)memset(p_dst, 0, sizeof(*p_dst));

    while (offset < buf_len)
    {
        uint8_t  type;
        uint8_t  length;
        size_t   header_end;
        size_t   value_end;
        uint32_t net_value;

        /* MISRA-C Directive 4.14 / CERT-C ARR30-C: validate before every
         * read. First confirm the Type+Length header itself is present.
         */
        header_end = offset + TLV_TYPE_FIELD_SIZE_BYTES +
                     TLV_LENGTH_FIELD_SIZE_BYTES;
        if (header_end > buf_len)
        {
            result = TLV_DESERIALIZE_ERROR;
            break;
        }

        type   = p_buf[offset];
        length = p_buf[offset + TLV_TYPE_FIELD_SIZE_BYTES];

        /* Now confirm the declared Value length fits within the buffer
         * (Heartbleed-style bounds check) before reading any Value bytes.
         */
        value_end = header_end + (size_t)length;
        if (value_end > buf_len)
        {
            result = TLV_DESERIALIZE_ERROR;
            break;
        }

        if ((type == (uint8_t)TLV_TYPE_DEVICE_ID) ||
            (type == (uint8_t)TLV_TYPE_TEMPERATURE) ||
            (type == (uint8_t)TLV_TYPE_HUMIDITY))
        {
            /* All recognised fields in this protocol are exactly 4 bytes.
             * A mismatched length on a known type is treated as invalid
             * input rather than silently misinterpreted.
             */
            if (length != (uint8_t)TLV_VALUE_SIZE_BYTES)
            {
                result = TLV_DESERIALIZE_ERROR;
                break;
            }

            (void)memcpy(&net_value, &p_buf[header_end], sizeof(net_value));

            if (type == (uint8_t)TLV_TYPE_DEVICE_ID)
            {
                p_dst->device_id = ntohl(net_value);
            }
            else if (type == (uint8_t)TLV_TYPE_TEMPERATURE)
            {
                p_dst->temperature = net_u32_to_float(net_value);
            }
            else
            {
                p_dst->humidity = net_u32_to_float(net_value);
            }
        }
        else
        {
            /* Unknown TLV type: skip gracefully for forward compatibility.
             * No Value bytes are read since they are not understood.
             */
        }

        offset = value_end;
    }

    return result;
}