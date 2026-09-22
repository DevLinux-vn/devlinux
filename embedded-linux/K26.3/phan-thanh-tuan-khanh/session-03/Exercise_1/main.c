#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define FILE_NAME "students.dat"

typedef struct {
    int id;
    char name[64];
    int age;
    float gpa;
} Student;

void addStudent(int fd)
{
    Student s;

    printf("Enter ID: ");
    scanf("%d", &s.id);

    printf("Enter name: ");
    scanf(" %63[^\n]", s.name);

    printf("Enter age: ");
    scanf("%d", &s.age);

    printf("Enter GPA: ");
    scanf("%f", &s.gpa);

    lseek(fd, 0, SEEK_END);

    if (write(fd, &s, sizeof(Student)) != sizeof(Student))
    {
        perror("write");
        return;
    }

    printf("Student added successfully.\n");
}

void listStudents(int fd)
{
    Student s;

    lseek(fd, 0, SEEK_SET);

    printf("\n===== Student List =====\n");

    while (read(fd, &s, sizeof(Student)) == sizeof(Student))
    {
        printf("ID: %d\n", s.id);
        printf("Name: %s\n", s.name);
        printf("Age: %d\n", s.age);
        printf("GPA: %.2f\n", s.gpa);
        printf("------------------------\n");
    }
}

void findStudent(int fd)
{
    int targetId;
    Student s;
    int found = 0;

    printf("Enter ID to find: ");
    scanf("%d", &targetId);

    lseek(fd, 0, SEEK_SET);

    while (read(fd, &s, sizeof(Student)) == sizeof(Student))
    {
        if (s.id == targetId)
        {
            printf("\nStudent Found:\n");
            printf("ID: %d\n", s.id);
            printf("Name: %s\n", s.name);
            printf("Age: %d\n", s.age);
            printf("GPA: %.2f\n", s.gpa);

            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("Student with ID %d not found.\n", targetId);
    }
}

int main()
{
    int choice;

    int fd = open(FILE_NAME,
                  O_RDWR | O_CREAT,
                  0644);

    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    while (1)
    {
        printf("\n===== MENU =====\n");
        printf("1. Add student\n");
        printf("2. List all students\n");
        printf("3. Find student by ID\n");
        printf("4. Exit\n");
        printf("Choose: ");

        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                addStudent(fd);
                break;

            case 2:
                listStudents(fd);
                break;

            case 3:
                findStudent(fd);
                break;

            case 4:
                close(fd);
                printf("Bye!\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }

    close(fd);
    return 0;
}
