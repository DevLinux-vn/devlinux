#ifndef __INCLUDE_LOGGER_H_
#define __INCLUDE_LOGGER_H_

#pragma once

#include <stdio.h>

#ifdef __cplusplus
    extern "C" {
#endif

/*
 * @brief Writes a log message.
 * @param msg The message to log.
 * @return void
 */
void log_write(const char *msg);

/*
 * @brief Writes a timestamped log entry.
 * @return void
 */
void log_timestamp(void);

/*
 * @brief Writes an error log entry.
 * @param msg The error message to log.
 * @return void
 */
void log_error(const char *msg);

#ifdef __cplusplus
    }
#endif

#endif /*__INCLUDE_LOGGER_H_*/

