#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocol_adapter.h"

static parse_result_t parse_tempe(const uint8_t *buf, size_t len, sensor_data_t *out, size_t *consumed){
    if (len < 18){
        *consumed = 0;
        return PARSE_NEED_MORE_DATA;
    }

    uint16_t id;
    memcpy(&id, buf, 2);
    double value;
    memcpy(&value, buf+2, 8);
    int64_t ts;
    memcpy(&ts, buf+10, 8);

    out->id = id;
    out->type = SENSOR_TEMPERATURE;
    out->value = value;
    out->ts = (time_t)ts;

    *consumed = 18;
    return PARSE_OK;
}
static size_t reply_tempe(parse_result_t result, uint8_t *rep, size_t cap){
    (void)result;
    (void)rep;
    (void)cap;
    return 0;
}

static parse_result_t parse_humi(const uint8_t *buf, size_t len, sensor_data_t *out, size_t *consumed){
    size_t i =0;
    while (i < len && buf[i] != '\n'){
        i++;
    }
    if (i >= len){
        if(len >= 64){
            *consumed = len;
            return PARSE_ERROR;
        }
        *consumed = 0;
        return PARSE_NEED_MORE_DATA;
    }

    char line[64];
   size_t line_len = i < sizeof(line) ? i : sizeof(line)-1;
   memcpy(line, buf, line_len);
   line[line_len] = '\0';

   unsigned int id;
   double val;
   long long ts;
   char prefix[8];

   if (sscanf(line, "%7s %u %lf %lld", prefix, &id, &val, &ts) == 4 && strcmp(prefix, "HUM") ==0 ){
       out->id = (sensor_id_t)id;
       out->type = SENSOR_HUMIDITY;
       out->value = val;
       out->ts = (time_t)ts;
       *consumed = i+1;
       return PARSE_OK;
   }

   *consumed = i+1;
   return PARSE_ERROR;
}
static size_t reply_humi(parse_result_t result, uint8_t *rep, size_t cap){
    (void)result;
    (void)rep;
    (void)cap;
    return 0;
}

static parse_result_t parse_door(const uint8_t *buf, size_t len, sensor_data_t *out, size_t *consumed){
    if(len < 2){
        *consumed = 0;
        return PARSE_NEED_MORE_DATA;
    }
    uint8_t type = buf[0];
    uint8_t length = buf[1];
    size_t total_frame_len = 2 + length;
    if(len < total_frame_len){
        *consumed = 0;
        return PARSE_NEED_MORE_DATA;
    }

    if(type == 0x01 && length == 6){
        uint16_t id = ((uint16_t)buf[2] << 8) | buf[3];
        uint32_t ts_net = ((uint32_t) buf[4] << 24) | ((uint32_t)buf[5] << 16) | ((uint32_t)buf[6] << 8) | buf[7];
        
        out->id = id;
        out->type = SENSOR_DOOR;
        out->value = 1.0;
        out->ts = (time_t)ts_net;
        *consumed = total_frame_len;
        return PARSE_OK;
    }else if(type == 0x02 && length == 7){
        uint16_t id = ((uint16_t)buf[2] << 8) | buf[3];
        uint8_t status = buf[4];
        uint32_t ts_net = ((uint32_t) buf[5] << 24) | ((uint32_t)buf[6] << 16) | ((uint32_t)buf[7] << 8) | buf[8];
        
        out->id = id;
        out->type = SENSOR_DOOR;
        out->value = (double)status;
        out->ts = (time_t)ts_net;
        *consumed = total_frame_len;
        return PARSE_OK;        
    }
    *consumed = total_frame_len;
    return PARSE_ERROR;
}
static size_t reply_door(parse_result_t result, uint8_t *rep, size_t cap){
    (void)result;
    (void)rep;
    (void)cap;
    return 0;
}

static uint16_t calculate_gas(const uint8_t *data, size_t len){
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++){
        crc ^= ((uint16_t)data[i] << 8);
        for (uint8_t j = 0; j < 8; j++){
            if (crc & 0x8000){
                crc = (crc << 1) ^ 0x1021;
            }else{
                crc <<= 1;
            }
        }
    }
    return crc;
}
static parse_result_t parse_gas(const uint8_t *data, size_t len, sensor_data_t *out, size_t *consumed){
    if (len < 11){
        *consumed = 0;
        return PARSE_NEED_MORE_DATA;
    }
    if (data[0] != 0x7E){
        *consumed = 1;
        return PARSE_ERROR;
    }
    uint16_t id = ((uint16_t)data[1] << 8) | data[2];
    uint16_t ppm = ((uint16_t)data[3] << 8) | data[4];
    uint32_t ts = ((uint32_t)data[5] << 24) | ((uint32_t)data[6] << 16) | ((uint32_t)data[7] << 8) | data[8];
    uint16_t recv_crc = ((uint16_t)data[9] << 8) | data[10];

    uint16_t calc_crc = calculate_gas(data+1, 8);
    if (calc_crc != recv_crc){
        *consumed = 11;
        return PARSE_ERROR;
    }

    out->id = id;
    out->type = SENSOR_SMOKE_GAS;
    out->value = (double)ppm;
    out->ts = (time_t)ts;
    *consumed = 11;
    return PARSE_OK;
}
static size_t reply_gas(parse_result_t result, uint8_t *rep, size_t cap){
    if(cap < 1){
        return 0;
    }
    if(result == PARSE_OK){
        rep[0] = 0x06;
    }else{
        rep[0] = 0x15;
    }
    return 1;
}
const protocol_adapter_t adapters[4] = {
    { "TBP", 0, parse_tempe, reply_tempe },
    { "THP", 1, parse_humi, reply_humi },
    { "TDP", 2, parse_door, reply_door },
    { "TGP", 3, parse_gas, reply_gas }
};

