#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define DATA_FILE "students.dat"

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void add_student(int fd) {
    Student s;

    printf("Enter student ID: ");
    scanf("%d", &s.id);
    clear_input_buffer();

    printf("Enter student name: ");
    fgets(s.name, sizeof(s.name), stdin);

    s.name[strcspn(s.name, "\n")] = '\0';

    printf("Enter student age: ");
    scanf("%d", &s.age);

    printf("Enter student GPA: ");
    scanf("%f", &s.gpa);

    lseek(fd, 0, SEEK_END);

    ssize_t bytes_written = write(fd, &s, sizeof(Student));

    if (bytes_written != sizeof(Student)) {
        perror("write");
        return;
    }

    printf("Student added successfully.\n");
}

void list_students(int fd) {
    Student s;
    int index = 0;

    lseek(fd, 0, SEEK_SET);

    printf("\n===== Student List =====\n");

    while (read(fd, &s, sizeof(Student)) == sizeof(Student)) {
        printf("Record #%d\n", index);
        printf("ID   : %d\n", s.id);
        printf("Name : %s\n", s.name);
        printf("Age  : %d\n", s.age);
        printf("GPA  : %.2f\n", s.gpa);
        printf("------------------------\n");
        index++;
    }

    if (index == 0) {
        printf("No students found.\n");
    }
}

void find_student_by_id(int fd) {
    Student s;
    int target_id;
    int found = 0;

    printf("Enter student ID to find: ");
    scanf("%d", &target_id);

    lseek(fd, 0, SEEK_SET);

    while (read(fd, &s, sizeof(Student)) == sizeof(Student)) {
        if (s.id == target_id) {
            printf("\nStudent found:\n");
            printf("ID   : %d\n", s.id);
            printf("Name : %s\n", s.name);
            printf("Age  : %d\n", s.age);
            printf("GPA  : %.2f\n", s.gpa);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Student with ID %d not found.\n", target_id);
    }
}

int main() {
    int fd;
    int choice;

    fd = open(DATA_FILE, O_RDWR | O_CREAT, 0644);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    while (1) {
        printf("\n===== Student Management =====\n");
        printf("1. Add student\n");
        printf("2. List all students\n");
        printf("3. Find student by ID\n");
        printf("4. Exit\n");
        printf("Choose an option: ");

        scanf("%d", &choice);

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
                close(fd);
                printf("Program exited.\n");
                return 0;

            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }
}