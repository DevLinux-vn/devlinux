#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct {
    int id;
    char name[64];
    int age;
    float gpa;
} Student;

#define FILE_NAME "students.dat"

void add_student() {
    // Mở file ở chế độ: Chỉ ghi, Tạo nếu chưa có, Append vào cuối file. Quyền ghi đọc: 0644
    int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("Error opening file for adding");
        return;
    }

    Student s;
    printf("Enter Student ID: ");
    if (scanf("%d", &s.id) != 1) return;
    
    getchar(); // Đọc bỏ ký tự thừa '\n' tránh trôi lệnh
    printf("Enter Name: ");
    if (fgets(s.name, sizeof(s.name), stdin) != NULL) {
        s.name[strcspn(s.name, "\n")] = '\0'; // Xóa dấu xuống dòng ở cuối
    }

    printf("Enter Age: ");
    if (scanf("%d", &s.age) != 1) return;

    printf("Enter GPA: ");
    if (scanf("%f", &s.gpa) != 1) return;

    // Ghi trực tiếp cả struct vào file bằng system call write
    if (write(fd, &s, sizeof(Student)) < 0) {
        perror("Error writing to file");
    } else {
        printf("Student added successfully.\n");
    }

    close(fd);
}

void list_students() {
    // Mở chế độ chỉ đọc
    int fd = open(FILE_NAME, O_RDONLY);
    if (fd < 0) {
        // Nếu file chưa tồn tại thì coi như danh sách rỗng
        printf("No student records found.\n");
        return;
    }

    Student s;
    int count = 0;
    printf("\n--- Student List ---\n");
    
    // Đọc tuần tự từng struct cho đến khi hết file (read trả về 0)
    while (read(fd, &s, sizeof(Student)) > 0) {
        printf("ID: %d | Name: %s | Age: %d | GPA: %.2f\n", s.id, s.name, s.age, s.gpa);
        count++;
    }

    if (count == 0) {
        printf("No student records found.\n");
    }
    close(fd);
}

void find_student() {
    int fd = open(FILE_NAME, O_RDONLY);
    if (fd < 0) {
        printf("No student records found.\n");
        return;
    }

    int search_id;
    printf("Enter Student ID to find: ");
    if (scanf("%d", &search_id) != 1) {
        close(fd);
        return;
    }

    Student s;
    int found = 0;

    while (read(fd, &s, sizeof(Student)) > 0) {
        if (s.id == search_id) {
            printf("\nStudent Found:\n");
            printf("ID: %d | Name: %s | Age: %d | GPA: %.2f\n", s.id, s.name, s.age, s.gpa);
            found = 1;
            break; // Tìm thấy bản ghi đầu tiên khớp ID thì dừng luôn theo đề bài
        }
    }

    if (!found) {
        printf("Student with ID %d not found.\n", search_id);
    }
    close(fd);
}

int main() {
    int choice;
    while (1) {
        printf("\n=== Student Management ===\n");
        printf("1. Add student\n");
        printf("2. List all students\n");
        printf("3. Find student by ID\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            getchar(); // Clear ký tự lỗi
            continue;
        }

        switch (choice) {
            case 1:
                add_student();
                break;
            case 2:
                list_students();
                break;
            case 3:
                find_student();
                break;
            case 4:
                printf("Exiting program.\n");
                return 0;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}