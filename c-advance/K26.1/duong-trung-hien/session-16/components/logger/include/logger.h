#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>

/**
 * @brief Log severity levels.
 */
typedef enum {
  LOG_LEVEL_DEBUG, /**< Debug-level message. */
  LOG_LEVEL_INFO,  /**< Informational message. */
  LOG_LEVEL_WARN,  /**< Warning message. */
  LOG_LEVEL_ERROR  /**< Error message. */
} log_level_t;

/**
 * @brief Log a debug-level message.
 *
 * Automatically includes the source file, line number, and function name.
 *
 * @param[in] fmt  printf-style format string.
 * @param[in] ...  Optional arguments for the format string.
 */
#define LOG_DEBUG(fmt, ...)                                                    \
  do {                                                                         \
    logger_log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, (fmt),           \
               ##__VA_ARGS__);                                                 \
  } while (0)

/**
 * @brief Log an informational message.
 *
 * Automatically includes the source file, line number, and function name.
 *
 * @param[in] fmt  printf-style format string.
 * @param[in] ...  Optional arguments for the format string.
 */
#define LOG_INFO(fmt, ...)                                                     \
  do {                                                                         \
    logger_log(LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, (fmt),            \
               ##__VA_ARGS__);                                                 \
  } while (0)

/**
 * @brief Log a warning message.
 *
 * Automatically includes the source file, line number, and function name.
 *
 * @param[in] fmt  printf-style format string.
 * @param[in] ...  Optional arguments for the format string.
 */
#define LOG_WARN(fmt, ...)                                                     \
  do {                                                                         \
    logger_log(LOG_LEVEL_WARN, __FILE__, __LINE__, __func__, (fmt),            \
               ##__VA_ARGS__);                                                 \
  } while (0)

/**
 * @brief Log an error message.
 *
 * Automatically includes the source file, line number, and function name.
 *
 * @param[in] fmt  printf-style format string.
 * @param[in] ...  Optional arguments for the format string.
 */
#define LOG_ERROR(fmt, ...)                                                    \
  do {                                                                         \
    logger_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, (fmt),           \
               ##__VA_ARGS__);                                                 \
  } while (0)

/**
 * @brief Initialize the logger.
 *
 * Opens the specified log file and sets the minimum severity level
 * that will be recorded.
 *
 * @param[in] min_level    Minimum log severity level to record.
 * @param[in] p_file_path  Path to the log file.
 */
void logger_init(log_level_t min_level, const char *p_file_path);

/**
 * @brief Write a log message.
 *
 * This function is the internal logging function used by the LOG_*
 * macros. It records the log severity, source location, function name,
 * and formatted message.
 *
 * @param[in] level    Log severity level.
 * @param[in] p_file   Source file name (__FILE__).
 * @param[in] line     Source line number (__LINE__).
 * @param[in] p_func   Function name (__func__).
 * @param[in] p_fmt    printf-style format string.
 * @param[in] ...      Variadic arguments corresponding to @p p_fmt.
 */
void logger_log(log_level_t level, const char *p_file, int line,
                const char *p_func, const char *p_fmt, ...);

/**
 * @brief Close the logger.
 *
 * Closes the log file and releases resources used by the logger.
 */
void logger_close(void);

#endif /* LOGGER_H */