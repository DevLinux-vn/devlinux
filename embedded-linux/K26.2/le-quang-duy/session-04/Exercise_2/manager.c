#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

int main() {
    char input[64];
    
    printf("=============================================\n");
    printf("   STUDENT LOOKUP SYSTEM — MANAGER\n");
    printf("   (fork + execve | file: students.txt)\n");
    printf("=============================================\n");
    printf("[MANAGER] PID: %d\n", getpid());
    printf("Enter student ID ('quit' to exit).\n");

    while (1) {
        printf("\n---------------------------------------------\n");
        printf("Student ID: ");
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = 0; // Xoá ký tự \n ở cuối

        if (strcmp(input, "quit") == 0) {
            printf("[MANAGER] Exiting. Goodbye!\n");
            break;
        }
        if (strlen(input) == 0) continue;

        fflush(stdout); // Quan trọng trước khi fork
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            continue;
        } else if (pid == 0) {
            // Child
            char *args[] = {"./searcher", input, "students.txt", NULL};
            execve("./searcher", args, environ);
            
            /* Đoạn code này thông thường không bao giờ được chạm tới vì execve()
               sẽ thay thế toàn bộ không gian bộ nhớ của tiến trình hiện tại bằng
               chương trình "./searcher". Nếu lệnh perror ở dưới được chạy,
               điều đó có nghĩa là execve() đã THẤT BẠI (ví dụ: không tìm thấy file thực thi). */
            perror("execve failed");
            exit(2);
        } else {
            // Parent
            printf("\n[MANAGER] fork() -> child PID: %d\n", pid);
            printf("[MANAGER] Waiting for child (waitpid)...\n\n");
            
            int status;
            waitpid(pid, &status, 0);
            
            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                const char *msg = "Unknown";
                if (exit_code == 0) msg = "Found";
                else if (exit_code == 1) msg = "Not found";
                else if (exit_code == 2) msg = "Error";
                
                printf("\n[MANAGER] Child (PID %d) exited. code=%d -> %s\n", pid, exit_code, msg);
            }
        }
    }
    
    return 0;
}