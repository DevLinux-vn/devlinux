#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_INPUT_LEN 256

int main(void) {
    char input[MAX_INPUT_LEN];

    printf("=============================================\n");
    printf("   STUDENT LOOKUP SYSTEM — MANAGER\n");
    printf("   (fork + execve | file: students.txt)\n");
    printf("=============================================\n");
    printf("[MANAGER] PID: %d\n", getpid());
    printf("Enter student ID ('quit' to exit).\n\n");

    while (1) {
        printf("---------------------------------------------\n");
        printf("Student ID: ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            if (feof(stdin)) {
                printf("\n[MANAGER] EOF detected. Exiting.\n");
                break;
            }
            perror("fgets error");
            continue;
        }

        // Remove trailing newline
        size_t len = strcspn(input, "\n");
        input[len] = '\0';
        
        // Handle input that is too long, which leaves chars in stdin buffer
        if (len == MAX_INPUT_LEN - 1 && input[len-1] != '\n') {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }

        if (strcmp(input, "quit") == 0) {
            printf("[MANAGER] Exiting. Goodbye!\n");
            break;
        }

        if (strlen(input) == 0) {
            continue;
        }

        fflush(stdout); // prepare for fork
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            continue;
        } else if (pid == 0) {
            // Child process
            char *args[] = {"./searcher", input, "students.txt", NULL};
            extern char **environ;
            execve(args[0], args, environ);

            // This line is only reached if execve fails
            perror("execve failed");
            exit(2);
        } else {
            // Parent process
            printf("\n[MANAGER] fork() → child PID: %d\n", pid);
            printf("[MANAGER] Waiting for child (waitpid)...\n\n");

            int status;
            pid_t wpid = waitpid(pid, &status, 0);

            if (wpid == -1) {
                perror("waitpid failed");
                continue;
            }

            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                char *msg = "";
                if (exit_code == 0) msg = "Found";
                else if (exit_code == 1) msg = "Not found";
                else if (exit_code == 2) msg = "Error";
                else msg = "Unknown";
                
                printf("\n[MANAGER] Child (PID %d) exited. code=%d → %s\n\n", pid, exit_code, msg);
            }
        }
    }

    return 0;
}
