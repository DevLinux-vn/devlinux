#ifndef LOGGER_H
#define LOGGER_H

/* Write a plain log entry to app.log (with a timestamp prefix) */
void log_write(const char *msg);

/* Print the current timestamp (YYYY-MM-DD HH:MM:SS) to stdout */
void log_timestamp(void);

/* Write an error log entry prefixed with [ERROR] to app.log */
void log_error(const char *msg);

#endif /* LOGGER_H */
