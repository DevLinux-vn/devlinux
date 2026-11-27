/**
 * @file logger.c
 * @brief Implementation of Industrial Logger Module.
 */

#include "logger.h"
#include <stdarg.h>
#include <stdio.h>

/** @brief Maximum formatted message buffer length in bytes. */
#define LOG_BUF_SIZE 256U

/**
 * @brief Human-readable labels for each log level.
 */
static const char * const s_level_str[LOG_LEVEL_COUNT] = {
    "ERROR",
    "WARN",
    "INFO",
    "DEBUG"
};

void log_write(log_level_t level, const char *p_file, uint32_t line,
               const char *p_func, const char *p_fmt, ...)
{
    if ((level >= LOG_LEVEL_COUNT) || (NULL == p_file) || (NULL == p_func) || (NULL == p_fmt)) {
        return;
    }

    if (level <= LOG_LEVEL_MAX) {
        char msg_buf[LOG_BUF_SIZE];
        va_list ap;

        va_start(ap, p_fmt);
        (void)vsnprintf(msg_buf, sizeof(msg_buf), p_fmt, ap);
        va_end(ap);

        printf("[%-5s] %s:%u (%s) | %s\n",
               s_level_str[level], p_file, line, p_func, msg_buf);
    }
}
