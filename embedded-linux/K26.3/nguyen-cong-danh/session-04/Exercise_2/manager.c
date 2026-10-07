#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUF_SIZE 64

int main(void)
{
    char student_id[BUF_SIZE];

    printf("===========================================\n");
    printf("     STUDENT LOOKUP SYSTEM - MANAGER\n");
    printf("      (fork + execve | students.txt)\n");
    printf("===========================================\n");

    printf("[MANAGER] PID: %d\n", getpid());
    printf("Enter student ID ('quit' to exit).\n");

    while (1)
    {
        printf("\n-------------------------------------------\n");
        printf("Student ID: ");

        if (fgets(student_id, sizeof(student_id), stdin) == NULL)
            break;

        student_id[strcspn(student_id, "\n")] = '\0';

        if (strcmp(student_id, "quit") == 0)
        {
            printf("[MANAGER] Exiting. Goodbye!\n");
            break;
        }

        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork");
            continue;
        }

        if (pid == 0)
        {
            char *envp[] = { NULL };

            char *argv[] = {
                "./searcher",
                student_id,
                "students.txt",
                NULL
            };

            execve("./searcher", argv, envp);

            perror("execve");

            exit(2);
        }

        printf("\n[MANAGER] fork() -> child PID: %d\n", pid);
        printf("[MANAGER] Waiting for child (waitpid)...\n");

        int status;

        waitpid(pid, &status, 0);

        if (WIFEXITED(status))
        {
            int code = WEXITSTATUS(status);

            switch (code)
            {
                case 0:
                    printf("\n[MANAGER] Child (PID %d) exited. code=0 -> Found\n",
                           pid);
                    break;

                case 1:
                    printf("\n[MANAGER] Child (PID %d) exited. code=1 -> Not found\n",
                           pid);
                    break;

                case 2:
                    printf("\n[MANAGER] Child (PID %d) exited. code=2 -> Error\n",
                           pid);
                    break;

                default:
                    printf("\n[MANAGER] Child (PID %d) exited. code=%d\n",
                           pid,
                           code);
                    break;
            }
        }
    }

    return 0;
}