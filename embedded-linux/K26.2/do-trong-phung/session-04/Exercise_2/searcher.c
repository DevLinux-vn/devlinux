#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        errno = EINVAL;
        perror("searcher argument error");
        exit(2);
    }

    char *student_id = argv[1];
    char *filename = argv[2];

    printf("\n[SEARCHER] PID: %d | PPID: %d\n",
           getpid(), getppid());

    printf("[SEARCHER] Searching for \"%s\" in %s...\n",
           student_id, filename);

    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        perror("fopen failed");
        exit(2);
    }

    char line[256];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        /*
         * Remove newline character.
         */
        line[strcspn(line, "\n")] = '\0';

        /*
         * Split the record using '|'
         *
         * fields[0] = ID
         * fields[1] = Name
         * fields[2] = Class
         * fields[3] = GPA
         */
        char *id = strtok(line, "|");
        char *name = strtok(NULL, "|");
        char *class_name = strtok(NULL, "|");
        char *gpa_string = strtok(NULL, "|");

        if (id == NULL || name == NULL ||
            class_name == NULL || gpa_string == NULL)
        {
            continue;
        }

        if (strcmp(id, student_id) == 0)
        {
            float gpa = strtof(gpa_string, NULL);

            const char *grade;

            if (gpa >= 8.5)
            {
                grade = "Excellent";
            }
            else if (gpa >= 7.0)
            {
                grade = "Good";
            }
            else if (gpa >= 5.0)
            {
                grade = "Average";
            }
            else
            {
                grade = "Poor";
            }

            printf("\n========== SEARCH RESULT ==========\n");
            printf("  ID      : %s\n", id);
            printf("  Name    : %s\n", name);
            printf("  Class   : %s\n", class_name);
            printf("  GPA     : %.1f\n", gpa);
            printf("  Grade   : %s\n", grade);
            printf("====================================\n");

            fclose(file);
            exit(0);
        }
    }

    fclose(file);

    printf("[SEARCHER] No student found with ID: %s\n", student_id);

    exit(1);
}