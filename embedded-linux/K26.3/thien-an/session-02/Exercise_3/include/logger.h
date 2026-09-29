#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>

/* include/logger.h */
void log_write(const char *msg);
void log_timestamp(void);
void log_error(const char *msg);

#endif