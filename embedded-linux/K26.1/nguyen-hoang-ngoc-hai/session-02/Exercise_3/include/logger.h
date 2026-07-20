#ifndef LOGGER_H
#define LOGGER_H

void log_init(void);
void log_close(void);

void log_write(const char *msg);
void log_timestamp(void);
void log_error(const char *msg);

#endif