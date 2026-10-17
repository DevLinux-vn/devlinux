#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LINE_SIZE 256

const char *classify_grade(float gpa) {
    if (gpa >= 8.5f) {
        return "Excellent";
    }

    if (gpa >= 7.0f) {
        return "Good";
    }

    if (gpa >= 5.0f) {
        return "Average";
    }

    return "Poor";
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <student_id> <students_file>\n", argv[0]);
        exit(2);
    }

    const char *target_id = argv[1];
    const char *file_path = argv[2];

    printf("[SEARCHER] PID: %d | PPID: %d\n", getpid(), getppid());
    printf("[SEARCHER] Searching for \"%s\" in %s...\n", target_id, file_path);

    FILE *file = fopen(file_path, "r");

    if (file == NULL) {
        perror("fopen");
        exit(2);
    }

    char line[LINE_SIZE];

    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        char original_line[LINE_SIZE];
        strncpy(original_line, line, sizeof(original_line) - 1);
        original_line[sizeof(original_line) - 1] = '\0';

        char *id = strtok(line, "|");
        char *name = strtok(NULL, "|");
        char *class_name = strtok(NULL, "|");
        char *gpa_text = strtok(NULL, "|");

        if (id == NULL || name == NULL || class_name == NULL || gpa_text == NULL) {
            continue;
        }

        if (strcmp(id, target_id) == 0) {
            float gpa = atof(gpa_text);

            printf("\n========== SEARCH RESULT ==========\n");
            printf("  ID      : %s\n", id);
            printf("  Name    : %s\n", name);
            printf("  Class   : %s\n", class_name);
            printf("  GPA     : %.1f\n", gpa);
            printf("  Grade   : %s\n", classify_grade(gpa));
            printf("====================================\n");

            fclose(file);
            exit(0);
        }

        (void)original_line;
    }

    fclose(file);

    printf("[SEARCHER] No student found with ID: %s\n", target_id);
    exit(1);
}