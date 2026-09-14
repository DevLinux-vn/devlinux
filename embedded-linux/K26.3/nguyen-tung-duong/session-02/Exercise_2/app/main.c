#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stringutils.h"

#define MAX_STR_LEN 256

int main() {
    char buffer[MAX_STR_LEN];
    char working[MAX_STR_LEN];
    
    printf("======================================\n");
    printf("String Utilities Program\n");
    printf("======================================\n");
    printf("Enter a string: ");
    fflush(stdout);
    
    /* Read input from keyboard */
    if (fgets(buffer, MAX_STR_LEN, stdin) == NULL) {
        printf("Error: Failed to read input\n");
        return 1;
    }
    
    /* Remove newline character if present */
    int len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    
    printf("\nOriginal string: \"%s\"\n\n", buffer);
    
    /* Test 1: Count characters */
    int count = str_count(buffer);
    printf("[1] Character count: %d\n", count);
    
    /* Test 2: Convert to uppercase */
    strcpy(working, buffer);
    str_to_upper(working);
    printf("[2] Uppercase: %s\n", working);
    
    /* Test 3: Reverse string */
    strcpy(working, buffer);
    str_reverse(working);
    printf("[3] Reversed: %s\n", working);
    
    printf("\n======================================\n");
    
    return 0;
}
