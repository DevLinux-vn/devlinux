#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Helper function to classify grade based on GPA.
 */
const char* get_grade_classification(float gpa) {
    if (gpa >= 8.5) return "Excellent";
    if (gpa >= 7.0) return "Good";
    if (gpa >= 5.0) return "Average";
    return "Poor";
}

int main(int argc, char *argv[]) {
    /* Print process details immediately to show image replacement proof */
    printf("[SEARCHER] PID: %d | PPID: %d\n", getpid(), getppid());

    /* Validate argument constraints */
    if (argc < 3) {
        fprintf(stderr, "[SEARCHER] Error: Missing arguments. Usage: %s <student_id> <data_file>\n", argv[0]);
        exit(2);
    }

    char *target_id = argv[1];
    char *file_path = argv[2];

    printf("[SEARCHER] Searching for \"%s\" in %s...\n", target_id, file_path);

    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("[SEARCHER] Error opening database file");
        exit(2); /* File error returns 2 */
    }

    char line[256];
    int found = 0;

    /* Read database line by line */
    while (fgets(line, sizeof(line), file)) {
        /* Strip potential trailing windows/linux newline characters */
        line[strcspn(line, "\r\n")] = '\0';

        /* Create a working copy for destructive strtok tokens */
        char line_copy[256];
        strcpy(line_copy, line);

        /* Extract tokens using pipe delimiter */
        char *id = strtok(line_copy, "|");
        char *name = strtok(NULL, "|");
        char *class_name = strtok(NULL, "|");
        char *gpa_str = strtok(NULL, "|");

        /* Defensive check for malformed database lines */
        if (!id || !name || !class_name || !gpa_str) {
            continue;
        }

        /* Match Student ID */
        if (strcmp(id, target_id) == 0) {
            float gpa = atof(gpa_str);
            
            /* Print record in specified format */
            printf("\n========== SEARCH RESULT ==========\n");
            printf("  ID      : %s\n", id);
            printf("  Name    : %s\n", name);
            printf("  Class   : %s\n", class_name);
            printf("  GPA     : %.1f\n", gpa);
            printf("  Grade   : %s\n", get_grade_classification(gpa));
            printf("====================================\n");
            
            found = 1;
            break; /* Optimization: Stop reading once target is found */
        }
    }

    fclose(file);

    if (found) {
        exit(0); /* Found exit contract */
    } else {
        printf("[SEARCHER] No student found with ID: %s\n", target_id);
        exit(1); /* Not found exit contract */
    }
}