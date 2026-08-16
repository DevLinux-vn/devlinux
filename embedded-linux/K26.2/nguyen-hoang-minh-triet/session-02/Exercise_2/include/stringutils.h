#ifndef __INCLUDE_STRINGUTILS_H_
#define __INCLUDE_STRINGUTILS_H_

#pragma once

#include <stdio.h>

#ifdef __cplusplus
    extern "C" {
#endif

/*
 * @brief Converts a string to uppercase.
 * @param s The string to convert.
 * @return void
 */
void str_to_upper(char *s);

/*
 * @brief Counts the number of characters in a string.
 * @param s The string to count.
 * @return int
 */
int str_count(const char *s);

/*
 * @brief Reverses a string in place.
 * @param s The string to reverse.
 * @return void
 */
void str_reverse(char *s);

#ifdef __cplusplus
    }
#endif

#endif /*__INCLUDE_STRINGUTILS_H_*/