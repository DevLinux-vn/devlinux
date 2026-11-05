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

/* MISRA-C Rule 20.10 violation: __VA_ARGS__ is standard practice in industrial logging. */
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

/**
 * @brief Write log message with level, file, line, and function context.
 * @param level Log level (LL_ERROR, LL_WARNING, LL_INFO, LL_DEBUG)
 * @param p_file Source file name (__FILE__)
 * @param line Source line number (__LINE__)
 * @param p_func Source function name (__func__)
 * @param p_fmt Printf-style format string
 * @param ... Variadic arguments for format string
 *
 * Only prints if LOG_LEVEL_MAX is defined and level <= LOG_LEVEL_MAX.
 */
__attribute__((format(printf, 5, 6)))
void log_write(e_log_level_t level, const char *p_file, uint32_t line, const char *p_func, const char *p_fmt, ...);

#endif