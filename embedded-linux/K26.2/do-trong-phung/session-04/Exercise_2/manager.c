#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

int main(void)
{
    char student_id[100];

    printf("=============================================\n");
    printf("   STUDENT LOOKUP SYSTEM - MANAGER\n");
    printf("   (fork + execve | file: students.txt)\n");
    printf("=============================================\n");

    printf("[MANAGER] PID: %d\n", getpid());
    printf("Enter student ID ('quit' to exit).\n");

    while (1)
    {
        printf("\n---------------------------------------------\n");
        printf("Student ID: ");

        if (fgets(student_id, sizeof(student_id), stdin) == NULL)
        {
            break;
        }

        /* Remove newline */
        student_id[strcspn(student_id, "\n")] = '\0';

        if (strcmp(student_id, "quit") == 0)
        {
            printf("[MANAGER] Exiting. Goodbye!\n");
            break;
        }

        if (strlen(student_id) == 0)
        {
            printf("[MANAGER] Student ID cannot be empty.\n");
            continue;
        }

        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork failed");
            continue;
        }

        if (pid == 0)
        {
            /*
             * Child process:
             * Replace the current process image with searcher.
             */
            char *args[] = {
                "./searcher",
                student_id,
                "students.txt",
                NULL
            };

            execve("./searcher", args, environ);

            /*
             * This line is normally never reached because a successful
             * execve() completely replaces the current process image.
             * It is reached only when execve() fails.
             */
            perror("execve failed");
            exit(2);
        }
        else
        {
            /*
             * Parent process
             */
            int status;

            printf("[MANAGER] fork() -> child PID: %d\n", pid);
            printf("[MANAGER] Waiting for child (waitpid)...\n");

            if (waitpid(pid, &status, 0) == -1)
            {
                perror("waitpid failed");
                continue;
            }

            if (WIFEXITED(status))
            {
                int exit_code = WEXITSTATUS(status);

                if (exit_code == 0)
                {
                    printf("\n[MANAGER] Child (PID %d) exited. "
                           "code=0 -> Found\n", pid);
                }
                else if (exit_code == 1)
                {
                    printf("\n[MANAGER] Child (PID %d) exited. "
                           "code=1 -> Not found\n", pid);
                }
                else if (exit_code == 2)
                {
                    printf("\n[MANAGER] Child (PID %d) exited. "
                           "code=2 -> Error\n", pid);
                }
                else
                {
                    printf("\n[MANAGER] Child (PID %d) exited. "
                           "code=%d -> Unknown\n", pid, exit_code);
                }
            }
            else if (WIFSIGNALED(status))
            {
                printf("\n[MANAGER] Child (PID %d) terminated by signal %d\n",
                       pid, WTERMSIG(status));
            }
        }
    }

    return 0;
}