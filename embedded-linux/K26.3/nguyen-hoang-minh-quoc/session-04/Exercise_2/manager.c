#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

#define DATA_FILE "students.txt"
#define SEARCHER_PATH "./searcher"

static void strip_newline(char *s)
{
    size_t len = strlen(s);

    if (len > 0 && s[len - 1] == '\n') {
        s[len - 1] = '\0';
    }
}

static void print_result(pid_t pid, int code)
{
    if (code == 0) {
        printf("[MANAGER] Child (PID %d) exited. code=0 -> Found\n", pid);
    } else if (code == 1) {
        printf("[MANAGER] Child (PID %d) exited. code=1 -> Not found\n", pid);
    } else if (code == 2) {
        printf("[MANAGER] Child (PID %d) exited. code=2 -> File or argument error\n", pid);
    } else {
        printf("[MANAGER] Child (PID %d) exited. code=%d -> Unknown result\n",
               pid, code);
    }
}

int main(void)
{
    char student_id[64];

    printf("=============================================\n");
    printf("   STUDENT LOOKUP SYSTEM - MANAGER\n");
    printf("   (fork + execve | file: %s)\n", DATA_FILE);
    printf("=============================================\n");
    printf("[MANAGER] PID: %d\n", getpid());
    printf("Enter student ID ('quit' to exit).\n");

    for (;;) {
        pid_t pid;
        int status;

        printf("\n---------------------------------------------\n");
        printf("Student ID: ");
        fflush(stdout);

        if (fgets(student_id, sizeof(student_id), stdin) == NULL) {
            break;
        }
        strip_newline(student_id);

        if (strcmp(student_id, "quit") == 0) {
            printf("[MANAGER] Exiting. Goodbye!\n");
            break;
        }
        if (student_id[0] == '\0') {
            printf("[MANAGER] Empty ID, please try again.\n");
            continue;
        }

        fflush(stdout);
        pid = fork();
        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            char *args[] = {
                SEARCHER_PATH,
                student_id,
                DATA_FILE,
                NULL
            };

            execve(SEARCHER_PATH, args, environ);
            /* Reached only if execve() fails, because success replaces this process image. */
            perror("execve failed");
            exit(2);
        }

        printf("\n[MANAGER] fork() -> child PID: %d\n", pid);
        printf("[MANAGER] Waiting for child (waitpid)...\n\n");

        if (waitpid(pid, &status, 0) < 0) {
            perror("waitpid");
            continue;
        }

        if (WIFEXITED(status)) {
            print_result(pid, WEXITSTATUS(status));
        } else {
            printf("[MANAGER] Child (PID %d) did not exit normally.\n", pid);
        }
    }

    return 0;
}
