#ifndef IOT_SERVER_H
#define IOT_SERVER_H

#include <time.h>

#define SERVER_PORT         9999
#define SERVER_IP_ADDR      "127.0.0.1"
#define MAX_CLIENTS         10
#define BROADCAST_INTERVAL  5  /* seconds */
#define BUFFER_SIZE         256
#define SELECT_TIMEOUT_SEC  1  /* 1 second timeout for select to check broadcast timer */

/* Device operation modes */
typedef enum {
    MODE_IDLE   = 0,
    MODE_ACTIVE = 1,
    MODE_ALERT  = 2
} device_mode_t;

/* Client connection representation */
typedef struct {
    int     fd;            /* Socket file descriptor (-1 when slot is unused) */
    int     mode;          /* 0=idle, 1=active, 2=alert */
    time_t  last_activity; /* Timestamp of the last received command */
} client_t;

/* Status codes for functions (rule: no void return) */
typedef enum {
    STATUS_SUCCESS = 0,
    STATUS_ERROR   = -1
} status_code_t;

#endif /* IOT_SERVER_H */
