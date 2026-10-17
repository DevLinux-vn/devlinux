#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FILE_NAME "students.dat"

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

static void add_student(int fd) {
    Student s;
    memset(&s, 0, sizeof(Student));

    printf("Enter ID: ");
    if (scanf("%d", &s.id) != 1) {
        printf("Invalid ID\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n'); /* Xóa bộ đệm dòng mới */

    printf("Enter Name: ");
    if (fgets(s.name, sizeof(s.name), stdin) != NULL) {
        s.name[strcspn(s.name, "\r\n")] = '\0';
    }

    printf("Enter Age: ");
    if (scanf("%d", &s.age) != 1) {
        printf("Invalid Age\n");
        while (getchar() != '\n');
        return;
    }

    printf("Enter GPA: ");
    if (scanf("%f", &s.gpa) != 1) {
        printf("Invalid GPA\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    if (lseek(fd, 0, SEEK_END) == (off_t)-1) {
        perror("lseek failed");
        return;
    }

    ssize_t bytes_written = write(fd, &s, sizeof(Student));
    if (bytes_written != (ssize_t)sizeof(Student)) {
        perror("write failed");
    } else {
        printf("Student added successfully!\n");
    }
}

static void list_students(int fd) {
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("lseek failed");
        return;
    }

    Student s;
    ssize_t bytes_read;
    int count = 0;

    printf("\n--- Student List ---\n");
    while ((bytes_read = read(fd, &s, sizeof(Student))) == (ssize_t)sizeof(Student)) {
        printf("ID: %-5d | Name: %-20s | Age: %-3d | GPA: %.2f\n",
               s.id, s.name, s.age, s.gpa);
        count++;
    }

    if (bytes_read == -1) {
        perror("read failed");
    } else if (count == 0) {
        printf("No student records found.\n");
    }
    printf("--------------------\n");
}

static void find_student_by_id(int fd) {
    int target_id;
    printf("Enter Student ID to find: ");
    if (scanf("%d", &target_id) != 1) {
        printf("Invalid ID\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("lseek failed");
        return;
    }

    Student s;
    ssize_t bytes_read;
    int found = 0;

    while ((bytes_read = read(fd, &s, sizeof(Student))) == (ssize_t)sizeof(Student)) {
        if (s.id == target_id) {
            printf("\nStudent Found:\n");
            printf("ID:   %d\n", s.id);
            printf("Name: %s\n", s.name);
            printf("Age:  %d\n", s.age);
            printf("GPA:  %.2f\n", s.gpa);
            found = 1;
            break;
        }
    }

    if (bytes_read == -1) {
        perror("read failed");
    } else if (!found) {
        printf("Student with ID %d not found.\n", target_id);
    }
}

int main(void) {
    int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("open failed");
        return 1;
    }

    int choice = 0;
    while (1) {
        printf("\n=== Student Management ===\n");
        printf("1. Add student\n");
        printf("2. List all students\n");
        printf("3. Find student by ID\n");
        printf("4. Exit\n");
        printf("Select an option (1-4): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid choice. Try again.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                add_student(fd);
                break;
            case 2:
                list_students(fd);
                break;
            case 3:
                find_student_by_id(fd);
                break;
            case 4:
                /* Đóng file descriptor và thoát */
                close(fd);
                printf("Exiting program...\n");
                return 0;
            default:
                printf("Invalid option! Please choose between 1 and 4.\n");
                break;
        }
    }

    close(fd);
    return 0;
}
