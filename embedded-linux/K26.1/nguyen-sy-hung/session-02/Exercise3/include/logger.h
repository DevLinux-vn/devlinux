#ifndef LOGGER_H
#define LOGGER_H

void logger_print_timestamp(void);
void logger_log(const char *message);
void logger_error(const char *message);

#endif