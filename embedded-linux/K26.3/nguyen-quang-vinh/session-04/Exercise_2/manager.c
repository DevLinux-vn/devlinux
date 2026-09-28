#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

int main(void)
{
    char student_id[50];
    pid_t pid;
    int status;

    printf("=============================================\n");
    printf("   STUDENT LOOKUP SYSTEM - MANAGER\n");
    printf("   (fork + execve | file: students.txt)\n");
    printf("=============================================\n");
    printf("[MANAGER] PID: %d\n", getpid());
    printf("Enter student ID ('quit' to exit).\n");

    while (1) {
        printf("\n---------------------------------------------\n");
        printf("Student ID: ");

        if (scanf("%49s", student_id) != 1) {
            break;
        }

        /* User wants to stop the program */
        if (strcmp(student_id, "quit") == 0) {
            printf("[MANAGER] Exiting. Goodbye!\n");
            break;
        }

        /* Create a child process */
        pid = fork();

        if (pid < 0) {
            perror("fork failed");
            continue;
        }

        if (pid == 0) {
            /* Child process */

            char *args[] = {
                "./searcher",
                student_id,
                "students.txt",
                NULL
            };

            execve("./searcher", args, environ);

            /* Only reached if execve() fails */
            perror("execve failed");
            exit(2);
        }

        /* Parent process */
        printf("\n[MANAGER] fork() -> child PID: %d\n", pid);
        printf("[MANAGER] Waiting for child (waitpid)...\n\n");

        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            continue;
        }

        /* Check how the child process terminated */
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);

            if (exit_code == 0) {
                printf("\n[MANAGER] Child (PID %d) exited. "
                       "code=0 -> Found\n", pid);
            }
            else if (exit_code == 1) {
                printf("\n[MANAGER] Child (PID %d) exited. "
                       "code=1 -> Not found\n", pid);
            }
            else if (exit_code == 2) {
                printf("\n[MANAGER] Child (PID %d) exited. "
                       "code=2 -> Error\n", pid);
            }
        }
        else {
            printf("[MANAGER] Child did not exit normally.\n");
        }
    }

    return 0;
}