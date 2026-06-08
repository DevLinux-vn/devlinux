#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>    // Định nghĩa các cờ O_RDWR, O_CREAT, ...
#include <unistd.h>   // Định nghĩa open, read, write, close

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

void print_menu() {
    printf("\n--- STUDENT MANAGEMENT ---\n");
    printf("1. Add student\n");
    printf("2. List all students\n");
    printf("3. Find student by ID\n");
    printf("4. Exit\n");
    printf("Choose an option: ");
}

int main() {
    int fd;
    int choice;
    Student s;

    // Mở file nhị phân ở chế độ Đọc/Ghi (O_RDWR).
    // Nếu chưa có file, tự tạo mới (O_CREAT) với quyền rw-r--r-- (0644).
    fd = open("students.dat", O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("Không thể mở file students.dat");
        exit(EXIT_FAILURE);
    }

    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            printf("Lựa chọn không hợp lệ!\n");
            // Xóa bộ đệm stdin để tránh vòng lặp vô hạn nếu user nhập chữ
            while (getchar() != '\n'); 
            continue;
        }

        if (choice == 1) {
            // --- CHỨC NĂNG 1: ADD STUDENT ---
            printf("Nhập ID: ");
            scanf("%d", &s.id);
            printf("Nhập tên (tối đa 63 ký tự): ");
            scanf(" %63[^\n]", s.name); // Đọc chuỗi có khoảng trắng
            printf("Nhập tuổi: ");
            scanf("%d", &s.age);
            printf("Nhập GPA: ");
            scanf("%f", &s.gpa);

            // Nguyên lý APPEND: Đẩy con trỏ file về cuối cùng trước khi ghi
            if (lseek(fd, 0, SEEK_END) < 0) {
                perror("lseek lỗi");
                continue;
            }

            // Ghi nguyên khối struct (kích thước cố định) vào file
            ssize_t bytes_written = write(fd, &s, sizeof(Student));
            if (bytes_written != sizeof(Student)) {
                perror("Ghi file thất bại");
            } else {
                printf("Đã thêm sinh viên thành công!\n");
            }

        } else if (choice == 2) {
            // --- CHỨC NĂNG 2: LIST ALL ---
            // Đưa con trỏ file về đầu để đọc từ đầu file
            if (lseek(fd, 0, SEEK_SET) < 0) {
                perror("lseek lỗi");
                continue;
            }

            printf("\nDanh sách sinh viên:\n");
            printf("%-5s | %-20s | %-5s | %-5s\n", "ID", "Tên sinh viên", "Tuổi", "GPA");
            printf("--------------------------------------------------\n");

            // Đọc tuần tự từng khối sizeof(Student) cho đến hết file (EOF)
            while (read(fd, &s, sizeof(Student)) == sizeof(Student)) {
                printf("%-5d | %-20s | %-5d | %-5.2f\n", s.id, s.name, s.age, s.gpa);
            }

        } else if (choice == 3) {
            // --- CHỨC NĂNG 3: FIND BY ID ---
            int search_id;
            int found = 0;
            printf("Nhập ID sinh viên cần tìm: ");
            scanf("%d", &search_id);

            // Quay con trỏ file về đầu để quét (scan) từ đầu file
            if (lseek(fd, 0, SEEK_SET) < 0) {
                perror("lseek lỗi");
                continue;
            }

            while (read(fd, &s, sizeof(Student)) == sizeof(Student)) {
                if (s.id == search_id) {
                    printf("\nĐã tìm thấy sinh viên:\n");
                    printf("ID: %d\nTên: %s\nTuổi: %d\nGPA: %.2f\n", s.id, s.name, s.age, s.gpa);
                    found = 1;
                    break; // Tìm thấy thì dừng vòng lặp quét luôn
                }
            }
            if (!found) {
                printf("Không tìm thấy sinh viên có ID = %d\n", search_id);
            }

        } else if (choice == 4) {
            // --- CHỨC NĂNG 4: EXIT ---
            printf("Đang đóng file và thoát chương trình...\n");
            break;
        } else {
            printf("Lựa chọn không hợp lệ, vui lòng chọn lại.\n");
        }
    }

    close(fd); // Đóng file descriptor giải phóng tài nguyên
    return 0;
}
