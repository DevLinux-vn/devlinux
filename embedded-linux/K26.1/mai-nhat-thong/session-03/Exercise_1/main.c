#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FILE_NAME "students.dat"
#define MENU_EXIT_CHOICE 4

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

static void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void add_student(int fd) {
    Student s;
    memset(&s, 0, sizeof(Student));

    printf("Enter Student ID: ");
    if (scanf("%d", &s.id) != 1) {
        flush_stdin();
        return;
    }
    flush_stdin();

    printf("Enter Name: ");
    if (fgets(s.name, sizeof(s.name), stdin)) {
        s.name[strcspn(s.name, "\n")] = 0;
    }

    printf("Enter Age: ");
    if (scanf("%d", &s.age) != 1) {
        flush_stdin();
        return;
    }

    printf("Enter GPA: ");
    if (scanf("%f", &s.gpa) != 1) {
        flush_stdin();
        return;
    }
    flush_stdin();

    if (lseek(fd, 0, SEEK_END) == (off_t)-1) {
        perror("lseek to end failed");
        return;
    }

    if (write(fd, &s, sizeof(Student)) != (ssize_t)sizeof(Student)) {
        perror("Error writing student record");
    } else {
        printf("Student added successfully.\n");
    }
}

static void list_students(int fd) {
    Student s;
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("lseek to start failed");
        return;
    }

    printf("\n--- Student List ---\n");
    printf("%-5s %-20s %-5s %-5s\n", "ID", "Name", "Age", "GPA");
    
    while (1) {
        const ssize_t bytes_read = read(fd, &s, sizeof(Student));
        if (bytes_read < 0) {
            perror("Error reading file");
            break;
        }
        if (bytes_read == 0) {
            break; /* EOF */
        }
        if (bytes_read == (ssize_t)sizeof(Student)) {
            printf("%-5d %-20s %-5d %-5.2f\n", s.id, s.name, s.age, s.gpa);
        }
    }
}

static void find_student(int fd) {
    int search_id;
    Student s;
    int found = 0;

    printf("Enter Student ID to find: ");
    if (scanf("%d", &search_id) != 1) {
        flush_stdin();
        return;
    }
    flush_stdin();

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("lseek to start failed");
        return;
    }

    while (1) {
        const ssize_t bytes_read = read(fd, &s, sizeof(Student));
        if (bytes_read < 0) {
            perror("Error reading file");
            break;
        }
        if (bytes_read == 0) {
            break; /* EOF */
        }
        if (bytes_read == (ssize_t)sizeof(Student) && s.id == search_id) {
            printf("\nStudent Found:\n");
            printf("ID: %d\nName: %s\nAge: %d\nGPA: %.2f\n", s.id, s.name, s.age, s.gpa);
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Student with ID %d not found.\n", search_id);
    }
}

int main(void) {
    int fd = open(FILE_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    int choice;
    do {
        printf("\n=== Student Management Menu ===\n");
        printf("1. Add student\n");
        printf("2. List all students\n");
        printf("3. Find student by ID\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            flush_stdin();
            continue;
        }
        flush_stdin();

        switch (choice) {
            case 1: add_student(fd); break;
            case 2: list_students(fd); break;
            case 3: find_student(fd); break;
            case 4: printf("Exiting...\n"); break;
            default: printf("Invalid choice, try again.\n");
        }
    } while (choice != MENU_EXIT_CHOICE);

    if (close(fd) < 0) {
        perror("Error closing file descriptor");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}