#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    FILE *file;
    char line[200];

    /* Check arguments */
    if (argc != 3) {
        perror("Invalid arguments");
        exit(2);
    }

    char *student_id = argv[1];
    char *file_path = argv[2];

    printf("[SEARCHER] PID: %d | PPID: %d\n",
           getpid(), getppid());

    printf("[SEARCHER] Searching for \"%s\" in %s...\n\n",
           student_id, file_path);

    /* Open students.txt */
    file = fopen(file_path, "r");

    if (file == NULL) {
        perror("fopen failed");
        exit(2);
    }

    /* Read file line by line */
    while (fgets(line, sizeof(line), file) != NULL) {

        /*
         * Format:
         * ID|Name|Class|GPA
         */

        char *id = strtok(line, "|");
        char *name = strtok(NULL, "|");
        char *class_name = strtok(NULL, "|");
        char *gpa_string = strtok(NULL, "|\n");

        if (id == NULL ||
            name == NULL ||
            class_name == NULL ||
            gpa_string == NULL) {
            continue;
        }

        /* Compare ID from file with ID entered by user */
        if (strcmp(id, student_id) == 0) {

            float gpa = atof(gpa_string);
            char *grade;

            if (gpa >= 8.5) {
                grade = "Excellent";
            }
            else if (gpa >= 7.0) {
                grade = "Good";
            }
            else if (gpa >= 5.0) {
                grade = "Average";
            }
            else {
                grade = "Poor";
            }

            printf("========== SEARCH RESULT ==========\n");
            printf("  ID      : %s\n", id);
            printf("  Name    : %s\n", name);
            printf("  Class   : %s\n", class_name);
            printf("  GPA     : %.1f\n", gpa);
            printf("  Grade   : %s\n", grade);
            printf("====================================\n");

            fclose(file);

            /* Student found */
            exit(0);
        }
    }

    /* Reached end of file without finding student */
    fclose(file);

    printf("[SEARCHER] No student found with ID: %s\n",
           student_id);

    exit(1);
}