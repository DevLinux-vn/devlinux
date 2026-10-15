#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        return 2;
    }

    FILE *fp;

    fp = fopen(argv[2], "r");

    if (fp == NULL)
    {
        perror("fopen");
        return 2;
    }

    char line[256];

    while (fgets(line,
                 sizeof(line),
                 fp) != NULL)
    {
        char *id;
        char *name;
        char *class_name;
        char *gpa;

        id = strtok(line, "|");
        name = strtok(NULL, "|");
        class_name = strtok(NULL, "|");
        gpa = strtok(NULL, "|");

        if (id == NULL)
        {
            continue;
        }

        if (strcmp(id, argv[1]) == 0)
        {
            printf("\n");
            printf("ID    : %s\n", id);
            printf("Name  : %s\n", name);
            printf("Class : %s\n", class_name);
            printf("GPA   : %s\n", gpa);

            fclose(fp);

            return 0;
        }
    }

    fclose(fp);

    return 1;
}

