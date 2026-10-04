#ifndef IOT_SERVER_H
#define IOT_SERVER_H

#include <time.h>

#define SERVER_PORT        9999
#define MAX_CLIENTS        10
#define BROADCAST_INTERVAL 5      /* seconds */
#define CLIENT_BUF_SIZE    256    

typedef struct {
    int     fd;                  
    int     id;                  
    int     mode;                 
    time_t  last_activity;
    char    inbuf[CLIENT_BUF_SIZE]; 
    size_t  inlen;
} client_t;

#endif 