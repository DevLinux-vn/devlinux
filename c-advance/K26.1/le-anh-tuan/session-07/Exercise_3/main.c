/**
 * @file main.c
 * @brief Exercise 3 - Industrial Logger Module.
 *
 * This program demonstrates a lightweight logging module for embedded
 * systems. Each log message contains the log level, source file name,
 * line number, function name, and formatted user message.
 *
 * The maximum enabled log level is controlled at compile time through
 * LOG_LEVEL_MAX.
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Available logger severity levels.
 *
 * Lower numerical values represent higher severity.
 */
typedef enum
{
    LOG_LEVEL_ERROR = 0,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG
} log_level_t;

/*
 * Default log level when LOG_LEVEL_MAX is not supplied by the compiler.
 */
#ifndef LOG_LEVEL_MAX
#define LOG_LEVEL_MAX LOG_LEVEL_INFO
#endif

/**
 * @brief Convert a log level to a printable text label.
 *
 * @param[in] level Log severity level.
 *
 * @return Pointer to a constant string representing the log level.
 */
static const char *log_level_to_string(log_level_t level)
{
    const char *level_string;

    switch (level)
    {
        case LOG_LEVEL_ERROR:
            level_string = "ERROR";
            break;

        case LOG_LEVEL_WARNING:
            level_string = "WARN ";
            break;

        case LOG_LEVEL_INFO:
            level_string = "INFO ";
            break;

        case LOG_LEVEL_DEBUG:
            level_string = "DEBUG";
            break;

        default:
            level_string = "UNKWN";
            break;
    }

    return level_string;
}

/**
 * @brief Write a formatted log message.
 *
 * The function prints the log level, source file, source line, calling
 * function, and the formatted user message. Variadic arguments are
 * forwarded to vprintf().
 *
 * The message is printed only when the supplied log level is enabled by
 * LOG_LEVEL_MAX.
 *
 * @param[in] level Log severity level.
 * @param[in] file Source file name supplied by __FILE__.
 * @param[in] line Source line number supplied by __LINE__.
 * @param[in] func Function name supplied by __func__.
 * @param[in] fmt printf-style format string.
 * @param[in] ... Optional arguments required by the format string.
 */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((format(printf, 5, 6)))
#endif
static void log_write(log_level_t level,
               const char *file,
               uint32_t line,
               const char *func,
               const char *fmt,
               ...)
{
    if (level <= LOG_LEVEL_MAX)
    {
        va_list args;

        (void)printf("[%s] %s:%lu (%s) | ",
                     log_level_to_string(level),
                     file,
                     (unsigned long)line,
                     func);

        va_start(args, fmt);
        (void)vprintf(fmt, args);
        va_end(args);

        (void)printf("\n");
    }
}

/**
 * @brief Log an ERROR-level message.
 *
 * @param fmt printf-style format string.
 * @param ... Optional formatting arguments.
 */
#define LOG_ERROR(fmt, ...)                                                 \
    do                                                                      \
    {                                                                       \
        if (LOG_LEVEL_ERROR <= LOG_LEVEL_MAX)                               \
        {                                                                   \
            log_write(LOG_LEVEL_ERROR,                                      \
                      __FILE__,                                             \
                      (uint32_t)__LINE__,                                   \
                      __func__,                                             \
                      (fmt),                                                \
                      ##__VA_ARGS__);                                       \
        }                                                                   \
    } while (0)

/**
 * @brief Log a WARNING-level message.
 *
 * @param fmt printf-style format string.
 * @param ... Optional formatting arguments.
 */
#define LOG_WARNING(fmt, ...)                                               \
    do                                                                      \
    {                                                                       \
        if (LOG_LEVEL_WARNING <= LOG_LEVEL_MAX)                             \
        {                                                                   \
            log_write(LOG_LEVEL_WARNING,                                    \
                      __FILE__,                                             \
                      (uint32_t)__LINE__,                                   \
                      __func__,                                             \
                      (fmt),                                                \
                      ##__VA_ARGS__);                                       \
        }                                                                   \
    } while (0)

/**
 * @brief Log an INFO-level message.
 *
 * @param fmt printf-style format string.
 * @param ... Optional formatting arguments.
 */
#define LOG_INFO(fmt, ...)                                                  \
    do                                                                      \
    {                                                                       \
        if (LOG_LEVEL_INFO <= LOG_LEVEL_MAX)                                \
        {                                                                   \
            log_write(LOG_LEVEL_INFO,                                       \
                      __FILE__,                                             \
                      (uint32_t)__LINE__,                                   \
                      __func__,                                             \
                      (fmt),                                                \
                      ##__VA_ARGS__);                                       \
        }                                                                   \
    } while (0)

/**
 * @brief Log a DEBUG-level message.
 *
 * @param fmt printf-style format string.
 * @param ... Optional formatting arguments.
 */
#define LOG_DEBUG(fmt, ...)                                                 \
    do                                                                      \
    {                                                                       \
        if (LOG_LEVEL_DEBUG <= LOG_LEVEL_MAX)                               \
        {                                                                   \
            log_write(LOG_LEVEL_DEBUG,                                      \
                      __FILE__,                                             \
                      (uint32_t)__LINE__,                                   \
                      __func__,                                             \
                      (fmt),                                                \
                      ##__VA_ARGS__);                                       \
        }                                                                   \
    } while (0)

/**
 * @brief Program entry point.
 *
 * Demonstrates all supported logger severity levels.
 *
 * @return Zero on successful program termination.
 */
int main(void)
{
    (void)printf("=== Exercise 3: Industrial Logger ===\n");
    (void)printf("Compiled with LOG_LEVEL_MAX = %d\n\n",
                 (int)LOG_LEVEL_MAX);

    LOG_INFO("System boot. Build time: %s", __TIME__);
    LOG_DEBUG("Discovered %d sensors on I2C bus.", 4);
    LOG_WARNING("Sensor %d reading is unstable.", 2);
    LOG_ERROR("Watchdog timeout! Rebooting in %d ms.", 500);
    LOG_INFO("Boot sequence complete.");

    return 0;
}