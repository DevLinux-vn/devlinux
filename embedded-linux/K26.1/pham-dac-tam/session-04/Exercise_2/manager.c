#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char student_id[20];
    pid_t pid;
    int status;
    
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
        
        if (fgets(student_id, sizeof(student_id), stdin) == NULL) {
            break;
        }
        
        // Remove newline
        student_id[strcspn(student_id, "\n")] = '\0';
        
        if (strcmp(student_id, "quit") == 0) {
            printf("[MANAGER] Exiting. Goodbye!\n");
            break;
        }
        
        // Fork
        fflush(stdout);
        pid = fork();
        
        if (pid == 0) {
            // Child process - execute searcher
            char *args[] = {"./searcher", student_id, "students.txt", NULL};
            execve("./searcher", args, NULL);
            /* Only reached if execve() FAILS */
            perror("execve failed");
            exit(2);
        } else if (pid > 0) {
            // Parent process
            printf("\n[MANAGER] fork() → child PID: %d\n", pid);
            printf("[MANAGER] Waiting for child (waitpid)...\n\n");
            
            waitpid(pid, &status, 0);
            
            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                if (exit_code == 0) {
                    printf("\n[MANAGER] Child (PID %d) exited. code=%d → Found\n", pid, exit_code);
                } else if (exit_code == 1) {
                    printf("\n[MANAGER] Child (PID %d) exited. code=%d → Not found\n", pid, exit_code);
                } else {
                    printf("\n[MANAGER] Child (PID %d) exited. code=%d → Error\n", pid, exit_code);
                }
            }
        } else {
            perror("fork failed");
            exit(1);
        }
    }
    
    return 0;
}
