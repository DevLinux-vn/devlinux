#ifndef EVENT_MONITOR_H
#define EVENT_MONITOR_H

#define FIFO_PATH       "/tmp/event_log"
#define SOCKET_PATH     "/tmp/event_control.sock"
#define FILE_PATH       "/tmp/system_status"
#define POLL_TIMEOUT_MS 2000  
#define BUF_SIZE        512

typedef enum {
    FD_FIFO = 0,
    FD_SOCKET_LISTENER,
    FD_FILE,
    NUM_POLL_FDS
} pollfd_index_t;

#endif 