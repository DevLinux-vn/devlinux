#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

// Khai báo mảng biến môi trường (bắt buộc phải có để truyền vào hàm execve)
extern char **environ;

int main() {
    char student_id[100];

    printf("\n=============================================\n");
    printf("    HỆ THỐNG TRA CỨU SINH VIÊN — MANAGER\n");
    printf("    (fork + execve | file: students.txt)\n");
    printf("=============================================\n");
    printf("[MANAGER] PID: %d\n", getpid());
    printf("Enter student ID ('quit' to exit).\n");

    // Vòng lặp liên tục đọc dữ liệu từ bàn phím
    while (1) {
        printf("---------------------------------------------\n");
        printf("Student ID: ");
        fflush(stdout);

        // Đọc một dòng chữ từ bàn phím (stdin)
        if (fgets(student_id, sizeof(student_id), stdin) == NULL) {
            break; 
        }

        // Xóa ký tự xuống dòng '\n' khi nhấn Enter
        student_id[strcspn(student_id, "\n")] = '\0';

        // Nếu người dùng nhập "quit" -> Thoát vòng lặp, tắt chương trình
        if (strcmp(student_id, "quit") == 0) {
            printf("[MANAGER] Exiting. Goodbye!\n");
            break;
        }

        // Bỏ qua nếu người dùng nhấn Enter trống mà không nhập gì
        if (strlen(student_id) == 0) {
            continue;
        }

        // Xả bộ đệm trước khi fork()
        fflush(stdout);

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            exit(1);
        }
        else if (pid == 0) {
            // === KHỐI CODE CỦA TIẾN TRÌNH CON ===
            // Chuẩn bị mảng các tham số để truyền sang cho chương trình searcher
            // argv[0]: tên chương trình, argv[1]: mã SV, argv[2]: tên file dữ liệu, argv[3]: phải là NULL
            char *args[] = {"./searcher", student_id, "students.txt", NULL};

            // Tiến hành lột xác! Thằng con biến thành chương trình độc lập ./searcher
            execve(args[0], args, environ);

            // 🛑 LƯU Ý THEO YÊU CẦU ĐỀ BÀI:
            // Dòng lệnh perror và exit dưới đây bình thường sẽ KHÔNG BAO GIỜ chạy tới được.
            // Lý do: Nếu hàm execve() thực hiện thành công, nó sẽ xóa sạch toàn bộ mã nguồn hiện tại 
            // của tiến trình con để thay bằng ruột của file './searcher'. Do đó, các dòng lệnh phía dưới 
            // bị biến mất hoàn toàn. Nó chỉ chạy xuống đây khi và chỉ khi hàm execve() bị LỖI 
            // (ví dụ: không tìm thấy file thực thi './searcher' hoặc không có quyền chạy).
            perror("execve failed");
            exit(2); 
        }
        else {
            // === KHỐI CODE CỦA TIẾN TRÌNH CHA ===
            printf("[MANAGER] fork() → child PID: %d\n", pid);
            printf("[MANAGER] Waiting for child (waitpid)...\n\n");

            int status;
            // Ép thằng cha đứng im đóng băng, đợi đích danh thằng con tìm kiếm xong và chết
            waitpid(pid, &status, 0);

            // Đọc mã thoát của con sau khi nó chết để đưa ra lời kết luận
            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                
                // Dựa vào "giao kèo" mã thoát của đề bài để in thông báo tương ứng
                if (exit_code == 0) {
                    printf("[MANAGER] Child (PID %d) exited. code=0 → Found\n", pid);
                } else if (exit_code == 1) {
                    printf("[MANAGER] Child (PID %d) exited. code=1 → Not found\n", pid);
                } else {
                    printf("[MANAGER] Child (PID %d) exited. code=%d → Error occurred in searcher\n", pid, exit_code);
                }
            } else {
                printf("[MANAGER] Child (PID %d) terminated abnormally\n", pid);
            }
        }
    }

    return 0;
}