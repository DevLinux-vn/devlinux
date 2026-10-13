#ifndef GATEWAY_H
#define GATEWAY_H

#include <pthread.h>
#include <stdbool.h>
#include <signal.h>
#include <stdint.h>
#include <sys/types.h>
#include "sbuffer.h"

#define FIFO_NAME "logFifo"
#define LOG_FILE "gateway.log"

typedef struct {
    uint16_t base_port;
    uint16_t http_port;
    volatile sig_atomic_t stop;
    pid_t log_pid;
    sbuffer_t *sbuffer;
} gateway_t;

void logger_send(const char *source, const char *message);

#endif /* GATEWAY_H */
