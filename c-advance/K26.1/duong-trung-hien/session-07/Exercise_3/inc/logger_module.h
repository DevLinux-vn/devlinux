#ifndef LOGGER_MODULE_H
#define LOGGER_MODULE_H                                              

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#define SIZE_BUF (512)

/**
 * @brief Log message severity levels.
 *
 * Defines the supported log levels used by the logging module.
 * Lower numeric values indicate higher priority.
 */
typedef enum
{
    /** Critical error messages. */
    LOG_LEVEL_ERROR = 0,

    /** Warning messages indicating potential issues. */
    LOG_LEVEL_WARNING,

    /** Informational messages describing normal operation. */
    LOG_LEVEL_INFO,

    /** Debug messages for development and troubleshooting. */
    LOG_LEVEL_DEBUG,

    /** Total number of log levels. */
    LOG_LEVEL_COUNT
} log_level_t;

/**
 * @brief Write a formatted log message.
 *
 * Outputs a log message with additional debugging information,
 * including the log level, source file, line number, and function name.
 *
 * @param level Log severity level.
 * @param file Source file where the log is generated.
 * @param line Source line number.
 * @param func Function name where the log is generated.
 * @param fmt Printf-style format string.
 * @param ... Optional arguments corresponding to the format string.
 */
__attribute__((format(printf, 5, 6)))
void log_write(log_level_t level, const char *file, uint32_t line,
               const char *func, const char *fmt, ...);

/**
 * @brief Log an error message.
 *
 * Logs a message with the @ref LOG_LEVEL_ERROR severity if
 * `LOG_LEVEL_MAX` permits it.
 *
 * @param fmt Printf-style format string.
 * @param ... Optional arguments corresponding to the format string.
 */
#define LOG_ERROR(fmt, ...)                                                                 \
    do {                                                                                    \
        if (LOG_LEVEL_ERROR <= LOG_LEVEL_MAX)                                               \
        {                                                                                   \
            log_write(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__);   \
        }                                                                                   \
    } while (0)

/**
 * @brief Log a warning message.
 *
 * Logs a message with the @ref LOG_LEVEL_WARNING severity if
 * `LOG_LEVEL_MAX` permits it.
 *
 * @param fmt Printf-style format string.
 * @param ... Optional arguments corresponding to the format string.
 */
#define LOG_WARNING(fmt, ...)                                                               \
    do {                                                                                    \
        if (LOG_LEVEL_WARNING <= LOG_LEVEL_MAX)                                             \
        {                                                                                   \
            log_write(LOG_LEVEL_WARNING, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__); \
        }                                                                                   \
    } while (0)

/**
 * @brief Log an informational message.
 *
 * Logs a message with the @ref LOG_LEVEL_INFO severity if
 * `LOG_LEVEL_MAX` permits it.
 *
 * @param fmt Printf-style format string.
 * @param ... Optional arguments corresponding to the format string.
 */
#define LOG_INFO(fmt, ...)                                                                  \
    do {                                                                                    \
        if (LOG_LEVEL_INFO <= LOG_LEVEL_MAX)                                                \
        {                                                                                   \
            log_write(LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__);    \
        }                                                                                   \
    } while (0)

/**
 * @brief Log a debug message.
 *
 * Logs a message with the @ref LOG_LEVEL_DEBUG severity if
 * `LOG_LEVEL_MAX` permits it.
 *
 * @param fmt Printf-style format string.
 * @param ... Optional arguments corresponding to the format string.
 */
#define LOG_DEBUG(fmt, ...)                                                                 \
    do {                                                                                    \
        if (LOG_LEVEL_DEBUG <= LOG_LEVEL_MAX)                                               \
        {                                                                                   \
            log_write(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__);   \
        }                                                                                   \
    } while (0)

#endif /* LOGGER_MODULE_H */