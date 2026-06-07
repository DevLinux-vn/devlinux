#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

int main(void)
{
    printf("=============================================\n");
    printf("   STUDENT LOOKUP SYSTEM — MANAGER\n");
    printf("   (fork + execve | file: students.txt)\n");
    printf("=============================================\n");
    printf("[MANAGER] PID: %d\n", getpid());
    printf("Enter student ID ('quit' to exit).\n");

    char id[64];
    while (1) {
        printf("\n---------------------------------------------\n");
        printf("Student ID: ");
        fflush(stdout);

        if (scanf("%63s", id) != 1)
            break;

        if (strcmp(id, "quit") == 0) {
            printf("[MANAGER] Exiting. Goodbye!\n");
            break;
        }

        fflush(stdout);
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) {
            /* Child: replace image with searcher */
            char *args[] = {"./searcher", id, "students.txt", NULL};
            execve("./searcher", args, environ);
            /* execve() only returns if it fails — process image was NOT replaced */
            perror("execve failed");
            exit(2);
        }

        /* Parent */
        printf("\n[MANAGER] fork() → child PID: %d\n", pid);
        printf("[MANAGER] Waiting for child (waitpid)...\n\n");

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);
            if (code == 0)
                printf("[MANAGER] Child (PID %d) exited. code=%d → Found\n", pid, code);
            else if (code == 1)
                printf("[MANAGER] Child (PID %d) exited. code=%d → Not found\n", pid, code);
            else
                printf("[MANAGER] Child (PID %d) exited. code=%d → Error\n", pid, code);
        } else if (WIFSIGNALED(status)) {
            printf("[MANAGER] Child (PID %d) killed by signal %d\n", pid, WTERMSIG(status));
        }
    }

    return 0;
}
