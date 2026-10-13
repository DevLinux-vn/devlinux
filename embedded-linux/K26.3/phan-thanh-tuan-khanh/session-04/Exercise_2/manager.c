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

    while (1) {
        printf("\n---------------------------------------------\n");
        printf("Student ID: ");

        if (fgets(student_id, sizeof(student_id), stdin) == NULL) {
            break;
        }

        student_id[strcspn(student_id, "\n")] = '\0';

        if (strcmp(student_id, "quit") == 0) {
            printf("[MANAGER] Exiting. Goodbye!\n");
            break;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            char *args[] = {
                "./searcher",
                student_id,
                "students.txt",
                NULL
            };

            execve("./searcher", args, environ);

            /*
             * Only reached if execve() fails.
             * On success, execve() replaces the current process image
             * and never returns.
             */
            perror("execve failed");
            exit(2);
        }

        printf("\n[MANAGER] fork() -> child PID: %d\n", pid);
        printf("[MANAGER] Waiting for child (waitpid)...\n\n");

        int status;

        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            continue;
        }

        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);

            switch (code) {
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
                       pid, code);
                break;
            }
        }
    }

    return 0;
}