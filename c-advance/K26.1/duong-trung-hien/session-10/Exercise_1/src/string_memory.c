#include "string_memory.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>



const char* get_memory_region(const void *p_addr) {
    if (NULL == p_addr) {
        return "UNKNOWN";
    }

    FILE *fp;
    char line[512];
    uintptr_t addr = (uintptr_t)p_addr;
    uintptr_t start;
    uintptr_t end;

    fp = fopen("/proc/self/maps", "r");
    if (NULL == fp) {
        return "UNKNOWN";
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if ((strstr(line, "[heap]")) != NULL || (strstr(line, "[stack]") != NULL)) {
            if (sscanf(line, "%lx-%lx", &start, &end) == 2) {
                if (addr >= start && addr < end) {
                    if (strstr(line, "[heap]") != NULL) {
                        return HEAP_LABEL;
                    }
                    else if (strstr(line, "[stack]") != NULL) {
                        return STACK_LABEL;
                    }
                }
            }
        }
    }

    fclose(fp);
    return RODATA_LABEL;
}

char* heap_string_copy(const char *p_src) {
    if (NULL == p_src || *p_src == '\0') {
        return NULL;
    }

    size_t len = strlen(p_src);
    char *p_des = malloc(len + 1);

    if (NULL == p_des) {
        return NULL;
    }

    strcpy(p_des, p_src);

    return p_des;
}