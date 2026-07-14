/* Copyright (C) 2025 XYZ Corp. All rights reserved. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

void print_menu() {
    printf("\n=== Student Management System ===\n");
    printf("1. Add student\n");
    printf("2. List all students\n");
    printf("3. Find student by ID\n");
    printf("4. Exit\n");
    printf("Choose an option: ");
}

void add_student(int fd) {
    Student s;
    printf("\nEnter student ID: ");
    scanf("%d", &s.id);
    printf("Enter student name: ");
    scanf("%63s", s.name);
    printf("Enter student age: ");
    scanf("%d", &s.age);
    printf("Enter student GPA: ");
    scanf("%f", &s.gpa);

    if (write(fd, &s, sizeof(Student)) == sizeof(Student)) {
        printf("Student added successfully!\n");
    } else {
        printf("Error writing to file!\n");
    }
}

void list_all_students(int fd) {
    Student s;
    ssize_t bytes_read;
    int count = 0;

    lseek(fd, 0, SEEK_SET);

    printf("\n=== All Students ===\n");
    while ((bytes_read = read(fd, &s, sizeof(Student))) == sizeof(Student)) {
        count++;
        printf("\nStudent %d:\n", count);
        printf("  ID: %d\n", s.id);
        printf("  Name: %s\n", s.name);
        printf("  Age: %d\n", s.age);
        printf("  GPA: %.2f\n", s.gpa);
    }

    if (count == 0) {
        printf("No students found!\n");
    }
}

void find_student(int fd) {
    int search_id;
    Student s;
    ssize_t bytes_read;
    int found = 0;

    printf("\nEnter student ID to search: ");
    scanf("%d", &search_id);

    lseek(fd, 0, SEEK_SET);

    while ((bytes_read = read(fd, &s, sizeof(Student))) == sizeof(Student)) {
        if (s.id == search_id) {
            printf("\n=== Student Found ===\n");
            printf("ID: %d\n", s.id);
            printf("Name: %s\n", s.name);
            printf("Age: %d\n", s.age);
            printf("GPA: %.2f\n", s.gpa);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Student with ID %d not found!\n", search_id);
    }
}

int main() {
    int fd = open("students.dat", O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    int choice;
    while (1) {
        print_menu();
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
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Invalid option! Please try again.\n");
        }
    }

    return 0;
}
