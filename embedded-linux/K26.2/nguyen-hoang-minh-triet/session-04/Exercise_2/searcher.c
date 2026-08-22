#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char* get_grade(float gpa) {
    if (gpa >= 8.5f) return "Excellent";
    if (gpa >= 7.0f) return "Good";
    if (gpa >= 5.0f) return "Average";
    return "Poor";
}

int main(int argc, char *argv[]) {
    
    // 1. Check command-line arguments (expects: ./searcher <student_id> <data_file>)
    if (argc < 3) {
        fprintf(stderr, "[SEARCHER] Usage: %s <student_id> <data_file>\n", argv[0]);
        exit(2);
    }

    const char *target_id = argv[1];
    const char *filepath = argv[2];

    printf("[SEARCHER] PID: %d | PPID: %d\n", getpid(), getppid());
    printf("[SEARCHER] Searching for \"%s\" in %s...\n", target_id, filepath);

    // 2. Open the data file
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        perror("[SEARCHER] Failed to open data file");
        exit(2);
    }

    char line[256];
    int found = 0;

    // 3. Process line by line
    while (fgets(line, sizeof(line), file) != NULL) {

        // Remove trailing newline character
        line[strcspn(line, "\r\n")] = '\0';

        if (strlen(line) == 0) continue;
        
        // Duplicate line because strtok modifies the input string
        char line_copy[256];
        strncpy(line_copy, line, sizeof(line_copy) - 1);
        line_copy[sizeof(line_copy) - 1] = '\0';

        char *id = strtok(line_copy, "|");
        char *name = strtok(NULL, "|");
        char *cls = strtok(NULL, "|");
        char *gpa_str = strtok(NULL, "|");

        if (id && name && cls && gpa_str) {
            if (strcmp(id, target_id) == 0) {
                float gpa = atof(gpa_str);
                const char *grade = get_grade(gpa);

                printf("\n========== SEARCH RESULT ==========\n");
                printf("  ID      : %s\n", id);
                printf("  Name    : %s\n", name);
                printf("  Class   : %s\n", cls);
                printf("  GPA     : %.1f\n", gpa);
                printf("  Grade   : %s\n", grade);
                printf("====================================\n");

                found = 1;
                break;
            }
        }
    }

    if (fclose(file) != 0) {
        perror("[SEARCHER] Warning: fclose failed");
    }

    // 4. Return exit status according to contract
    if (found) {
        exit(0);
    } else {
        printf("[SEARCHER] No student found with ID: %s\n", target_id);
        exit(1);
    }
}


