    #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

#define ID_SIZE 32
#define DATA_FILE "students.txt"

int main(void) {
    char student_id[ID_SIZE];

    printf("\n=============================================\n");
    printf("   STUDENT LOOKUP SYSTEM — MANAGER\n");
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
                DATA_FILE,
                NULL
            };

            execve("./searcher", args, environ);

            /*
             * This line is normally never reached because execve()
             * replaces the current child process image with ./searcher.
             * If execve() returns, it means execve() failed.
             */
            perror("execve failed");
            exit(2);
        }

        printf("\n[MANAGER] fork() → child PID: %d\n", pid);
        printf("[MANAGER] Waiting for child (waitpid)...\n\n");

        int status;
        pid_t waited_pid = waitpid(pid, &status, 0);

        if (waited_pid < 0) {
            perror("waitpid");
            continue;
        }

        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);

            if (code == 0) {
                printf("\n[MANAGER] Child (PID %d) exited. code=%d → Found\n",
                       waited_pid, code);
            } else if (code == 1) {
                printf("\n[MANAGER] Child (PID %d) exited. code=%d → Not found\n",
                       waited_pid, code);
            } else if (code == 2) {
                printf("\n[MANAGER] Child (PID %d) exited. code=%d → File or argument error\n",
                       waited_pid, code);
            } else {
                printf("\n[MANAGER] Child (PID %d) exited. code=%d → Unknown result\n",
                       waited_pid, code);
            }
        } else {
            printf("\n[MANAGER] Child (PID %d) did not exit normally.\n", waited_pid);
        }
    }

    return 0;
}