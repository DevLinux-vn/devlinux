#ifndef IOT_SERVER_H
#define IOT_SERVER_H

#include <time.h>

#define SERVER_HOST         "127.0.0.1"
#define SERVER_PORT         9999
#define MAX_CLIENTS         10
#define BROADCAST_INTERVAL  5     
#define BUF_SIZE            512

typedef struct {
    int    fd;              
    int    mode;            
    time_t last_activity;
    char   inbuf[BUF_SIZE]; 
    size_t inlen;
} client_t;

#endif 