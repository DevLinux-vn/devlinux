#ifndef STRINGUTILS_H
#define STRINGUTILS_H

/* Convert a string to uppercase in-place */
void str_to_upper(char *s);

/* Count the number of characters in a string (excluding null terminator) */
int  str_count(const char *s);

/* Reverse a string in-place */
void str_reverse(char *s);

#endif /* STRINGUTILS_H */
