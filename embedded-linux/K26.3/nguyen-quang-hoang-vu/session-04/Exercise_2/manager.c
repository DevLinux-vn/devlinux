#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

int main(void)
{
    char student_id[32];

    while (1)
    {
        printf("\n");
        printf("Enter student ID (or quit): ");

        if (scanf("%31s", student_id) != 1)
        {
            printf("Invalid input\n");
            continue;
        }

        if (strcmp(student_id, "quit") == 0)
        {
            break;
        }

        pid_t pid;

        pid = fork();

        if (pid < 0)
        {
            perror("fork");
            continue;
        }

        if (pid == 0)
        {
            char *args[] =
            {
                "./searcher",
                student_id,
                "students.txt",
                NULL
            };

            execve("./searcher",
                   args,
                   environ);

            perror("execve");

            exit(2);
        }

        int status;

        waitpid(pid,
                &status,
                0);

        if (WIFEXITED(status))
        {
            int code;

            code = WEXITSTATUS(status);

            if (code == 0)
            {
                printf("[PARENT] Student found\n");
            }
            else if (code == 1)
            {
                printf("[PARENT] Student not found\n");
            }
            else
            {
                printf("[PARENT] File error\n");
            }
        }
    }

    return 0;
}
