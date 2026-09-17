#ifndef LIB_LOGGER
#define LIB_LOGGER

void log_write(const char *msg);
void log_timestamp(void);
void log_error(const char *msg);

#endif