#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ARG_COUNT          3
#define LINE_SIZE          256U

#define FIELD_ID           0U
#define FIELD_NAME         1U
#define FIELD_CLASS        2U
#define FIELD_GPA          3U
#define FIELD_COUNT        4U

#define DELIMITER          "|"

#define NOT_FOUND_CODE     1
#define ERROR_CODE         2

#define EXCELLENT_GPA      8.5F
#define GOOD_GPA           7.0F
#define AVERAGE_GPA        5.0F


static const char *get_grade(float gpa)
{
    if (gpa >= EXCELLENT_GPA)
    {
        return "Excellent";
    }

    if (gpa >= GOOD_GPA)
    {
        return "Good";
    }

    if (gpa >= AVERAGE_GPA)
    {
        return "Average";
    }

    return "Poor";
}


int main(int argc, char *argv[])
{
    if (argc != ARG_COUNT)
    {
        fprintf(stderr,
                "Invalid arguments\n");

        return ERROR_CODE;
    }


    printf("[SEARCHER] PID: %d | PPID: %d\n",
            getpid(),
            getppid());

    printf("[SEARCHER] Searching ID %s in %s\n\n",
            argv[1],
            argv[2]);


    FILE *file = fopen(argv[2], "r");


    if (file == NULL)
    {
        perror("fopen failed");
        return ERROR_CODE;
    }


    char line[LINE_SIZE];


    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *fields[FIELD_COUNT];

        char *token = strtok(line, DELIMITER);

        size_t index = 0U;


        while ((token != NULL) && (index < FIELD_COUNT))
        {
            fields[index] = token;

            index++;

            token = strtok(NULL, DELIMITER);
        }


        if (index != FIELD_COUNT)
        {
            continue;
        }


        if (strcmp(fields[FIELD_ID], argv[1]) == 0)
        {
            float gpa = strtof(fields[FIELD_GPA], NULL);


            printf("========== RESULT ==========\n");

            printf("ID    : %s", fields[FIELD_ID]);
            printf("Name  : %s", fields[FIELD_NAME]);
            printf("Class : %s", fields[FIELD_CLASS]);
            printf("GPA   : %.1f", gpa); 
            printf("Grade : %s\n",
                    get_grade(gpa));  
                    

            fclose(file);
            return EXIT_SUCCESS;
        }
    
        if (strcmp(fields[FIELD_ID], argv[1]) == 0)
        {
            float gpa = strtof(fields[FIELD_GPA], NULL);

            printf("========== SEARCH RESULT ==========\n");
            printf("ID    : %s\n", fields[FIELD_ID]);
            printf("Name  : %s\n", fields[FIELD_NAME]);
            printf("Class : %s\n", fields[FIELD_CLASS]);
            printf("GPA   : %.1f\n", gpa);
            printf("Grade : %s\n", get_grade(gpa));
            printf("===================================\n");


            if (fclose(file) != 0)
            {
                perror("fclose failed");
                return ERROR_CODE;
            }


            return EXIT_SUCCESS;
        }
    }


    /*
     * Distinguish between EOF and file read error.
     */
    if (ferror(file))
    {
        perror("fgets failed");

        if (fclose(file) != 0)
        {
            perror("fclose failed");
        }

        return ERROR_CODE;
    }


    printf("[SEARCHER] No student found with ID: %s\n",
            argv[1]);


    if (fclose(file) != 0)
    {
        perror("fclose failed");
        return ERROR_CODE;
    }


    return NOT_FOUND_CODE;
}