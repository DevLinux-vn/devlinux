#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LINE_SIZE 256

static const char *get_grade(double gpa)
{
    if (gpa >= 8.5)
        return "Excellent";
    else if (gpa >= 7.0)
        return "Good";
    else if (gpa >= 5.0)
        return "Average";
    else
        return "Poor";
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        perror("Invalid arguments");
        exit(2);
    }

    char *student_id = argv[1];
    char *file_path = argv[2];

    printf("\n[SEARCHER] PID: %d | PPID: %d\n",
           getpid(),
           getppid());

    printf("[SEARCHER] Searching for \"%s\" in %s...\n",
           student_id,
           file_path);

    FILE *fp = fopen(file_path, "r");

    if (fp == NULL)
    {
        perror("fopen");
        exit(2);
    }

    char line[LINE_SIZE];

    while (fgets(line, sizeof(line), fp))
    {
        line[strcspn(line, "\n")] = '\0';

        char *id = strtok(line, "|");
        char *name = strtok(NULL, "|");
        char *class_name = strtok(NULL, "|");
        char *gpa_str = strtok(NULL, "|");

        if (!id || !name || !class_name || !gpa_str)
            continue;

        if (strcmp(id, student_id) == 0)
        {
            double gpa = atof(gpa_str);

            printf("\n=========== SEARCH RESULT ===========\n");
            printf("ID      : %s\n", id);
            printf("Name    : %s\n", name);
            printf("Class   : %s\n", class_name);
            printf("GPA     : %.1f\n", gpa);
            printf("Grade   : %s\n", get_grade(gpa));
            printf("=====================================\n\n");

            fclose(fp);
            exit(0);
        }
    }

    fclose(fp);

    printf("[SEARCHER] No student found with ID: %s\n",
           student_id);

    exit(1);
}