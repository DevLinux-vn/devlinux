#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ; /* Access system environment variables for execve */

int main(void) {
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
        fflush(stdout);

        /* Read student ID from standard input safely */
        if (fgets(student_id, sizeof(student_id), stdin) == NULL) {
            break;
        }

        /* Strip trailing newline character */
        student_id[strcspn(student_id, "\n")] = '\0';

        /* Exit condition */
        if (strcmp(student_id, "quit") == 0) {
            printf("[MANAGER] Exiting. Goodbye!\n");
            break;
        }

        /* Skip empty inputs */
        if (strlen(student_id) == 0) {
            continue;
        }

        printf("\n[MANAGER] fork() → child PID pending...\n");
        pid_t pid = fork();

        if (pid < 0) {
            perror("[MANAGER] fork failed");
            continue;
        } 
        else if (pid == 0) {
            /* Child Process Context */
            /* Construct argument vector for execve. 
               argv[0]: program name, argv[1]: ID, argv[2]: database path, argv[3]: NULL terminator */
            char *args[] = { "./searcher", student_id, "students.txt", NULL };

            execve(args[0], args, environ);

            /* ------------------------------------------------------------------
             * REASON WHY THE LINE BELOW IS NORMALLY NEVER REACHED:
             * On a successful execve() call, the text, data, bss, and stack segments
             * of the calling process are completely overwritten by the new program 
             * ("./searcher"). The execution continues at the entry point of the new 
             * binary. Therefore, control only returns to this point if execve fails 
             * (e.g., binary not found, incorrect permissions, execution denied).
             * ------------------------------------------------------------------ */
            perror("[MANAGER] execve failed");
            exit(2); /* Exit code 2 indicates structural or environment error */
        } 
        else {
            /* Parent Process Context */
            printf("[MANAGER] fork() → child PID: %d\n", pid);
            printf("[MANAGER] Waiting for child (waitpid)...\n\n");

            int status;
            if (waitpid(pid, &status, 0) < 0) {
                perror("[MANAGER] waitpid failed");
                continue;
            }

            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                printf("\n[MANAGER] Child (PID %d) exited. code=%d → ", pid, exit_code);
                
                switch (exit_code) {
                    case 0:
                        printf("Found\n");
                        break;
                    case 1:
                        printf("Not found\n");
                        break;
                    case 2:
                        printf("Error (File or argument issue occurred in child)\n");
                        break;
                    default:
                        printf("Unknown exit code\n");
                        break;
                }
            } else {
                printf("\n[MANAGER] Child (PID %d) terminated abnormally.\n", pid);
            }
        }
    }

    return EXIT_SUCCESS;
}