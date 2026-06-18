#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // 1. Kiểm tra xem tiến trình cha có truyền đủ 2 tham số (Mã SV và Tên file) không
    if (argc < 3) {
        fprintf(stderr, "[SEARCHER] Error: Missing arguments!\n");
        exit(2); // Lỗi tham số -> Thoát với code 2
    }

    char *search_id = argv[1];   // Mã số sinh viên cần tìm
    char *file_path = argv[2];   // Đường dẫn đến file students.txt

    // In ra PID và PPID để chứng minh nó là con của thằng manager
    printf("[SEARCHER] PID: %d | PPID: %d\n", getpid(), getppid());
    printf("[SEARCHER] Searching for \"%s\" in %s...\n", search_id, file_path);

    // 2. Mở file dữ liệu students.txt
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("[SEARCHER] Error opening file");
        exit(2); // Lỗi mở file -> Thoát with code 2
    }

    char line[256];
    int found = 0;

    // 3. Đọc file theo từng dòng cho đến hết
    while (fgets(line, sizeof(line), file)) {
        // Loại bỏ ký tự xuống dòng '\n' ở cuối nếu có
        line[strcspn(line, "\n")] = '\0';

        // Tạo một bản sao của dòng để cắt chuỗi (vì strtok sẽ làm thay đổi chuỗi gốc)
        char line_copy[256];
        strcpy(line_copy, line);

        // Cắt trường dữ liệu đầu tiên: Mã số sinh viên
        char *id = strtok(line_copy, "|");
        
        // Nếu mã SV trùng khớp với mã cần tìm (search_id)
        if (id != NULL && strcmp(id, search_id) == 0) {
            // Cắt tiếp các trường còn lại để lấy thông tin
            char *name = strtok(NULL, "|");
            char *class_name = strtok(NULL, "|");
            char *gpa_str = strtok(NULL, "|");
            
            float gpa = atof(gpa_str); // Chuyển chuỗi điểm sang số thực float
            char grade[20];

            // Xếp loại học lực theo quy ước của đề bài
            if (gpa >= 8.5)      strcpy(grade, "Excellent");
            else if (gpa >= 7.0) strcpy(grade, "Good");
            else if (gpa >= 5.0) strcpy(grade, "Average");
            else                 strcpy(grade, "Poor");

            // In bảng kết quả chuẩn chỉnh theo Output mẫu
            printf("\n========== SEARCH RESULT ==========\n");
            printf("  ID      : %s\n", id);
            printf("  Name    : %s\n", name);
            printf("  Class   : %s\n", class_name);
            printf("  GPA     : %.1f\n", gpa);
            printf("  Grade   : %s\n", grade);
            printf("====================================\n\n");

            found = 1;
            break; // Tìm thấy rồi thì dừng vòng lặp luôn
        }
    }

    fclose(file); // Đóng file lại cho an toàn

    // 4. Trả về mã thoát theo đúng giao kèo (Contract) của đề bài
    if (found) {
        exit(0); // Tìm thấy -> Exit code = 0
    } else {
        printf("[SEARCHER] No student found with ID: %s\n\n", search_id);
        exit(1); // Không tìm thấy -> Exit code = 1
    }
}