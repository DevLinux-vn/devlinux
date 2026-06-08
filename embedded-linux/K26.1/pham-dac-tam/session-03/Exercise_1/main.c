#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

#define DATA_FILE "students.dat"

// Mở file (tạo nếu chưa tồn tại)
int open_data_file() {
    int fd = open(DATA_FILE, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    return fd;
}

// Thêm sinh viên vào file
void add_student(int fd) {
    Student student;
    
    printf("\n=== Add Student ===\n");
    printf("Enter ID: ");
    scanf("%d", &student.id);
    printf("Enter Name: ");
    scanf("%s", student.name);
    printf("Enter Age: ");
    scanf("%d", &student.age);
    printf("Enter GPA: ");
    scanf("%f", &student.gpa);
    
    // Di chuyển con trỏ đến cuối file
    if (lseek(fd, 0, SEEK_END) == -1) {
        perror("lseek");
        return;
    }
    
    // Ghi dữ liệu sinh viên (binary format)
    if (write(fd, &student, sizeof(Student)) == -1) {
        perror("write");
        return;
    }
    
    printf("✓ Student added successfully!\n");
}

// Hiển thị tất cả sinh viên
void list_all_students(int fd) {
    Student student;
    ssize_t bytes_read;
    int count = 0;
    
    printf("\n=== All Students ===\n");
    
    // Trở về đầu file
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }
    
    // Đọc từng record và in ra
    while ((bytes_read = read(fd, &student, sizeof(Student))) > 0) {
        if (bytes_read != sizeof(Student)) {
            fprintf(stderr, "Warning: Incomplete record read\n");
            break;
        }
        count++;
        printf("\nStudent #%d:\n", count);
        printf("  ID:   %d\n", student.id);
        printf("  Name: %s\n", student.name);
        printf("  Age:  %d\n", student.age);
        printf("  GPA:  %.2f\n", student.gpa);
    }
    
    if (count == 0) {
        printf("No students found.\n");
    }
}

// Tìm sinh viên theo ID
void find_student(int fd) {
    int target_id;
    Student student;
    ssize_t bytes_read;
    int found = 0;
    
    printf("\n=== Find Student ===\n");
    printf("Enter ID to search: ");
    scanf("%d", &target_id);
    
    // Trở về đầu file
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }
    
    // Quét từng record để tìm
    while ((bytes_read = read(fd, &student, sizeof(Student))) > 0) {
        if (bytes_read != sizeof(Student)) {
            fprintf(stderr, "Warning: Incomplete record read\n");
            break;
        }
        
        if (student.id == target_id) {
            printf("\n✓ Found!\n");
            printf("  ID:   %d\n", student.id);
            printf("  Name: %s\n", student.name);
            printf("  Age:  %d\n", student.age);
            printf("  GPA:  %.2f\n", student.gpa);
            found = 1;
            break;
        }
    }
    
    if (!found) {
        printf("✗ Student with ID %d not found.\n", target_id);
    }
}

void show_menu() {
    printf("\n========= STUDENT MANAGER =========\n");
    printf("1. Add student\n");
    printf("2. List all students\n");
    printf("3. Find student by ID\n");
    printf("4. Exit\n");
    printf("==================================\n");
    printf("Enter choice: ");
}

int main() {
    int fd = open_data_file();
    int choice;
    
    while (1) {
        show_menu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                add_student(fd);
                break;
            case 2:
                list_all_students(fd);
                break;
            case 3:
                find_student(fd);
                break;
            case 4:
                close(fd);
                printf("\n✓ Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    
    return 0;
}
