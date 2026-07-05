#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define INPUT_BUF_SIZE 100

char *data_path = "students.txt";
char *envp[] = {"PATH=/bin:/usr/bin", NULL};

int main(void)
{
    char input[INPUT_BUF_SIZE];

    printf("=============================================\n");
    printf("   STUDENT LOOKUP SYSTEM — MANAGER\n");
    printf("   (fork + execve | file: students.txt)\n");
    printf("=============================================\n");
    printf("[MANAGER] PID: %d\n", getpid());
    printf("Enter student ID ('quit' to exit).\n");

    while (1) {
        printf("\n---------------------------------------------\n");
        printf("Student ID: ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "quit") == 0) {
            printf("[MANAGER] Exiting. Goodbye!\n");
            break;
        }

        fflush(stdout);
        pid_t pid = fork();
        if (pid == -1) {
            perror("[MANAGER] Error when create child process \n");
        } else if (pid == 0) {
            char *args[] = {"./searcher", input, data_path, NULL};
            execve("./searcher", args, envp);
            // Chỉ tới được đây nếu execve() thất bại — execve() thành công
            // sẽ thay thế toàn bộ process image nên không bao giờ return.
            perror("[MANAGER] Child error when run execve \n");
            exit(2);
        } else {
            printf("\n[MANAGER] fork() → child PID: %d\n", pid);
            printf("[MANAGER] Waiting for child (waitpid)...\n\n");
            fflush(stdout);

            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                int code = WEXITSTATUS(status);
                const char *result = (code == 0) ? "Found"
                                    : (code == 1) ? "Not found"
                                                  : "Error";
                printf("\n[MANAGER] Child (PID %d) exited. code=%d → %s\n", pid, code, result);
            } else if (WIFSIGNALED(status)) {
                printf("\n[MANAGER] Child (PID %d) killed by signal %d\n", pid, WTERMSIG(status));
            }
        }
    }

    return 0;
}