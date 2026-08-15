#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define NAMELEN 64

typedef struct {
    int   id;
    char  name[NAMELEN];
    int   age;
    float gpa;
} Student;

void add_student(void);
void list_students(void);
void find_student(void);

int main(void)
{
    int choice;

    while (1)
    {
        printf("\n");
        printf("====== Student Management System ========\n");
        printf("1. Add student\n");
        printf("2. List all students\n");
        printf("3. Find student by ID\n");
        printf("4. Exit\n");
        printf("========================================\n");
        printf("Choose: ");

        scanf("%d", &choice);

        switch (choice)
        {
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
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }
}

void add_student(void)
{
    int fd;
    Student student;

    fd = open("students.dat", O_WRONLY | O_CREAT | O_APPEND, 0644);

    if (fd == -1)
    {
        perror("open");
        return;
    }

    printf("Enter ID: ");
    scanf("%d", &student.id);

    printf("Enter name: ");
    getchar(); 
    fgets(student.name, sizeof(student.name), stdin);
    student.name[strcspn(student.name, "\n")] = '\0';

    printf("Enter age: ");
    scanf("%d", &student.age);

    printf("Enter GPA: ");
    scanf("%f", &student.gpa);

    ssize_t bytes_written = 0;
    ssize_t total = sizeof(Student);

    while (bytes_written < total)
    {
        ssize_t ret = write(fd,
                            (char *)&student + bytes_written,
                            total - bytes_written);

        if (ret <= 0)
        {
            perror("write");
            break;
        }

        bytes_written += ret;
    }

    if (bytes_written != total)
    {
        printf("Write failed\n");
        close(fd);
        return;
    }

    printf("Student added successfully.\n");

    close(fd);
}

void list_students(void)
{
    int fd;
    Student student;

    fd = open("students.dat", O_RDONLY);

    if (fd == -1)
    {
        if (errno == ENOENT)
        {
            printf("No students yet.\n");
        }
        else
        {
            perror("open");
        }

        return;
    }

    while (read(fd, &student, sizeof(Student)) == sizeof(Student))
    {
        printf("ID: %d\n", student.id);
        printf("Name: %s\n", student.name);
        printf("Age: %d\n", student.age);
        printf("GPA: %.2f\n", student.gpa);
        printf("--------------------\n");
    }

    close(fd);
}

void find_student(void)
{
    int fd;
    int target_id;
    Student student;

    printf("Enter ID to find: ");
    scanf("%d", &target_id);

    fd = open("students.dat", O_RDONLY);

    if (fd == -1)
    {
        perror("open");
        return;
    }

    while (read(fd, &student, sizeof(Student)) == sizeof(Student))
    {
        if (student.id == target_id)
        {
            printf("ID: %d\n", student.id);
            printf("Name: %s\n", student.name);
            printf("Age: %d\n", student.age);
            printf("GPA: %.2f\n", student.gpa);

            close(fd);
            return;
        }
    }

    printf("Student not found.\n");

    close(fd);
}