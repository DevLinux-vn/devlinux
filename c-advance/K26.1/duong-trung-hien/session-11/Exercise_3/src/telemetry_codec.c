#include "telemetry_codec.h"
#include <string.h>
#include <math.h>
#include <arpa/inet.h>

size_t serialize_telemetry(const telemetry_t *p_src, uint8_t *p_buf) {
    if (NULL == p_src || NULL == p_buf) {
        return ERROR;
    }

    uint32_t value = 0U;
    size_t offset = 0;

    /* Serialize device ID. */
    value = htonl(p_src->device_id);
    memcpy(&p_buf[offset], &value, sizeof(value));
    offset += sizeof(value);

    /* Serialize temperature. */
    memcpy(&value, &p_src->temperature, sizeof(value));
    value = htonl(value);
    memcpy(&p_buf[offset], &value, sizeof(value));
    offset += sizeof(value);

    /* Serialize humidity. */
    memcpy(&value, &p_src->humidity, sizeof(value));
    value = htonl(value);
    memcpy(&p_buf[offset], &value, sizeof(value));
    offset += sizeof(value);

    return offset;
}   

void deserialize_telemetry(const uint8_t *p_buf, telemetry_t *p_dst) {
    if (NULL == p_buf || NULL == p_dst) {
        return;
    }

    uint32_t value = 0U;
    size_t  offset = 0;

    /* Deserialize device ID. */
    memcpy(&value, &p_buf[offset], sizeof(value));
    value = ntohl(value);
    p_dst->device_id = value;
    offset += sizeof(value);

    /* Deserialize temperature. */
    memcpy(&value, &p_buf[offset], sizeof(value));
    value = ntohl(value);
    memcpy(&p_dst->temperature, &value, sizeof(value));
    offset += sizeof(value);

    /* Deserialize humidity. */
    memcpy(&value, &p_buf[offset], sizeof(value));
    value = ntohl(value);
    memcpy(&p_dst->humidity, &value, sizeof(value));
    offset += sizeof(value);

    (void)offset;
}

size_t serialize_telemetry_tlv(const telemetry_t *p_src, uint8_t *p_buf) {
    if (NULL == p_src || NULL == p_buf) {
        return ERROR;
    }

    uint32_t value = 0U;
    size_t offset = 0U;

    /* TLV Serialize device ID. */
    p_buf[offset] = TLV_TYPE_DEVICE_ID;
    offset += TYPE_SIZE;

    p_buf[offset] = sizeof(uint32_t);
    offset += LENGTH_SIZE;

    value = htonl(p_src->device_id);
    memcpy(&p_buf[offset], &value, sizeof(value));
    offset += sizeof(value);

    /* TLV Serialize temperature. */
    p_buf[offset] = TLV_TYPE_TEMPERATURE;
    offset += TYPE_SIZE;

    p_buf[offset] = sizeof(float);
    offset += LENGTH_SIZE;

    memcpy(&value, &p_src->temperature, sizeof(float));
    value = htonl(value);
    memcpy(&p_buf[offset], &value, sizeof(value));
    offset += sizeof(value);

    /* TLV Serialize humidity. */
    p_buf[offset] = TLV_TYPE_HUMIDITY;
    offset += TYPE_SIZE;

    p_buf[offset] = sizeof(float);
    offset += LENGTH_SIZE;

    memcpy(&value, &p_src->humidity, sizeof(float));
    value = htonl(value);
    memcpy(&p_buf[offset], &value, sizeof(value));
    offset += sizeof(value);

    return offset;
}

int32_t deserialize_telemetry_tlv(const uint8_t *p_buf, size_t buf_len, telemetry_t *p_dst) {
    if (NULL == p_buf || NULL == p_dst || buf_len == 0U) {
        return FAILED;
    }
    uint32_t value = 0U;
    size_t offset = 0U;

    while (offset < buf_len) {
        uint8_t type = p_buf[offset];
        offset += TYPE_SIZE;

        uint8_t length = p_buf[offset];
        offset += LENGTH_SIZE;

        if ((offset + length) > buf_len) {
            return FAILED;
        }

        if (type == TLV_TYPE_DEVICE_ID) {
            /* TLV Deserialize device ID. */
            memcpy(&value, &p_buf[offset], sizeof(value));
            value = ntohl(value);
            memcpy(&p_dst->device_id, &value, sizeof(value));
            offset += sizeof(value);
        }
        else if (type == TLV_TYPE_TEMPERATURE) {
            /* TLV Deserialize temperature. */
            memcpy(&value, &p_buf[offset], sizeof(value));
            value = ntohl(value);
            memcpy(&p_dst->temperature, &value, sizeof(value));
            offset += sizeof(value);
        }
        else if (type == TLV_TYPE_HUMIDITY) {
             /* TLV Deserialize temperature. */
            memcpy(&value, &p_buf[offset], sizeof(value));
            value = ntohl(value);
            memcpy(&p_dst->humidity, &value, sizeof(value));
            offset += sizeof(value);
        }
        else
        {
            #ifdef DEBUG
            printf("Buffer with unknown type 0x%02X: Skipped gracefully.\n", type);
            #endif

            offset += length;
        }
    }

    return SUCCESS;
}

const char *check_round_trip(const telemetry_t *p_data_1, const telemetry_t *p_data_2, float epsilon) {
    if (p_data_1->device_id != p_data_2->device_id) {
        return "FALSE";
    }

    if (fabsf(p_data_1->temperature - p_data_2->temperature) > epsilon) {
        return "FALSE";
    }

    if (fabsf(p_data_1->humidity - p_data_2->humidity) > epsilon) {
        return "FALSE";
    }

    return "PASS";
}