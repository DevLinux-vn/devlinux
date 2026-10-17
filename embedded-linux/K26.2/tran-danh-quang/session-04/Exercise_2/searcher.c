#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE_LEN 256

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <student_id> <data_file>\n", argv[0]);
        exit(2);
    }

    char *student_id = argv[1];
    char *data_file = argv[2];

    printf("[SEARCHER] PID: %d | PPID: %d\n", getpid(), getppid());
    printf("[SEARCHER] Searching for \"%s\" in %s...\n\n", student_id, data_file);

    FILE *file = fopen(data_file, "r");
    if (file == NULL) {
        perror("fopen failed");
        exit(2);
    }

    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';

        char *line_copy = strdup(line); // copy to keep original since strtok modifies it
        if (!line_copy) {
            perror("strdup failed");
            fclose(file);
            exit(2);
        }

        char *id = strtok(line, "|");
        char *name = strtok(NULL, "|");
        char *class_name = strtok(NULL, "|");
        char *gpa_str = strtok(NULL, "|");

        if (id && name && class_name && gpa_str) {
            if (strcmp(id, student_id) == 0) {
                float gpa = atof(gpa_str);
                char *grade = "";
                if (gpa >= 8.5) grade = "Excellent";
                else if (gpa >= 7.0) grade = "Good";
                else if (gpa >= 5.0) grade = "Average";
                else grade = "Poor";

                printf("========== SEARCH RESULT ==========\n");
                printf("  ID      : %s\n", id);
                printf("  Name    : %s\n", name);
                printf("  Class   : %s\n", class_name);
                printf("  GPA     : %s\n", gpa_str);
                printf("  Grade   : %s\n", grade);
                printf("====================================\n");

                free(line_copy);
                fclose(file);
                exit(0);
            }
        }
        free(line_copy);
    }

    if (ferror(file)) {
        perror("fgets error");
        fclose(file);
        exit(2);
    }

    fclose(file);
    printf("[SEARCHER] No student found with ID: %s\n", student_id);
    exit(1);
}
