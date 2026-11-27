/**
 * @file logger.h
 * @brief Industrial Logger Module Interface.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>

/**
 * @brief Supported compile-time and runtime log severity levels.
 */
typedef enum {
    LOG_LEVEL_ERROR   = 0, /**< Critical failure events.      */
    LOG_LEVEL_WARNING = 1, /**< Non-fatal warning conditions. */
    LOG_LEVEL_INFO    = 2, /**< General operational status.   */
    LOG_LEVEL_DEBUG   = 3, /**< Detailed debug diagnostics.   */
    LOG_LEVEL_COUNT        /**< Total number of log levels.   */
} log_level_t;

#ifndef LOG_LEVEL_MAX
/**
 * @brief Default maximum enabled log level if not overridden via -DLOG_LEVEL_MAX.
 */
#define LOG_LEVEL_MAX LOG_LEVEL_DEBUG
#endif

/**
 * @brief Format and output a log message with call-site metadata.
 * @param[in] level  Severity level of the log entry.
 * @param[in] p_file Source filename from __FILE__.
 * @param[in] line   Source line number from __LINE__.
 * @param[in] p_func Calling function name from __func__.
 * @param[in] p_fmt  Printf-style format string.
 * @param[in] ...    Optional variadic arguments matching p_fmt.
 */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((format(printf, 5, 6)))
#endif
void log_write(log_level_t level, const char *p_file, uint32_t line,
               const char *p_func, const char *p_fmt, ...);

/**
 * @brief Internal helper macro to emit a log entry if enabled at compile time.
 */
#define LOG_EMIT(lvl, ...) \
    do { \
        if ((lvl) <= LOG_LEVEL_MAX) { \
            log_write((lvl), __FILE__, (uint32_t)__LINE__, __func__, __VA_ARGS__); \
        } \
    } while (0)

/** @brief Log an ERROR level message. */
#define LOG_ERROR(...)   LOG_EMIT(LOG_LEVEL_ERROR, __VA_ARGS__)

/** @brief Log a WARNING level message. */
#define LOG_WARNING(...) LOG_EMIT(LOG_LEVEL_WARNING, __VA_ARGS__)

/** @brief Log an INFO level message. */
#define LOG_INFO(...)    LOG_EMIT(LOG_LEVEL_INFO, __VA_ARGS__)

/** @brief Log a DEBUG level message. */
#define LOG_DEBUG(...)   LOG_EMIT(LOG_LEVEL_DEBUG, __VA_ARGS__)

#endif /* LOGGER_H */
