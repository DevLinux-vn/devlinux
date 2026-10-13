#ifndef PROTOCOL_ADAPTER_H
#define PROTOCOL_ADAPTER_H

#include <stdint.h>
#include <stddef.h>
#include "sensor_types.h"

typedef enum {
    PARSE_OK = 1,
    PARSE_ERROR = -1,
    PARSE_NEED_MORE_DATA = 0
}parse_result_t;

typedef struct {
    const char *name;
    uint16_t port;

    parse_result_t (*parse)(const uint8_t *buf, size_t len, sensor_data_t *out, size_t *consumed);
    size_t (*build_reply)(parse_result_t result, uint8_t *reply, size_t cap);
} protocol_adapter_t;

#endif /* PROTOCOL_ADAPTER_H */