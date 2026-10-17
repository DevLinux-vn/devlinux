#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

typedef enum {
    LOG_LEVEL_ERROR = 0,
    LOG_LEVEL_WARNING = 1,
    LOG_LEVEL_INFO = 2,
    LOG_LEVEL_DEBUG = 3,
    LOG_LEVEL_COUNT
} log_level_t;

#ifndef LOG_LEVEL_MAX
#define LOG_LEVEL_MAX LOG_LEVEL_DEBUG
#endif

#define LOG_BUF_SIZE 256U

static const char *s_level_str[LOG_LEVEL_COUNT] = {
    "ERROR", "WARNING", "INFO", "DEBUG"
};

__attribute__((format(printf, 5, 6)))
void log_write(log_level_t level, const char *file, uint32_t line, const char *func, const char *fmt, ...){
    if(level > LOG_LEVEL_MAX) {
        return;
    }
    char buf[LOG_BUF_SIZE];
    int32_t offset = snprintf(buf, sizeof(buf), "[%s] %s:%u (%s) | ",s_level_str[level],file,line,func);
    if( offset > 0 && (uint32_t)offset < sizeof(buf)) {
        va_list ap;
        va_start(ap,fmt);
        vsnprintf(buf+offset, sizeof(buf) - (uint32_t)offset, fmt,ap);
        va_end(ap);
    }
    puts(buf);
}

#define LOG_ERROR(fmt, ...) \
    do{\
        if(LOG_LEVEL_ERROR <= LOG_LEVEL_MAX){\
            log_write(LOG_LEVEL_ERROR, __FILE__, __LINE__, \
                __func__, fmt, ##__VA_ARGS__); \
        }\
    }while(0)

#define LOG_WARNING(fmt, ...) \
    do{\
        if(LOG_LEVEL_WARNING <= LOG_LEVEL_MAX){\
            log_write(LOG_LEVEL_WARNING, __FILE__, __LINE__, \
                __func__, fmt, ##__VA_ARGS__); \
        }\
    }while(0)

#define LOG_INFO(fmt, ...) \
    do{\
        if(LOG_LEVEL_INFO <= LOG_LEVEL_MAX){\
            log_write(LOG_LEVEL_INFO, __FILE__, __LINE__, \
                __func__, fmt, ##__VA_ARGS__); \
        }\
    }while(0)

#define LOG_DEBUG(fmt, ...) \
    do{\
        if(LOG_LEVEL_DEBUG <= LOG_LEVEL_MAX){\
            log_write(LOG_LEVEL_DEBUG, __FILE__, __LINE__, \
                __func__, fmt, ##__VA_ARGS__); \
        }\
    }while(0)

/**
 * @brief Program entry point. Demonstrates all four log levels.
 *
 * @return 0 on successful completion.
 */
int main(void)
{
    printf("=== Exercise 3: Industrial Logger ===\n");
    printf("Compiled with LOG_LEVEL_MAX = %d\n", (int)LOG_LEVEL_MAX);

    LOG_INFO("System boot. Build time: 12:00:00");
    LOG_DEBUG("Discovered %d sensors on I2C bus.", 4);
    LOG_WARNING("Sensor %d reading is unstable.", 2);
    LOG_ERROR("Watchdog timeout! Rebooting in %d ms.", 500);
    LOG_INFO("Boot sequence complete.");

    return 0;
}
