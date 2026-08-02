#include <logger_module.h>

/**
 * @brief String representations of log levels.
 *
 * Maps each @ref log_level_t enumeration value to its corresponding
 * human-readable string used when printing log messages.
 *
 * @note The array index corresponds directly to the values of
 *       @ref log_level_t.
 */
static const char *LOG_LEVEL[LOG_LEVEL_COUNT] = {
    [LOG_LEVEL_ERROR]   = "ERROR",
    [LOG_LEVEL_WARNING] = "WARN",
    [LOG_LEVEL_INFO]    = "INFO",
    [LOG_LEVEL_DEBUG]   = "DEBUG"
};

__attribute__((format(printf, 5, 6)))
void log_write(log_level_t level, const char *file, uint32_t line,
               const char *func, const char *fmt, ...)
{
    if (level >= LOG_LEVEL_COUNT)
    {
        return;
    }

    if (level <= LOG_LEVEL_MAX)
    {
        char buf[SIZE_BUF];
        uint32_t offset = snprintf(buf, sizeof(buf), "[%-5s] %s:%d (%s) | ", LOG_LEVEL[level], file, line, func);
        printf("%s", buf);

        va_list ap;
        va_start(ap, fmt);

        vprintf(fmt, ap);   

        va_end(ap);

        (void)offset;
        printf("\n");
    }
}