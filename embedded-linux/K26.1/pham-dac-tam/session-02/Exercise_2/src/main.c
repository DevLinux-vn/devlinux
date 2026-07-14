#include <stdio.h>
#include <string.h>
#include "stringutils.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }
    
    // Make a copy of the input string for manipulation
    char str[256];
    strncpy(str, argv[1], sizeof(str) - 1);
    str[sizeof(str) - 1] = '\0';
    
    printf("=== Shared Library Demo (stringutils) ===\n\n");
    printf("Input string: %s\n\n", str);
    
    // Test to_uppercase
    char upper_copy[256];
    strncpy(upper_copy, str, sizeof(upper_copy) - 1);
    upper_copy[sizeof(upper_copy) - 1] = '\0';
    to_uppercase(upper_copy);
    printf("Uppercase: %s\n", upper_copy);
    
    // Test string_length
    int len = string_length(str);
    printf("Length: %d\n", len);
    
    // Test reverse_string
    char reverse_copy[256];
    strncpy(reverse_copy, str, sizeof(reverse_copy) - 1);
    reverse_copy[sizeof(reverse_copy) - 1] = '\0';
    reverse_string(reverse_copy);
    printf("Reversed: %s\n", reverse_copy);
    
    return 0;
}
