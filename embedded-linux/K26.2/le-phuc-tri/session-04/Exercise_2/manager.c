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

    printf("=============================================\n");
    printf("   STUDENT LOOKUP SYSTEM — MANAGER\n");
    printf("   (fork + execve | file: students.txt)\n");
    printf("=============================================\n");

    printf("[MANAGER] PID: %d\n", getpid());
    printf("Enter student ID ('quit' to exit).\n");

    while (1) {
        printf("\n---------------------------------------------\n");
        printf("Student ID: ");

        /*
         * Read student ID from stdin.
         */
        if (fgets(student_id, sizeof(student_id), stdin) == NULL) {
            break;
        }

        /*
         * Remove newline '\n' from fgets().
         */
        student_id[strcspn(student_id, "\n")] = '\0';

        /*
         * User wants to exit.
         */
        if (strcmp(student_id, "quit") == 0) {
            printf("[MANAGER] Exiting. Goodbye!\n");
            break;
        }

        /*
         * Ignore empty input.
         */
        if (strlen(student_id) == 0) {
            continue;
        }

        /*
         * Create a child process.
         */
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            continue;
        }

        /*
         * Child process.
         */
        if (pid == 0) {
            char *args[] = {
                "./searcher",
                student_id,
                "students.txt",
                NULL
            };

            /*
             * Replace child process with searcher program.
             */
            execve("./searcher", args, environ);

            /*
             * This line is normally never reached because
             * a successful execve() replaces the current program.
             * It is reached only when execve() fails.
             */
            perror("execve failed");
            exit(2);
        }

        /*
         * Parent process.
         */
        printf("[MANAGER] fork() → child PID: %d\n", pid);
        printf("[MANAGER] Waiting for child (waitpid)...\n");

        int status;

        /*
         * Wait for this specific child.
         */
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            continue;
        }

        /*
         * Check how the child terminated.
         */
        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);

            if (code == 0) {
                printf("[MANAGER] Child (PID %d) exited. "
                       "code=0 → Found\n", pid);
            }
            else if (code == 1) {
                printf("[MANAGER] Child (PID %d) exited. "
                       "code=1 → Not found\n", pid);
            }
            else if (code == 2) {
                printf("[MANAGER] Child (PID %d) exited. "
                       "code=2 → Error\n", pid);
            }
            else {
                printf("[MANAGER] Child (PID %d) exited. "
                       "code=%d → Unknown\n", pid, code);
            }
        }
        else {
            printf("[MANAGER] Child (PID %d) did not exit normally.\n",
                   pid);
        }
    }

    return 0;
}
