#ifndef EVENT_MONITOR_H
#define EVENT_MONITOR_H

#define FIFO_PATH           "/tmp/event_log"
#define SOCKET_PATH         "/tmp/event_control.sock"
#define FILE_PATH           "/tmp/system_status"
#define POLL_TIMEOUT_MS     2000  /* 2 seconds */
#define BUFFER_SIZE         256

/* Indices for the pollfd array */
typedef enum {
    FD_FIFO = 0,
    FD_SOCKET_LISTENER = 1,
    FD_FILE = 2,
    NUM_POLL_FDS = 3
} pollfd_index_t;

/* Status codes for functions (rule: no void return) */
typedef enum {
    STATUS_SUCCESS = 0,
    STATUS_ERROR   = -1
} status_code_t;

#endif /* EVENT_MONITOR_H */
