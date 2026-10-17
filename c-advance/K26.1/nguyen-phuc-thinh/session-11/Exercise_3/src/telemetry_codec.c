/**
 * @file telemetry_codec.c
 * @brief Implementation of Endian-Safe Serialization and TLV logic.
 */
#include "telemetry_codec.h"
#include <string.h>

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

/* --- Helper functions for safe byte-swapping (Type-punning via memcpy) --- */
static void serialize_uint32(uint32_t val, uint8_t *p_buf) 
{
    uint32_t net_val = htonl(val);
    memcpy(p_buf, &net_val, sizeof(uint32_t));
}

static void serialize_float(float val, uint8_t *p_buf) 
{
    uint32_t raw_val;
    memcpy(&raw_val, &val, sizeof(float)); /* Safe float -> uint32_t cast */
    uint32_t net_val = htonl(raw_val);
    memcpy(p_buf, &net_val, sizeof(uint32_t));
}

static uint32_t deserialize_uint32(const uint8_t *p_buf) 
{
    uint32_t net_val;
    memcpy(&net_val, p_buf, sizeof(uint32_t));
    return ntohl(net_val);
}

static float deserialize_float(const uint8_t *p_buf) 
{
    uint32_t net_val;
    memcpy(&net_val, p_buf, sizeof(uint32_t));
    uint32_t raw_val = ntohl(net_val);
    float val;
    memcpy(&val, &raw_val, sizeof(float)); /* Safe uint32_t -> float cast */
    return val;
}

/* --- Part A: Fixed-Layout Serialization --- */
size_t serialize_telemetry(const telemetry_t *p_src, uint8_t *p_buf) 
{
    if ((p_src == NULL) || (p_buf == NULL)) 
    {
        return 0U;
    }
    
    serialize_uint32(p_src->device_id, p_buf);
    serialize_float(p_src->temperature, p_buf + 4);
    serialize_float(p_src->humidity, p_buf + 8);
    
    return 12U;
}

void deserialize_telemetry(const uint8_t *p_buf, telemetry_t *p_dst) 
{
    if ((p_buf == NULL) || (p_dst == NULL)) 
    {
        return;
    }
    
    p_dst->device_id   = deserialize_uint32(p_buf);
    p_dst->temperature = deserialize_float(p_buf + 4);
    p_dst->humidity    = deserialize_float(p_buf + 8);
}

/* --- Part B: TLV Framing --- */
size_t serialize_telemetry_tlv(const telemetry_t *p_src, uint8_t *p_buf) 
{
    if ((p_src == NULL) || (p_buf == NULL)) 
    {
        return 0U;
    }
    
    size_t offset = 0U;
    
    /* Device ID TLV */
    p_buf[offset++] = TLV_TYPE_DEVICE_ID;
    p_buf[offset++] = 4U; /* Length */
    serialize_uint32(p_src->device_id, p_buf + offset);
    offset += 4U;

    /* Temperature TLV */
    p_buf[offset++] = TLV_TYPE_TEMPERATURE;
    p_buf[offset++] = 4U; /* Length */
    serialize_float(p_src->temperature, p_buf + offset);
    offset += 4U;

    /* Humidity TLV */
    p_buf[offset++] = TLV_TYPE_HUMIDITY;
    p_buf[offset++] = 4U; /* Length */
    serialize_float(p_src->humidity, p_buf + offset);
    offset += 4U;

    return offset;
}

int32_t deserialize_telemetry_tlv(const uint8_t *p_buf, size_t buf_len, telemetry_t *p_dst) 
{
    if ((p_buf == NULL) || (p_dst == NULL)) 
    {
        return -1;
    }
    
    size_t offset = 0U;
    
    while ((offset + 2U) <= buf_len) 
    {
        uint8_t type = p_buf[offset];
        uint8_t len  = p_buf[offset + 1U];
        
        /* Prevent zero-length loops (MEM04-C) and OOB reads (Heartbleed defense) */
        if ((len == 0U) || ((offset + 2U + len) > buf_len)) 
        {
            return -1; 
        }
        
        if ((type == TLV_TYPE_DEVICE_ID) && (len == 4U)) 
        {
            p_dst->device_id = deserialize_uint32(p_buf + offset + 2U);
        } 
        else if ((type == TLV_TYPE_TEMPERATURE) && (len == 4U)) 
        {
            p_dst->temperature = deserialize_float(p_buf + offset + 2U);
        } 
        else if ((type == TLV_TYPE_HUMIDITY) && (len == 4U)) 
        {
            p_dst->humidity = deserialize_float(p_buf + offset + 2U);
        } 
        else 
        {
            /* Forward Compatibility: Unknown type or mismatched length. Skip gracefully. */
        }
        
        offset += (2U + len);
    }
    
    return 0;
}