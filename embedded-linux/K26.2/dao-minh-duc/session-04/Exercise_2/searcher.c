#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *classify_grade(float gpa)
{
    if (gpa >= 8.5f)
        return "Excellent";
    else if (gpa >= 7.0f)
        return "Good";
    else if (gpa >= 5.0f)
        return "Average";
    else
        return "Poor";
}

int main(int argc, char *argv[])
{
    FILE *fp;
    char line[256];
    int found = 0;

    if (argc != 3) {
        perror("searcher: invalid arguments");
        exit(2);
    }

    const char *student_id = argv[1];
    const char *data_file = argv[2];

    printf("[SEARCHER] PID: %d | PPID: %d\n",
           getpid(), getppid());

    printf("[SEARCHER] Searching for \"%s\" in %s...\n\n",
           student_id, data_file);

    fp = fopen(data_file, "r");

    if (fp == NULL) {
        perror("fopen");
        exit(2);
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        char *id;
        char *name;
        char *class_name;
        char *gpa_string;
        float gpa;

        id = strtok(line, "|");
        name = strtok(NULL, "|");
        class_name = strtok(NULL, "|");
        gpa_string = strtok(NULL, "|");

        if (id == NULL || name == NULL ||
            class_name == NULL || gpa_string == NULL) {
            continue;
        }

        if (strcmp(id, student_id) == 0) {
            gpa = strtof(gpa_string, NULL);

            printf("========== SEARCH RESULT ==========\n");
            printf("  ID      : %s\n", id);
            printf("  Name    : %s\n", name);
            printf("  Class   : %s\n", class_name);
            printf("  GPA     : %.1f\n", gpa);
            printf("  Grade   : %s\n", classify_grade(gpa));
            printf("====================================\n");

            found = 1;
            break;
        }
    }

    fclose(fp);

    if (found) {
        exit(0);
    }

    printf("[SEARCHER] No student found with ID: %s\n",
           student_id);

    exit(1);
}
