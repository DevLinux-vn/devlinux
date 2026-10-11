#include <stdio.h>
#include "logging.h"

static const char *sg_level_str[LL_COUNT] =
{
    [LL_ERROR]   = "ERROR",
    [LL_WARNING] = "WARN ",
    [LL_INFO]    = "INFO ",
    [LL_DEBUG]   = "DEBUG"
};

__attribute__((format(printf, 5, 6)))
void log_write(e_log_level_t level, const char *p_file, uint32_t line, const char *p_func, const char *p_fmt, ...)  // NOLINT(bugprone-easily-swappable-parameters)
{
#if defined(LOG_LEVEL_MAX)
    if ((LL_ERROR <= level) && (level < LL_COUNT) && (level <= LOG_LEVEL_MAX))
    {
        printf("[%s] %s:%u (%s) | ", sg_level_str[level], p_file, line, p_func);
        va_list ap;
        va_start(ap, p_fmt);
        vprintf(p_fmt, ap); // NOLINT(clang-analyzer-valist.Uninitialized)
        va_end(ap);
        printf("\n");
    }
#endif
}