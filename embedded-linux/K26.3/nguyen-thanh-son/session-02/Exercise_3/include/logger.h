#ifndef LOGGER_H
#define LOGGER_H

#define LOG_FILE "app.log"

/* Append "[YYYY-MM-DD HH:MM:SS] msg" to LOG_FILE */
void log_write(const char *msg);

/* Print the current timestamp "YYYY-MM-DD HH:MM:SS" to stdout */
void log_timestamp(void);

/* Append "[YYYY-MM-DD HH:MM:SS] [ERROR] msg" to LOG_FILE */
void log_error(const char *msg);

#endif
