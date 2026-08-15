#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LINE_SIZE 256

int main(int argc, char *argv[])
{
    /*
     * We need:
     *
     * argv[0] = ./searcher
     * argv[1] = student ID
     * argv[2] = data file
     */
    if (argc != 3) {
        fprintf(stderr,
                "Usage: %s <student_id> <data_file>\n",
                argv[0]);

        exit(2);
    }

    char *student_id = argv[1];
    char *filename = argv[2];

    /*
     * Print process information.
     */
    printf("[SEARCHER] PID: %d | PPID: %d\n",
           getpid(), getppid());

    printf("[SEARCHER] Searching for \"%s\" in %s...\n",
           student_id, filename);

    /*
     * Open students.txt.
     */
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("fopen");
        exit(2);
    }

    char line[LINE_SIZE];

    /*
     * Used to remember whether we found the student.
     */
    int found = 0;

    /*
     * Read file line by line.
     */
    while (fgets(line, sizeof(line), file) != NULL) {

        /*
         * Remove newline.
         */
        line[strcspn(line, "\n")] = '\0';

        /*
         * Split the line using '|'.
         *
         * Example:
         *
         * SV003|Le Van Cuong|HTTT02|9.1
         *
         * becomes:
         *
         * ID     = SV003
         * Name   = Le Van Cuong
         * Class  = HTTT02
         * GPA    = 9.1
         */
        char *id = strtok(line, "|");
        char *name = strtok(NULL, "|");
        char *class_name = strtok(NULL, "|");
        char *gpa_string = strtok(NULL, "|");

        /*
         * Make sure all fields exist.
         */
        if (id == NULL ||
            name == NULL ||
            class_name == NULL ||
            gpa_string == NULL) {
            continue;
        }

        /*
         * Compare student ID.
         */
        if (strcmp(id, student_id) == 0) {

            found = 1;

            /*
             * Convert GPA from string to float.
             */
            float gpa = atof(gpa_string);

            /*
             * Determine grade classification.
             */
            const char *grade;

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

            /*
             * Print search result.
             */
            printf("\n========== SEARCH RESULT ==========\n");
            printf("  ID      : %s\n", id);
            printf("  Name    : %s\n", name);
            printf("  Class   : %s\n", class_name);
            printf("  GPA     : %.1f\n", gpa);
            printf("  Grade   : %s\n", grade);
            printf("====================================\n");

            /*
             * Student found.
             */
            fclose(file);
            exit(0);
        }
    }

    /*
     * Close file after finishing the search.
     */
    fclose(file);

    /*
     * Student was not found.
     */
    if (!found) {
        printf("[SEARCHER] No student found with ID: %s\n",
               student_id);

        exit(1);
    }

    return 0;
}
