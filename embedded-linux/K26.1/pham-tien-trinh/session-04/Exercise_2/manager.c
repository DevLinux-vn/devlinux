#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

#define MAX_ID_LEN 32

int main(void)
{
    char student_id[MAX_ID_LEN];

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
            char *argv[] = {
                "./searcher",
                student_id,
                "students.txt",
                NULL
            };

            execve("./searcher", argv, environ);

            /* Only reached if execve() fails */
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

            if (code == 0) {
                printf("\n[MANAGER] Child (PID %d) exited. code=%d -> Found\n",
                       pid, code);
            } else if (code == 1) {
                printf("\n[MANAGER] Child (PID %d) exited. code=%d -> Not found\n",
                       pid, code);
            } else if (code == 2) {
                printf("\n[MANAGER] Child (PID %d) exited. code=%d -> Error\n",
                       pid, code);
            } else {
                printf("\n[MANAGER] Child (PID %d) exited. code=%d -> Unknown\n",
                       pid, code);
            }
        } else {
            printf("\n[MANAGER] Child (PID %d) did not exit normally\n", pid);
        }
    }

    return 0;
}