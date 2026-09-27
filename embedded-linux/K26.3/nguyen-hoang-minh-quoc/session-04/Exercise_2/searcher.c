#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *classify_grade(float gpa)
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
    const char *target_id;
    const char *file_path;
    FILE *fp;
    char line[256];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <student_id> <students_file>\n", argv[0]);
        return 2;
    }

    target_id = argv[1];
    file_path = argv[2];

    printf("[SEARCHER] PID: %d | PPID: %d\n", getpid(), getppid());
    printf("[SEARCHER] Searching for \"%s\" in %s...\n", target_id, file_path);

    fp = fopen(file_path, "r");
    if (fp == NULL) {
        perror("fopen");
        return 2;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        char *id = strtok(line, "|\n");
        char *name = strtok(NULL, "|\n");
        char *class_name = strtok(NULL, "|\n");
        char *gpa_text = strtok(NULL, "|\n");
        float gpa;

        if (id == NULL || name == NULL || class_name == NULL || gpa_text == NULL) {
            continue;
        }

        if (strcmp(id, target_id) != 0) {
            continue;
        }

        gpa = strtof(gpa_text, NULL);

        printf("\n========== SEARCH RESULT ==========\n");
        printf("  ID      : %s\n", id);
        printf("  Name    : %s\n", name);
        printf("  Class   : %s\n", class_name);
        printf("  GPA     : %.1f\n", gpa);
        printf("  Grade   : %s\n", classify_grade(gpa));
        printf("====================================\n\n");

        fclose(fp);
        return 0;
    }

    fclose(fp);
    printf("[SEARCHER] No student found with ID: %s\n\n", target_id);
    return 1;
}
