#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LINE_SIZE 256

const char *classify_grade(float gpa)
{
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

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "[SEARCHER] Usage: %s <student_id> <file_path>\n", argv[0]);
        exit(2);
    }

    const char *target_id = argv[1];
    const char *file_path = argv[2];

    printf("[SEARCHER] PID: %d | PPID: %d\n", getpid(), getppid());
    printf("[SEARCHER] Searching for \"%s\" in %s...\n", target_id, file_path);

    FILE *fp = fopen(file_path, "r");

    if (fp == NULL) {
        perror("fopen");
        exit(2);
    }

    char line[LINE_SIZE];

    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        char *id = strtok(line, "|");
        char *name = strtok(NULL, "|");
        char *class_name = strtok(NULL, "|");
        char *gpa_str = strtok(NULL, "|");

        if (id == NULL || name == NULL || class_name == NULL || gpa_str == NULL) {
            continue;
        }

        if (strcmp(id, target_id) == 0) {
            float gpa = atof(gpa_str);

            printf("\n========== SEARCH RESULT ==========\n");
            printf("  ID      : %s\n", id);
            printf("  Name    : %s\n", name);
            printf("  Class   : %s\n", class_name);
            printf("  GPA     : %.1f\n", gpa);
            printf("  Grade   : %s\n", classify_grade(gpa));
            printf("====================================\n");

            fclose(fp);
            exit(0);
        }
    }

    fclose(fp);

    printf("[SEARCHER] No student found with ID: %s\n", target_id);
    exit(1);
}