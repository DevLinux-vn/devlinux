#ifndef LOGGING_H
#define LOGGING_H

#include <stdint.h>
#include <stdarg.h>

typedef enum e_log_level
{
    LL_ERROR    = 0,
    LL_WARNING,
    LL_INFO,
    LL_DEBUG,
    LL_COUNT
} e_log_level_t;

#define LOG_WRITE(level, fmt, ...)                                                  \
    do {                                                                            \
        if ((level) <= LOG_LEVEL_MAX)                                               \
        {                                                                           \
            log_write((level), __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__);   \
        }                                                                           \
    } while (0)

#ifdef ENABLE_LOGGING
#define LOG_ERROR(fmt, ...)     LOG_WRITE(LL_ERROR, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...)   LOG_WRITE(LL_WARNING, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)      LOG_WRITE(LL_INFO, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)     LOG_WRITE(LL_DEBUG, fmt, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...)
#define LOG_WARNING(fmt, ...)
#define LOG_INFO(fmt, ...)
#define LOG_DEBUG(fmt, ...)
#endif

__attribute__((format(printf, 5, 6)))
void log_write(e_log_level_t level, const char *p_file, uint32_t line, const char *p_func, const char *p_fmt, ...);

#endif