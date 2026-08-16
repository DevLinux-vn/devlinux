#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;   /* can de truyen moi truong sang execve() */

int main(void)
{
    char input[64];

    printf("=============================================\n");
    printf("   STUDENT LOOKUP SYSTEM — MANAGER\n");
    printf("   (fork + execve | file: students.txt)\n");
    printf("=============================================\n");
    printf("[MANAGER] PID: %d\n", getpid());
    printf("Enter student ID ('quit' to exit).\n");
    printf("---------------------------------------------\n");
    fflush(stdout);

    while (1) {
        printf("Student ID: ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;   /* EOF (vd Ctrl+D) */
        }
        input[strcspn(input, "\r\n")] = '\0';

        if (strcmp(input, "quit") == 0) {
            printf("[MANAGER] Exiting. Goodbye!\n");
            break;
        }
        if (strlen(input) == 0) {
            continue;   /* bo qua dong trong */
        }

        fflush(stdout);   /* tranh con thua ke buffer chua flush cua cha */
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            /* ---- Process con: nap chuong trinh searcher de thay the chinh no ---- */
            char *argv_exec[] = { "./searcher", input, "students.txt", NULL };
            execve("./searcher", argv_exec, environ);

            /* Dong nay BINH THUONG khong bao gio chay toi: neu execve() thanh cong,
               hinh anh bo nho cua process da bi thay the hoan toan boi chuong trinh
               searcher, code phia sau execve() khong con ton tai de ma chay tiep.
               Chi khi execve() THAT BAI (vd file khong ton tai, khong co quyen thuc
               thi) no moi tra ve -1 va code duoi day duoc thuc thi. */
            perror("execve failed");
            exit(2);
        }

        /* ---- Process cha: doi con xu ly xong ---- */
        printf("[MANAGER] fork() → child PID: %d\n", pid);
        printf("[MANAGER] Waiting for child (waitpid)...\n");
        fflush(stdout);

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);
            const char *result;
            switch (code) {
                case 0: result = "Found";       break;
                case 1: result = "Not found";   break;
                case 2: result = "Error";       break;
                default: result = "Unknown";
            }
            printf("[MANAGER] Child (PID %d) exited. code=%d → %s\n",
                   pid, code, result);
        } else {
            printf("[MANAGER] Child (PID %d) terminated abnormally.\n", pid);
        }

        printf("---------------------------------------------\n");
        fflush(stdout);
    }

    return 0;
}
