#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct {
    int id;
    char name[64];
    int age;
    float gpa;
} Student;

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void add_student(int fd) {
    Student s;

    printf("Enter ID: ");
    scanf("%d", &s.id);
    clear_input_buffer();

    printf("Enter name: ");
    fgets(s.name, sizeof(s.name), stdin);
    s.name[strcspn(s.name, "\n")] = '\0';

    printf("Enter age: ");
    scanf("%d", &s.age);

    printf("Enter GPA: ");
    scanf("%f", &s.gpa);

    lseek(fd, 0, SEEK_END);

    if (write(fd, &s, sizeof(Student)) != sizeof(Student)) {
        perror("write");
        return;
    }

    printf("Student added successfully.\n");
}

void list_students(int fd) {
    Student s;
    ssize_t bytes_read;
    int count = 0;

    lseek(fd, 0, SEEK_SET);

    while ((bytes_read = read(fd, &s, sizeof(Student))) == sizeof(Student)) {
        printf("\nStudent #%d\n", ++count);
        printf("ID  : %d\n", s.id);
        printf("Name: %s\n", s.name);
        printf("Age : %d\n", s.age);
        printf("GPA : %.2f\n", s.gpa);
    }

    if (bytes_read < 0) {
        perror("read");
    }

    if (count == 0) {
        printf("No students found.\n");
    }
}

void find_student(int fd) {
    int target_id;
    Student s;
    ssize_t bytes_read;

    printf("Enter ID to find: ");
    scanf("%d", &target_id);

    lseek(fd, 0, SEEK_SET);

    while ((bytes_read = read(fd, &s, sizeof(Student))) == sizeof(Student)) {
        if (s.id == target_id) {
            printf("\nStudent found\n");
            printf("ID  : %d\n", s.id);
            printf("Name: %s\n", s.name);
            printf("Age : %d\n", s.age);
            printf("GPA : %.2f\n", s.gpa);
            return;
        }
    }

    if (bytes_read < 0) {
        perror("read");
        return;
    }

    printf("Student with ID %d not found.\n", target_id);
}

int main(void) {
    int fd;
    int choice;

    fd = open("students.dat", O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        perror("open");
        return 1;
    }

    while (1) {
        printf("\n===== Student Manager =====\n");
        printf("1. Add student\n");
        printf("2. List all students\n");
        printf("3. Find student by ID\n");
        printf("4. Exit\n");
        printf("Choose: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            clear_input_buffer();
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
                find_student(fd);
                break;
            case 4:
                close(fd);
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
}