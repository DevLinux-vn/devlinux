#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

int main(void) {
    char input[100];

    printf("=============================================\n");
    printf("   STUDENT LOOKUP SYSTEM — MANAGER\n");
    printf("   (fork + execve | file: students.txt)\n");
    printf("=============================================\n");
    printf("[MANAGER] PID: %d\n", getpid());
    printf("Enter student ID ('quit' to exit).\n\n");

    while (1) {
        printf("---------------------------------------------\n");
        printf("Student ID: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove trailing newline character
        input[strcspn(input, "\r\n")] = '\0';

        // Check for exit condition
        if (strcmp(input, "quit") == 0) {
            printf("[MANAGER] Exiting. Goodbye!\n");
            break;
        }

        // Skip empty inputs
        if (strlen(input) == 0) {
            continue;
        }

        printf("\n[MANAGER] fork() → ");
        fflush(stdout); // Flush buffer before fork

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            continue;
        }

        if (pid == 0) {
            // Child process: execute searcher binary
            char *args[] = {"./searcher", input, "students.txt", NULL};
            execve("./searcher", args, environ);

            /* 
             * This line is normally NEVER reached because execve() replaces 
             * the child process's text, data, heap, and stack segments with 
             * the new binary image upon success. If control reaches here, 
             * it indicates that execve() failed (e.g., binary not found or permissions denied).
             */
            perror("[MANAGER] execve failed");
            exit(2);

        } else {
            // Parent process
            printf("child PID: %d\n", pid);
            printf("[MANAGER] Waiting for child (waitpid)...\n\n");

            int status;
            if (waitpid(pid, &status, 0) > 0) {
                if (WIFEXITED(status)) {
                    int exit_code = WEXITSTATUS(status);
                    printf("\n[MANAGER] Child (PID %d) exited. code=%d → ", pid, exit_code);
                    switch (exit_code) {
                        case 0:
                            printf("Found\n\n");
                            break;
                        case 1:
                            printf("Not found\n\n");
                            break;
                        case 2:
                        default:
                            printf("Error (file or argument error)\n\n");
                            break;
                    }
                } else {
                    printf("\n[MANAGER] Child (PID %d) terminated abnormally\n\n", pid);
                }
            } else {
                perror("[MANAGER] waitpid failed");
            }
        }
    }

    return 0;
}


